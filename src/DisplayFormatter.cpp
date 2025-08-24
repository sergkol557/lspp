#include "DisplayFormatter.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <sys/ioctl.h>
#include <unistd.h>
#include <clocale>
#include <cwchar>

DisplayFormatter::DisplayFormatter(const LsOptions& options) 
    : m_options(options), m_icon_provider() {
    m_icon_provider.setColorEnabled(options.use_color);
    setlocale(LC_ALL, "");
}

void DisplayFormatter::displayFiles(const std::vector<FileInfo>& files, std::ostream& out) {
    if (files.empty()) {
        return;
    }
    
    switch (m_options.format) {
        case ListFormat::LONG:
            displayLongFormat(files, out);
            break;
        case ListFormat::ONE_PER_LINE:
            displayOnePerLine(files, out);
            break;
        case ListFormat::COMMAS:
            displayCommaSeparated(files, out);
            break;
        case ListFormat::ACROSS:
            displayAcross(files, out);
            break;
        case ListFormat::COLUMNS:
        case ListFormat::VERTICAL:
        default:
            displayColumnar(files, out);
            break;
    }
}

void DisplayFormatter::displayLongFormat(const std::vector<FileInfo>& files, std::ostream& out) {
    LongFormatWidths widths = calculateLongFormatWidths(files);
    
    // Show total blocks if -s option
    if (m_options.show_size) {
        off_t total_blocks = 0;
        for (const auto& file : files) {
            total_blocks += (file.size + 1023) / 1024; // Convert to 1K blocks
        }
        out << "total " << total_blocks << "\n";
    }
    
    for (const auto& file : files) {
        displaySingleFileLong(file, widths, out);
        out << "\n";
    }
}

void DisplayFormatter::displaySingleFileLong(const FileInfo& file, const LongFormatWidths& widths, std::ostream& out) const {
    // Inode number
    if (m_options.show_inode) {
        out << std::setw(widths.inode_width) << std::right << file.inode << " ";
    }
    
    // Block count
    if (m_options.show_size) {
        off_t blocks = (file.size + 1023) / 1024;
        out << std::setw(widths.blocks_width) << std::right << blocks << " ";
    }
    
    // File permissions
    if (m_options.use_color) {
        out << formatColoredPermissions(file.mode) << " ";
    } else {
        out << formatPermissions(file.mode) << " ";
    }
    
    // Number of hard links
    out << std::setw(widths.links_width) << std::right << file.hard_links << " ";
    
    // Owner
    if (m_options.numeric_uid_gid) {
        struct stat st;
        if (stat(file.path.c_str(), &st) == 0) {
            out << std::setw(widths.owner_width) << std::left << st.st_uid << " ";
        }
    } else {
        out << std::setw(widths.owner_width) << std::left << file.owner << " ";
    }
    
    // Group (unless -G option)
    if (m_options.numeric_uid_gid) {
        struct stat st;
        if (stat(file.path.c_str(), &st) == 0) {
            out << std::setw(widths.group_width) << std::left << st.st_gid << " ";
        }
    } else {
        out << std::setw(widths.group_width) << std::left << file.group << " ";
    }
    
    // File size
    std::string size_str;
    if (m_options.human_readable) {
        size_str = formatFileSize(file.size, true, m_options.si_units);
    } else {
        size_str = std::to_string(file.size);
    }
    out << std::setw(widths.size_width) << std::right << size_str << " ";
    
    // Modification time
    std::chrono::system_clock::time_point time_to_show;
    switch (m_options.time_type) {
        case TimeType::ATIME:
            time_to_show = file.atime;
            break;
        case TimeType::CTIME:
            time_to_show = file.ctime;
            break;
        case TimeType::BTIME:
        case TimeType::MTIME:
        default:
            time_to_show = file.mtime;
            break;
    }
    
    out << formatTime(time_to_show, m_options.time_style) << " ";
    
    // File name with icon and color
    out << getIconAndColor(file) << formatFileName(file);
    
    // SELinux context
    if (m_options.show_context && !file.selinux_context.empty()) {
        out << " " << file.selinux_context;
    }
    
    resetColor(out);
}

