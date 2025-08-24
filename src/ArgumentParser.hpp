#pragma once

#include <string>
#include <vector>
#include <unordered_set>

enum class SortOrder {
    NAME,
    TIME,
    SIZE,
    VERSION,
    EXTENSION,
    NONE
};

enum class TimeType {
    MTIME,  // modification time (default)
    ATIME,  // access time
    CTIME,  // status change time
    BTIME   // birth time (creation)
};

enum class ListFormat {
    COLUMNS,     // default multi-column
    LONG,        // -l format
    ONE_PER_LINE, // -1 format
    COMMAS,      // -m format
    ACROSS,      // -x format
    VERTICAL     // -C format (explicit)
};

struct LsOptions {
    // Display options
    bool show_all = false;              // -a, --all
    bool show_almost_all = false;       // -A, --almost-all
    bool ignore_backups = false;        // -B, --ignore-backups
    bool use_color = true;              // --color=auto/always/never
    bool show_control_chars = false;    // -q, --hide-control-chars
    bool show_directory_entries = false; // -d, --directory
    bool escape_names = false;          // -b, --escape
    bool quote_names = false;           // -Q, --quote-name
    bool literal_names = false;         // -N, --literal
    bool show_file_type = false;        // -F, --classify
    bool show_indicators = false;       // -p, --indicator-style=slash
    bool group_directories_first = false; // --group-directories-first
    bool show_hidden = false;           // show hidden files
    bool numeric_uid_gid = false;       // -n, --numeric-uid-gid
    bool dereference_links = false;     // -L, --dereference
    bool no_dereference = false;        // -P, --no-dereference
    bool follow_symlinks = false;       // -H, --dereference-command-line
    bool show_inode = false;            // -i, --inode
    bool show_size = false;             // -s, --size
    bool human_readable = false;        // -h, --human-readable
    bool si_units = false;              // --si
    bool block_size_1024 = false;       // -k, --kibibytes
    bool recursive = false;             // -R, --recursive
    bool reverse_order = false;         // -r, --reverse
    bool ignore_case = false;           // ignore case in sorting
    bool show_author = false;           // --author
    bool full_time = false;             // --full-time
    bool show_context = false;          // -Z, --context (SELinux)
    
    // Sorting options
    SortOrder sort_order = SortOrder::NAME;
    TimeType time_type = TimeType::MTIME;
    bool sort_reverse = false;
    
    // Format options
    ListFormat format = ListFormat::COLUMNS;
    
    // Filter options
    std::vector<std::string> ignore_patterns;  // --ignore
    std::vector<std::string> hide_patterns;    // --hide
    
    // Size and formatting
    int tab_size = 8;                   // -T, --tabsize
    int width = 0;                      // -w, --width
    std::string time_style = "locale";  // --time-style
    std::string block_size = "1024";    // --block-size
    
    // Paths to process
    std::vector<std::string> paths;
};

class ArgumentParser {
public:
    ArgumentParser();
    
    LsOptions parse(int argc, char** argv);
    void printHelp() const;
    void printVersion() const;
    
private:
    void parseShortOption(const std::string& arg, LsOptions& options, size_t& pos);
    void parseLongOption(const std::string& arg, LsOptions& options);
    bool isOption(const std::string& arg) const;
    void handleColorOption(const std::string& value, LsOptions& options);
    void handleTimeStyleOption(const std::string& value, LsOptions& options);
    void handleSortOption(const std::string& value, LsOptions& options);
    void handleFormatOption(const std::string& value, LsOptions& options);
    
    std::unordered_set<char> m_valid_short_options;
    std::unordered_set<std::string> m_valid_long_options;
};