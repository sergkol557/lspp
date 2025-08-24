#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <sys/stat.h>
#include "ArgumentParser.hpp"

namespace fs = std::filesystem;

struct FileInfo {
    fs::path path;
    fs::path display_name;
    std::string owner;
    std::string group;
    mode_t mode;
    off_t size;
    nlink_t hard_links;
    ino_t inode;
    std::chrono::system_clock::time_point mtime;
    std::chrono::system_clock::time_point atime;
    std::chrono::system_clock::time_point ctime;
    bool is_directory;
    bool is_symlink;
    bool is_executable;
    bool is_hidden;
    std::string symlink_target;
    std::string selinux_context;
    
    FileInfo(const fs::path& p);
    
private:
    void loadFileStats();
    void loadExtendedInfo();
};

class FileOperations {
public:
    FileOperations();
    
    std::vector<FileInfo> listDirectory(const fs::path& path, const LsOptions& options);
    std::vector<FileInfo> processTargets(const std::vector<std::string>& targets, const LsOptions& options);
    
    void sortFiles(std::vector<FileInfo>& files, const LsOptions& options);
    std::vector<FileInfo> filterFiles(const std::vector<FileInfo>& files, const LsOptions& options);
    
    static bool isHidden(const std::string& name);
    static bool isBackupFile(const std::string& name);
    static bool matchesPattern(const std::string& name, const std::string& pattern);
    static std::string getFileOwner(uid_t uid, bool numeric = false);
    static std::string getFileGroup(gid_t gid, bool numeric = false);
    static std::string getSymlinkTarget(const fs::path& path);
    static std::string getSelinuxContext(const fs::path& path);
    
private:
    void processDirectory(const fs::path& dir_path, const LsOptions& options, 
                         std::vector<FileInfo>& results, bool show_header = false);
    void processDirectoryRecursive(const fs::path& dir_path, const LsOptions& options, 
                                  std::vector<FileInfo>& results);
    void processFile(const fs::path& file_path, std::vector<FileInfo>& results);
    
    bool shouldShowFile(const FileInfo& file, const LsOptions& options) const;
    
    static bool compareByName(const FileInfo& a, const FileInfo& b, bool ignore_case = false);
    static bool compareByTime(const FileInfo& a, const FileInfo& b, TimeType time_type);
    static bool compareBySize(const FileInfo& a, const FileInfo& b);
    static bool compareByExtension(const FileInfo& a, const FileInfo& b);
    static bool compareByVersion(const FileInfo& a, const FileInfo& b);
};