void DisplayFormatter::displayColumnar(const std::vector<FileInfo>& files, std::ostream& out) {
    if (files.empty()) return;
    
    int terminal_width = m_options.width > 0 ? m_options.width : getTerminalWidth();
    ColumnLayout layout = calculateLayout(files, terminal_width);
    
    std::vector<std::string> formatted_names = formatFilesForLayout(files);
    
    for (size_t row = 0; row < layout.rows; ++row) {
        for (size_t col = 0; col < layout.cols; ++col) {
            size_t index = col * layout.rows + row;
            
            if (index < files.size()) {
                // Show inode if requested
                if (m_options.show_inode) {
                    out << std::setw(8) << files[index].inode << " ";
                }
                
                // Show block size if requested
                if (m_options.show_size) {
                    off_t blocks = (files[index].size + 1023) / 1024;
                    out << std::setw(6) << blocks << " ";
                }
                
                out << getIconAndColor(files[index]) << formatted_names[index];
                resetColor(out);
                
                // Add padding except for last column
                if (col < layout.cols - 1) {
                    size_t name_width = getDisplayWidth(formatted_names[index]);
                    size_t padding = layout.col_widths[col] + 2 - name_width;
                    out << std::string(padding, ' ');
                }
            }
        }
        out << "\n";
    }
}

void DisplayFormatter::displayOnePerLine(const std::vector<FileInfo>& files, std::ostream& out) {
    for (const auto& file : files) {
        // Show inode if requested
        if (m_options.show_inode) {
            out << std::setw(8) << file.inode << " ";
        }
        
        // Show block size if requested
        if (m_options.show_size) {
            off_t blocks = (file.size + 1023) / 1024;
            out << std::setw(6) << blocks << " ";
        }
        
        out << getIconAndColor(file) << formatFileName(file);
        resetColor(out);
        out << "\n";
    }
}

void DisplayFormatter::displayCommaSeparated(const std::vector<FileInfo>& files, std::ostream& out) {
    for (size_t i = 0; i < files.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        
        out << getIconAndColor(files[i]) << formatFileName(files[i]);
        resetColor(out);
    }
    out << "\n";
}

void DisplayFormatter::displayAcross(const std::vector<FileInfo>& files, std::ostream& out) {
    if (files.empty()) return;
    
    int terminal_width = m_options.width > 0 ? m_options.width : getTerminalWidth();
    std::vector<std::string> formatted_names = formatFilesForLayout(files);
    
    // Calculate maximum width needed
    size_t max_width = 0;
    for (const auto& name : formatted_names) {
        max_width = std::max<size_t>(max_width, getDisplayWidth(name));
    }
    
    size_t cols_per_row = std::max<size_t>(1, terminal_width / (max_width + 2));
    
    for (size_t i = 0; i < files.size(); ++i) {
        if (i > 0 && i % cols_per_row == 0) {
            out << "\n";
        }
        
        // Show inode if requested
        if (m_options.show_inode) {
            out << std::setw(8) << files[i].inode << " ";
        }
        
        // Show block size if requested
        if (m_options.show_size) {
            off_t blocks = (files[i].size + 1023) / 1024;
            out << std::setw(6) << blocks << " ";
        }
        
        out << getIconAndColor(files[i]) << formatted_names[i];
        resetColor(out);
        
        // Add padding except for last item in row
        if ((i + 1) % cols_per_row != 0 && i + 1 < files.size()) {
            size_t padding = max_width + 2 - getDisplayWidth(formatted_names[i]);
            out << std::string(padding, ' ');
        }
    }
    out << "\n";
}

