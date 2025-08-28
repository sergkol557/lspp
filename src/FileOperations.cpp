#include "FileOperations.hpp"
#include <iostream>
#include <algorithm>
#include <fnmatch.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctime>
#include <locale>
#include <iomanip>
#include <sstream>

FileInfo::FileInfo(const fs::path& p) : path(p), display_name(p.filename()) {
    loadFileStats();
    loadExtendedInfo();
}

void FileInfo::loadFileStats() {
    try {
        struct stat st;
        if (lstat(path.c_str(), &st) == 0) {
            mode = st.st_mode;
            size = st.st_size;
            hard_links = st.st_nlink;
            inode = st.st_ino;
            
            // Convert time_t to chrono time_point
            mtime = std::chrono::system_clock::from_time_t(st.st_mtime);
            atime = std::chrono::system_clock::from_time_t(st.st_atime);
            ctime = std::chrono::system_clock::from_time_t(st.st_ctime);
            
            is_directory = S_ISDIR(mode);
            is_symlink = S_ISLNK(mode);
            is_executable = (mode & S_IXUSR) || (mode & S_IXGRP) || (mode & S_IXOTH);
            is_hidden = display_name.string().length() > 0 && display_name.string()[0] == '.';
            
            owner = FileOperations::getFileOwner(st.st_uid);
            group = FileOperations::getFileGroup(st.st_gid);
        }
    } catch (const std::exception& e) {
        // Handle error - set default values
        mode = 0;
        size = 0;
        hard_links = 0;
        inode = 0;
        is_directory = false;
        is_symlink = false;
        is_executable = false;
        is_hidden = false;
        owner = "unknown";
        group = "unknown";
    }
}

void FileInfo::loadExtendedInfo() {
    if (is_symlink) {
        symlink_target = FileOperations::getSymlinkTarget(path);
    }
    
    // Load SELinux context if requested
    selinux_context = FileOperations::getSelinuxContext(path);
}

FileOperations::FileOperations() {
    std::locale::global(std::locale(""));
}

std::vector<FileInfo> FileOperations::listDirectory(const fs::path& path, const LsOptions& options) {
    std::vector<FileInfo> files;
    
    try {
        if (options.show_directory_entries) {
            // Just show the directory itself, not its contents
            files.emplace_back(path);
        } else {
            for (const auto& entry : fs::directory_iterator(path)) {
                files.emplace_back(entry.path());
            }
            
            // Add . and .. if showing all
            if (options.show_all || options.show_almost_all) {
                if (options.show_all) {
                    files.emplace_back(path / ".");
                }
                files.emplace_back(path / "..");
            }
        }
        
        files = filterFiles(files, options);
        sortFiles(files, options);
        
    } catch (const fs::filesystem_error& e) {
        std::cerr << "ls++: cannot access '" << path.string() << "': " << e.what() << "\n";
    }
    
    return files;
}

std::vector<FileInfo> FileOperations::processTargets(const std::vector<std::string>& targets, const LsOptions& options) {
    std::vector<FileInfo> all_files;
    std::vector<fs::path> directories;
    std::vector<fs::path> files;
    
    // Separate files and directories
    for (const auto& target : targets) {
        fs::path p(target);
        
        if (fs::exists(p)) {
            if (fs::is_directory(p) && !options.show_directory_entries) {
                directories.push_back(p);
            } else {
                files.push_back(p);
            }
        } else {
            std::cerr << "ls++: cannot access '" << target << "': No such file or directory\n";
        }
    }
    
    // Process files first
    for (const auto& file : files) {
        all_files.emplace_back(file);
    }
    
    // Process directories
    bool show_headers = (files.size() + directories.size()) > 1;
    bool first_dir = files.empty();
    
    for (const auto& dir : directories) {
        if (show_headers) {
            if (!first_dir) {
                std::cout << "\n";
            }
            std::cout << dir.string() << ":\n";
            first_dir = false;
        }
        
        auto dir_files = listDirectory(dir, options);
        
        if (options.recursive) {
            processDirectoryRecursive(dir, options, dir_files);
        }
        
        all_files.insert(all_files.end(), dir_files.begin(), dir_files.end());
    }
    
    return all_files;
}