std::string DisplayFormatter::formatFileName(const FileInfo& file) const {
    std::string name = file.display_name.string();
    
    if (m_options.quote_names) {
        return quoteFileName(name);
    }
    
    if (m_options.escape_names) {
        return escapeFileName(name);
    }
    
    if (!m_options.literal_names && !m_options.show_control_chars) {
        // Replace control characters with ?
        for (char& c : name) {
            if (std::iscntrl(c)) {
                c = '?';
            }
        }
    }
    
    // Add file type indicators
    if (m_options.show_file_type || m_options.show_indicators) {
        if (file.is_directory) {
            name += "/";
        } else if (file.is_symlink) {
            name += "@";
            if (!file.symlink_target.empty()) {
                name += " -> " + file.symlink_target;
            }
        } else if (file.is_executable && m_options.show_file_type) {
            name += "*";
        }
    }
    
    return name;
}

std::string DisplayFormatter::formatFileSize(off_t size, bool human_readable, bool si_units) const {
    if (!human_readable) {
        return std::to_string(size);
    }
    
    const std::vector<std::string> suffixes_1024 = {"", "K", "M", "G", "T", "P", "E", "Z", "Y"};
    const std::vector<std::string> suffixes_1000 = {"", "k", "M", "G", "T", "P", "E", "Z", "Y"};
    
    const auto& suffixes = si_units ? suffixes_1000 : suffixes_1024;
    const double base = si_units ? 1000.0 : 1024.0;
    
    if (size == 0) {
        return "0";
    }
    
    double fsize = static_cast<double>(size);
    size_t suffix_index = 0;
    
    while (fsize >= base && suffix_index < suffixes.size() - 1) {
        fsize /= base;
        suffix_index++;
    }
    
    std::ostringstream ss;
    if (suffix_index == 0) {
        ss << static_cast<long long>(fsize);
    } else if (fsize < 10.0) {
        ss << std::fixed << std::setprecision(1) << fsize;
    } else {
        ss << std::fixed << std::setprecision(0) << fsize;
    }
    
    ss << suffixes[suffix_index];
    return ss.str();
}

std::string DisplayFormatter::formatTime(const std::chrono::system_clock::time_point& time, const std::string& style) const {
    auto time_t_val = std::chrono::system_clock::to_time_t(time);
    struct tm* tm_info = localtime(&time_t_val);
    
    std::ostringstream ss;
    
    if (style == "full-iso" || m_options.full_time) {
        ss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S.%f %z");
    } else if (style == "long-iso") {
        ss << std::put_time(tm_info, "%Y-%m-%d %H:%M");
    } else if (style == "iso") {
        ss << std::put_time(tm_info, "%m-%d %H:%M");
    } else {
        // Default locale style
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        struct tm* now_tm = localtime(&now_time_t);
        
        // If file is older than 6 months or in the future, show year
        auto six_months_ago = now - std::chrono::hours(24 * 30 * 6);
        
        if (time < six_months_ago || time > now) {
            ss << std::put_time(tm_info, "%b %d  %Y");
        } else {
            ss << std::put_time(tm_info, "%b %d %H:%M");
        }
    }
    
    return ss.str();
}

std::string DisplayFormatter::formatPermissions(mode_t mode) const {
    std::string perms(10, '-');
    
    // File type
    if (S_ISDIR(mode)) perms[0] = 'd';
    else if (S_ISLNK(mode)) perms[0] = 'l';
    else if (S_ISBLK(mode)) perms[0] = 'b';
    else if (S_ISCHR(mode)) perms[0] = 'c';
    else if (S_ISFIFO(mode)) perms[0] = 'p';
    else if (S_ISSOCK(mode)) perms[0] = 's';
    
    // Owner permissions
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    
    // Group permissions
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    
    // Other permissions
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    
    // Special bits
    if (mode & S_ISUID) {
        perms[3] = (mode & S_IXUSR) ? 's' : 'S';
    }
    if (mode & S_ISGID) {
        perms[6] = (mode & S_IXGRP) ? 's' : 'S';
    }
    if (mode & S_ISVTX) {
        perms[9] = (mode & S_IXOTH) ? 't' : 'T';
    }
    
    return perms;
}

std::string DisplayFormatter::formatColoredPermissions(mode_t mode) const {
    std::string perms(10, '-');
    std::string result;
    
    // Color codes for permissions
    const std::string GREEN = "\033[32m";   // Green for read
    const std::string YELLOW = "\033[33m";  // Yellow for write
    const std::string RED = "\033[31m";     // Red for execute
    const std::string RESET = "\033[m";     // Reset color
    
    // File type (no color)
    if (S_ISDIR(mode)) perms[0] = 'd';
    else if (S_ISLNK(mode)) perms[0] = 'l';
    else if (S_ISBLK(mode)) perms[0] = 'b';
    else if (S_ISCHR(mode)) perms[0] = 'c';
    else if (S_ISFIFO(mode)) perms[0] = 'p';
    else if (S_ISSOCK(mode)) perms[0] = 's';
    
    // Owner permissions
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    
    // Group permissions
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    
    // Other permissions
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    
    // Special bits
    if (mode & S_ISUID) {
        perms[3] = (mode & S_IXUSR) ? 's' : 'S';
    }
    if (mode & S_ISGID) {
        perms[6] = (mode & S_IXGRP) ? 's' : 'S';
    }
    if (mode & S_ISVTX) {
        perms[9] = (mode & S_IXOTH) ? 't' : 'T';
    }
    
    // Apply colors to each character
    result += perms[0]; // File type without color
    
    // Owner permissions
    result += (perms[1] != '-') ? GREEN + perms[1] + RESET : std::string(1, perms[1]);
    result += (perms[2] != '-') ? YELLOW + perms[2] + RESET : std::string(1, perms[2]);
    result += (perms[3] != '-' && perms[3] != 'S') ? RED + perms[3] + RESET : std::string(1, perms[3]);
    
    // Group permissions
    result += (perms[4] != '-') ? GREEN + perms[4] + RESET : std::string(1, perms[4]);
    result += (perms[5] != '-') ? YELLOW + perms[5] + RESET : std::string(1, perms[5]);
    result += (perms[6] != '-' && perms[6] != 'S') ? RED + perms[6] + RESET : std::string(1, perms[6]);
    
    // Other permissions
    result += (perms[7] != '-') ? GREEN + perms[7] + RESET : std::string(1, perms[7]);
    result += (perms[8] != '-') ? YELLOW + perms[8] + RESET : std::string(1, perms[8]);
    result += (perms[9] != '-' && perms[9] != 'T') ? RED + perms[9] + RESET : std::string(1, perms[9]);
    
    return result;
}

std::string DisplayFormatter::getIconAndColor(const FileInfo& file) const {
    if (!m_options.use_color) {
        return m_icon_provider.getIcon(file) + " ";
    }
    
    auto [icon, color] = m_icon_provider.getIconAndColor(file);
    return color + icon + " ";
}

void DisplayFormatter::resetColor(std::ostream& out) const {
    if (m_options.use_color) {
        out << IconProvider::RESET_COLOR;
    }
}

std::string DisplayFormatter::escapeFileName(const std::string& name) const {
    std::string escaped;
    for (char c : name) {
        switch (c) {
            case '\n': escaped += "\\n"; break;
            case '\t': escaped += "\\t"; break;
            case '\r': escaped += "\\r"; break;
            case '\\': escaped += "\\\\"; break;
            case ' ':  escaped += "\\ "; break;
            default:
                if (std::iscntrl(c)) {
                    escaped += "\\";
                    escaped += ('0' + ((c >> 6) & 7));
                    escaped += ('0' + ((c >> 3) & 7));
                    escaped += ('0' + (c & 7));
                } else {
                    escaped += c;
                }
                break;
        }
    }
    return escaped;
}