void FileOperations::processDirectoryRecursive(const fs::path& dir_path, const LsOptions& options, std::vector<FileInfo>& results) {
    try {
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            if (fs::is_directory(entry) && !isHidden(entry.path().filename().string())) {
                std::cout << "\n" << entry.path().string() << ":\n";
                
                auto subdir_files = listDirectory(entry.path(), options);
                results.insert(results.end(), subdir_files.begin(), subdir_files.end());
                
                // Recursive call
                processDirectoryRecursive(entry.path(), options, results);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "ls++: cannot read directory '" << dir_path.string() << "': " << e.what() << "\n";
    }
}

std::vector<FileInfo> FileOperations::filterFiles(const std::vector<FileInfo>& files, const LsOptions& options) {
    std::vector<FileInfo> filtered;
    
    for (const auto& file : files) {
        if (shouldShowFile(file, options)) {
            filtered.push_back(file);
        }
    }
    
    return filtered;
}

bool FileOperations::shouldShowFile(const FileInfo& file, const LsOptions& options) const {
    const std::string& name = file.display_name.string();
    
    // Handle hidden files
    if (file.is_hidden) {
        if (!options.show_all && !options.show_almost_all) {
            return false;
        }
        if (options.show_almost_all && (name == "." || name == "..")) {
            return false;
        }
    }
    
    // Handle backup files
    if (options.ignore_backups && isBackupFile(name)) {
        return false;
    }
    
    // Handle ignore patterns
    for (const auto& pattern : options.ignore_patterns) {
        if (matchesPattern(name, pattern)) {
            return false;
        }
    }
    
    // Handle hide patterns
    for (const auto& pattern : options.hide_patterns) {
        if (matchesPattern(name, pattern)) {
            return false;
        }
    }
    
    return true;
}

void FileOperations::sortFiles(std::vector<FileInfo>& files, const LsOptions& options) {
    if (options.sort_order == SortOrder::NONE) {
        return;
    }
    
    // Primary sort
    switch (options.sort_order) {
        case SortOrder::NAME:
            std::sort(files.begin(), files.end(), [&](const FileInfo& a, const FileInfo& b) {
                // If group_directories_first is set, directories should come first
                if (options.group_directories_first && a.is_directory != b.is_directory) {
                    return a.is_directory > b.is_directory;
                }
                return compareByName(a, b, options.ignore_case);
            });
            break;
        case SortOrder::TIME:
            std::sort(files.begin(), files.end(), [&](const FileInfo& a, const FileInfo& b) {
                // If group_directories_first is set, directories should come first
                if (options.group_directories_first && a.is_directory != b.is_directory) {
                    return a.is_directory > b.is_directory;
                }
                return compareByTime(a, b, options.time_type);
            });
            break;
        case SortOrder::SIZE:
            std::sort(files.begin(), files.end(), [&](const FileInfo& a, const FileInfo& b) {
                // If group_directories_first is set, directories should come first
                if (options.group_directories_first && a.is_directory != b.is_directory) {
                    return a.is_directory > b.is_directory;
                }
                return compareBySize(a, b);
            });
            break;
        case SortOrder::EXTENSION:
            std::sort(files.begin(), files.end(), [&](const FileInfo& a, const FileInfo& b) {
                // If group_directories_first is set, directories should come first
                if (options.group_directories_first && a.is_directory != b.is_directory) {
                    return a.is_directory > b.is_directory;
                }
                return compareByExtension(a, b);
            });
            break;
        case SortOrder::VERSION:
            std::sort(files.begin(), files.end(), [&](const FileInfo& a, const FileInfo& b) {
                // If group_directories_first is set, directories should come first
                if (options.group_directories_first && a.is_directory != b.is_directory) {
                    return a.is_directory > b.is_directory;
                }
                return compareByVersion(a, b);
            });
            break;
        case SortOrder::NONE:
            break;
    }
    
    // Reverse if requested
    if (options.reverse_order) {
        std::reverse(files.begin(), files.end());
    }
}

bool FileOperations::compareByName(const FileInfo& a, const FileInfo& b, bool ignore_case) {
    std::locale loc("");
    const std::string& an = a.display_name.string();
    const std::string& bn = b.display_name.string();
    
    if (ignore_case) {
        std::string a_lower = an;
        std::string b_lower = bn;
        std::transform(a_lower.begin(), a_lower.end(), a_lower.begin(), ::tolower);
        std::transform(b_lower.begin(), b_lower.end(), b_lower.begin(), ::tolower);
        return std::use_facet<std::collate<char>>(loc).compare(
            a_lower.data(), a_lower.data() + a_lower.size(),
            b_lower.data(), b_lower.data() + b_lower.size()) < 0;
    } else {
        return std::use_facet<std::collate<char>>(loc).compare(
            an.data(), an.data() + an.size(),
            bn.data(), bn.data() + bn.size()) < 0;
    }
}

bool FileOperations::compareByTime(const FileInfo& a, const FileInfo& b, TimeType time_type) {
    switch (time_type) {
        case TimeType::MTIME:
            return a.mtime > b.mtime;
        case TimeType::ATIME:
            return a.atime > b.atime;
        case TimeType::CTIME:
            return a.ctime > b.ctime;
        case TimeType::BTIME:
            // Birth time not easily available on all systems, fall back to mtime
            return a.mtime > b.mtime;
        default:
            return a.mtime > b.mtime;
    }
}

bool FileOperations::compareBySize(const FileInfo& a, const FileInfo& b) {
    return a.size > b.size;
}

bool FileOperations::compareByExtension(const FileInfo& a, const FileInfo& b) {
    std::string ext_a = a.path.extension().string();
    std::string ext_b = b.path.extension().string();
    
    if (ext_a != ext_b) {
        return ext_a < ext_b;
    }
    
    // If extensions are the same, sort by name
    return compareByName(a, b);
}

bool FileOperations::compareByVersion(const FileInfo& a, const FileInfo& b) {
    // Simple version comparison - in a full implementation, this would use strverscmp
    return compareByName(a, b);
}

bool FileOperations::isHidden(const std::string& name) {
    return !name.empty() && name[0] == '.';
}

bool FileOperations::isBackupFile(const std::string& name) {
    return !name.empty() && name.back() == '~';
}

bool FileOperations::matchesPattern(const std::string& name, const std::string& pattern) {
    return fnmatch(pattern.c_str(), name.c_str(), 0) == 0;
}

std::string FileOperations::getFileOwner(uid_t uid, bool numeric) {
    if (numeric) {
        return std::to_string(uid);
    }
    
    struct passwd* pw = getpwuid(uid);
    if (pw && pw->pw_name) {
        return std::string(pw->pw_name);
    }
    
    return std::to_string(uid);
}

std::string FileOperations::getFileGroup(gid_t gid, bool numeric) {
    if (numeric) {
        return std::to_string(gid);
    }
    
    struct group* gr = getgrgid(gid);
    if (gr && gr->gr_name) {
        return std::string(gr->gr_name);
    }
    
    return std::to_string(gid);
}

std::string FileOperations::getSymlinkTarget(const fs::path& path) {
    try {
        return fs::read_symlink(path).string();
    } catch (const fs::filesystem_error&) {
        return "";
    }
}

std::string FileOperations::getSelinuxContext(const fs::path& path) {
    // SELinux context retrieval would require libselinux
    // For now, return empty string
    return "";
}