std::string DisplayFormatter::quoteFileName(const std::string& name) const {
    return "\"" + name + "\"";
}

DisplayFormatter::ColumnLayout DisplayFormatter::calculateLayout(const std::vector<FileInfo>& files, int terminal_width) const {
    if (files.empty()) {
        return {0, 0, {}, 0};
    }
    
    std::vector<std::string> formatted_names = formatFilesForLayout(files);
    std::vector<size_t> name_widths;
    
    for (const auto& name : formatted_names) {
        name_widths.push_back(getDisplayWidth(name));
    }
    
    size_t total_files = files.size();
    ColumnLayout best_layout = {total_files, 1, {*std::max_element(name_widths.begin(), name_widths.end())}, 0};
    
    // Try different numbers of rows
    for (size_t rows = 1; rows <= total_files; ++rows) {
        size_t cols = (total_files + rows - 1) / rows;
        std::vector<size_t> col_widths(cols, 0);
        
        // Calculate column widths
        for (size_t i = 0; i < total_files; ++i) {
            size_t col = i / rows;
            if (col < cols) {
                col_widths[col] = std::max(col_widths[col], name_widths[i]);
            }
        }
        
        // Calculate total width
        size_t total_width = 0;
        for (size_t w : col_widths) {
            total_width += w + 2; // 2 spaces between columns
        }
        total_width -= 2; // No trailing spaces
        
        if (total_width <= static_cast<size_t>(terminal_width)) {
            best_layout = {rows, cols, col_widths, total_width};
            break;
        }
    }
    
    return best_layout;
}

std::vector<std::string> DisplayFormatter::formatFilesForLayout(const std::vector<FileInfo>& files) const {
    std::vector<std::string> formatted_names;
    formatted_names.reserve(files.size());
    
    for (const auto& file : files) {
        formatted_names.push_back(formatFileName(file));
    }
    
    return formatted_names;
}

DisplayFormatter::LongFormatWidths DisplayFormatter::calculateLongFormatWidths(const std::vector<FileInfo>& files) const {
    LongFormatWidths widths;
    
    for (const auto& file : files) {
        if (m_options.show_inode) {
            widths.inode_width = std::max(widths.inode_width, std::to_string(file.inode).length());
        }
        
        if (m_options.show_size) {
            off_t blocks = (file.size + 1023) / 1024;
            widths.blocks_width = std::max(widths.blocks_width, std::to_string(blocks).length());
        }
        
        widths.links_width = std::max(widths.links_width, std::to_string(file.hard_links).length());
        widths.owner_width = std::max(widths.owner_width, file.owner.length());
        widths.group_width = std::max(widths.group_width, file.group.length());
        
        std::string size_str;
        if (m_options.human_readable) {
            size_str = formatFileSize(file.size, true, m_options.si_units);
        } else {
            size_str = std::to_string(file.size);
        }
        widths.size_width = std::max(widths.size_width, size_str.length());
    }
    
    return widths;
}

int DisplayFormatter::getTerminalWidth() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
        return w.ws_col;
    }
    return 80; // Default fallback
}

size_t DisplayFormatter::getDisplayWidth(const std::string& str) {
    size_t width = 0;
    const char* ptr = str.c_str();
    size_t remaining = str.size();
    std::mbstate_t state = std::mbstate_t();
    
    while (remaining > 0) {
        wchar_t wc;
        size_t len = std::mbrtowc(&wc, ptr, remaining, &state);
        
        if (len == static_cast<size_t>(-1) || len == static_cast<size_t>(-2)) {
            // Invalid sequence, count as 1
            width += remaining;
            break;
        } else if (len == 0) {
            len = 1;
        }
        
        int w = wcwidth(wc);
        width += (w >= 0) ? static_cast<size_t>(w) : 1;
        ptr += len;
        remaining -= len;
    }
    
    return width;
}