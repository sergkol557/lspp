#include "ArgumentParser.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <string>

const std::string LSPP_VERSION = "ls++ v1.0.0";

ArgumentParser::ArgumentParser() {
    // Initialize valid short options
    m_valid_short_options = {
        'a', 'A', 'b', 'B', 'c', 'C', 'd', 'f', 'F', 'g', 'G', 'h', 'H',
        'i', 'I', 'k', 'l', 'L', 'm', 'n', 'N', 'o', 'p', 'P', 'q', 'Q',
        'r', 'R', 's', 'S', 't', 'T', 'u', 'U', 'v', 'w', 'x', 'X', 'Z', '1'
    };
    
    // Initialize valid long options
    m_valid_long_options = {
        "all", "almost-all", "author", "escape", "ignore-backups", "block-size",
        "ignore", "directory", "classify", "file-type", "format", "full-time",
        "group-directories-first", "no-group", "human-readable", "si",
        "dereference-command-line", "hide", "indicator-style", "inode",
        "kibibytes", "dereference", "literal", "format", "comma-separated",
        "numeric-uid-gid", "no-dereference", "indicator-style", "hide-control-chars",
        "show-control-chars", "quote-name", "quoting-style", "reverse", "recursive",
        "size", "sort", "time", "time-style", "tabsize", "time", "version",
        "width", "context", "help", "version", "color", "hyperlink", "zero", "long"
    };
}

LsOptions ArgumentParser::parse(int argc, char** argv) {
    LsOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            std::exit(0);
        } else if (arg == "--version") {
            printVersion();
            std::exit(0);
        } else if (arg.substr(0, 2) == "--") {
            parseLongOption(arg, options);
        } else if (arg.length() > 1 && arg[0] == '-') {
            size_t pos = 1;
            parseShortOption(arg, options, pos);
        } else {
            // Non-option argument - add to paths
            options.paths.push_back(arg);
        }
    }
    
    // If no paths specified, use current directory
    if (options.paths.empty()) {
        options.paths.push_back(".");
    }
    
    // Handle conflicting options
    if (options.show_all) {
        options.show_almost_all = false;
    }
    
    // Set default format based on output
    if (options.format == ListFormat::COLUMNS && !isatty(STDOUT_FILENO)) {
        options.format = ListFormat::ONE_PER_LINE;
    }
    
    return options;
}

void ArgumentParser::parseShortOption(const std::string& arg, LsOptions& options, size_t& pos) {
    while (pos < arg.length()) {
        char opt = arg[pos++];
        
        switch (opt) {
            case 'a':
                options.show_all = true;
                break;
            case 'A':
                options.show_almost_all = true;
                break;
            case 'b':
                options.escape_names = true;
                break;
            case 'B':
                options.ignore_backups = true;
                break;
            case 'c':
                options.time_type = TimeType::CTIME;
                options.sort_order = SortOrder::TIME;
                break;
            case 'C':
                options.format = ListFormat::VERTICAL;
                break;
            case 'd':
                options.show_directory_entries = true;
                break;
            case 'f':
                options.show_all = true;
                options.sort_order = SortOrder::NONE;
                break;
            case 'F':
                options.show_file_type = true;
                break;
            case 'g':
                options.format = ListFormat::LONG;
                // Don't show owner in long format
                break;
            case 'G':
                options.use_color = false;
                break;
            case 'h':
                options.human_readable = true;
                break;
            case 'H':
                options.follow_symlinks = true;
                break;
            case 'i':
                options.show_inode = true;
                break;
            case 'I':
                // Ignore pattern - would need next argument
                // For now, skip this implementation
                break;
            case 'k':
                options.block_size_1024 = true;
                break;
            case 'l':
                options.format = ListFormat::LONG;
                break;
            case 'L':
                options.dereference_links = true;
                break;
            case 'm':
                options.format = ListFormat::COMMAS;
                break;
            case 'n':
                options.numeric_uid_gid = true;
                options.format = ListFormat::LONG;
                break;
            case 'N':
                options.literal_names = true;
                break;
            case 'o':
                options.format = ListFormat::LONG;
                // Don't show group in long format
                break;
            case 'p':
                options.show_indicators = true;
                break;
            case 'P':
                options.no_dereference = true;
                break;
            case 'q':
                options.show_control_chars = false;
                break;
            case 'Q':
                options.quote_names = true;
                break;
            case 'r':
                options.reverse_order = true;
                break;
            case 'R':
                options.recursive = true;
                break;
            case 's':
                options.show_size = true;
                break;
            case 'S':
                options.sort_order = SortOrder::SIZE;
                break;
            case 't':
                options.sort_order = SortOrder::TIME;
                break;
            case 'T':
                // Tab size - would need next argument
                break;
            case 'u':
                options.time_type = TimeType::ATIME;
                options.sort_order = SortOrder::TIME;
                break;
            case 'U':
                options.sort_order = SortOrder::NONE;
                break;
            case 'v':
                options.sort_order = SortOrder::VERSION;
                break;
            case 'w':
                // Width - would need next argument
                break;
            case 'x':
                options.format = ListFormat::ACROSS;
                break;
            case 'X':
                options.sort_order = SortOrder::EXTENSION;
                break;
            case 'Z':
                options.show_context = true;
                break;
            case '1':
                options.format = ListFormat::ONE_PER_LINE;
                break;
            default:
                std::cerr << "ls++: invalid option -- '" << opt << "'\n";
                std::cerr << "Try 'ls++ --help' for more information.\n";
                std::exit(1);
        }
    }
}

void ArgumentParser::parseLongOption(const std::string& arg, LsOptions& options) {
    std::string option = arg.substr(2);
    std::string value;
    
    // Check for option=value format
    size_t eq_pos = option.find('=');
    if (eq_pos != std::string::npos) {
        value = option.substr(eq_pos + 1);
        option = option.substr(0, eq_pos);
    }
    
    if (option == "all") {
        options.show_all = true;
    } else if (option == "almost-all") {
        options.show_almost_all = true;
    } else if (option == "author") {
        options.show_author = true;
        options.format = ListFormat::LONG;
    } else if (option == "escape") {
        options.escape_names = true;
    } else if (option == "ignore-backups") {
        options.ignore_backups = true;
    } else if (option == "block-size") {
        if (!value.empty()) {
            options.block_size = value;
        }
    } else if (option == "classify" || option == "file-type") {
        options.show_file_type = true;
    } else if (option == "color") {
        handleColorOption(value, options);
    } else if (option == "directory") {
        options.show_directory_entries = true;
    } else if (option == "dereference") {
        options.dereference_links = true;
    } else if (option == "dereference-command-line") {
        options.follow_symlinks = true;
    } else if (option == "format") {
        handleFormatOption(value, options);
    } else if (option == "full-time") {
        options.full_time = true;
        options.format = ListFormat::LONG;
    } else if (option == "group-directories-first") {
        options.group_directories_first = true;
    } else if (option == "human-readable") {
        options.human_readable = true;
    } else if (option == "si") {
        options.si_units = true;
    } else if (option == "hide") {
        if (!value.empty()) {
            options.hide_patterns.push_back(value);
        }
    } else if (option == "ignore") {
        if (!value.empty()) {
            options.ignore_patterns.push_back(value);
        }
    } else if (option == "indicator-style") {
        if (value == "slash") {
            options.show_indicators = true;
        } else if (value == "classify") {
            options.show_file_type = true;
        }
    } else if (option == "inode") {
        options.show_inode = true;
    } else if (option == "kibibytes") {
        options.block_size_1024 = true;
    } else if (option == "literal") {
        options.literal_names = true;
    } else if (option == "long") {
        options.format = ListFormat::LONG;
    } else if (option == "no-dereference") {
        options.no_dereference = true;
    } else if (option == "no-group") {
        // Don't show group in long format
    } else if (option == "numeric-uid-gid") {
        options.numeric_uid_gid = true;
        options.format = ListFormat::LONG;
    } else if (option == "quote-name") {
        options.quote_names = true;
    } else if (option == "reverse") {
        options.reverse_order = true;
    } else if (option == "recursive") {
        options.recursive = true;
    } else if (option == "size") {
        options.show_size = true;
    } else if (option == "sort") {
        handleSortOption(value, options);
    } else if (option == "time") {
        if (value == "atime" || value == "access" || value == "use") {
            options.time_type = TimeType::ATIME;
        } else if (value == "ctime" || value == "status") {
            options.time_type = TimeType::CTIME;
        } else if (value == "birth" || value == "creation") {
            options.time_type = TimeType::BTIME;
        }
        options.sort_order = SortOrder::TIME;
    } else if (option == "time-style") {
        handleTimeStyleOption(value, options);
    } else if (option == "tabsize") {
        if (!value.empty()) {
            options.tab_size = std::stoi(value);
        }
    } else if (option == "width") {
        if (!value.empty()) {
            options.width = std::stoi(value);
        }
    } else if (option == "context") {
        options.show_context = true;
    } else {
        std::cerr << "ls++: unrecognized option '--" << option << "'\n";
        std::cerr << "Try 'ls++ --help' for more information.\n";
        std::exit(1);
    }
}

void ArgumentParser::handleColorOption(const std::string& value, LsOptions& options) {
    if (value.empty() || value == "auto") {
        options.use_color = isatty(STDOUT_FILENO);
    } else if (value == "always" || value == "yes") {
        options.use_color = true;
    } else if (value == "never" || value == "no") {
        options.use_color = false;
    }
}

void ArgumentParser::handleTimeStyleOption(const std::string& value, LsOptions& options) {
    if (!value.empty()) {
        options.time_style = value;
    }
}

void ArgumentParser::handleSortOption(const std::string& value, LsOptions& options) {
    if (value == "none") {
        options.sort_order = SortOrder::NONE;
    } else if (value == "name") {
        options.sort_order = SortOrder::NAME;
    } else if (value == "time") {
        options.sort_order = SortOrder::TIME;
    } else if (value == "size") {
        options.sort_order = SortOrder::SIZE;
    } else if (value == "version") {
        options.sort_order = SortOrder::VERSION;
    } else if (value == "extension") {
        options.sort_order = SortOrder::EXTENSION;
    }
}

void ArgumentParser::handleFormatOption(const std::string& value, LsOptions& options) {
    if (value == "across" || value == "horizontal") {
        options.format = ListFormat::ACROSS;
    } else if (value == "commas") {
        options.format = ListFormat::COMMAS;
    } else if (value == "long" || value == "verbose") {
        options.format = ListFormat::LONG;
    } else if (value == "single-column") {
        options.format = ListFormat::ONE_PER_LINE;
    } else if (value == "vertical") {
        options.format = ListFormat::VERTICAL;
    }
}

bool ArgumentParser::isOption(const std::string& arg) const {
    return arg.starts_with("-") && arg.length() > 1;
}

void ArgumentParser::printHelp() const {
    std::cout << "Usage: ls++ [OPTION]... [FILE]...\n";
    std::cout << "List information about the FILEs (the current directory by default).\n";
    std::cout << "Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.\n\n";
    
    std::cout << "Mandatory arguments to long options are mandatory for short options too.\n";
    std::cout << "  -a, --all                  do not ignore entries starting with .\n";
    std::cout << "  -A, --almost-all           do not list implied . and ..\n";
    std::cout << "      --author               with -l, print the author of each file\n";
    std::cout << "  -b, --escape               print C-style escapes for nongraphic characters\n";
    std::cout << "      --block-size=SIZE      scale sizes by SIZE before printing them\n";
    std::cout << "  -B, --ignore-backups       do not list implied entries ending with ~\n";
    std::cout << "  -c                         with -lt: sort by, and show, ctime\n";
    std::cout << "  -C                         list entries by columns\n";
    std::cout << "      --color[=WHEN]         colorize the output; WHEN can be 'always',\n";
    std::cout << "                               'auto', or 'never' (default)\n";
    std::cout << "  -d, --directory            list directories themselves, not their contents\n";
    std::cout << "  -f                         do not sort, enable -aU, disable -ls --color\n";
    std::cout << "  -F, --classify             append indicator (one of */=>@|) to entries\n";
    std::cout << "      --file-type            likewise, except do not append '*'\n";
    std::cout << "      --format=WORD          across -x, commas -m, horizontal -x,\n";
    std::cout << "                               long -l, single-column -1, verbose -l,\n";
    std::cout << "                               vertical -C\n";
    std::cout << "      --full-time            like -l --time-style=full-iso\n";
    std::cout << "  -g                         like -l, but do not list owner\n";
    std::cout << "      --group-directories-first\n";
    std::cout << "                             group directories before files\n";
    std::cout << "  -G, --no-group             in a long listing, don't print group names\n";
    std::cout << "  -h, --human-readable       with -l and/or -s, print human readable sizes\n";
    std::cout << "      --si                   likewise, but use powers of 1000 not 1024\n";
    std::cout << "  -H, --dereference-command-line\n";
    std::cout << "                             follow symbolic links listed on the command line\n";
    std::cout << "      --hide=PATTERN         do not list implied entries matching shell PATTERN\n";
    std::cout << "  -i, --inode                print the index number of each file\n";
    std::cout << "  -I, --ignore=PATTERN       do not list implied entries matching shell PATTERN\n";
    std::cout << "  -k, --kibibytes            default to 1024-byte blocks for disk usage\n";
    std::cout << "  -l, --long                 use a long listing format\n";
    std::cout << "  -L, --dereference          when showing file information for a symbolic\n";
    std::cout << "                               link, show information for the file the link\n";
    std::cout << "                               references rather than for the link itself\n";
    std::cout << "  -m                         fill width with a comma separated list of entries\n";
    std::cout << "  -n, --numeric-uid-gid      like -l, but list numeric user and group IDs\n";
    std::cout << "  -N, --literal              print raw entry names (don't treat e.g. control\n";
    std::cout << "                               characters specially)\n";
    std::cout << "  -o                         like -l, but do not list group information\n";
    std::cout << "  -p, --indicator-style=slash\n";
    std::cout << "                             append / indicator to directories\n";
    std::cout << "  -q, --hide-control-chars   print ? instead of nongraphic characters\n";
    std::cout << "  -Q, --quote-name           enclose entry names in double quotes\n";
    std::cout << "      --quoting-style=WORD   use quoting style WORD for entry names\n";
    std::cout << "  -r, --reverse              reverse order while sorting\n";
    std::cout << "  -R, --recursive            list subdirectories recursively\n";
    std::cout << "  -s, --size                 print the allocated size of each file, in blocks\n";
    std::cout << "  -S                         sort by file size, largest first\n";
    std::cout << "      --sort=WORD            sort by WORD instead of name: none (-U),\n";
    std::cout << "                               size (-S), time (-t), version (-v), extension (-X)\n";
    std::cout << "      --time=WORD            with -l, show time as WORD instead of default\n";
    std::cout << "                               modification time: atime or access or use (-u),\n";
    std::cout << "                               ctime or status (-c), birth, creation\n";
    std::cout << "      --time-style=STYLE     with -l, show times using style STYLE\n";
    std::cout << "  -t                         sort by modification time, newest first\n";
    std::cout << "  -T, --tabsize=COLS         assume tab stops at each COLS instead of 8\n";
    std::cout << "  -u                         with -lt: sort by, and show, access time\n";
    std::cout << "  -U                         do not sort; list entries in directory order\n";
    std::cout << "  -v                         natural sort of (version) numbers within text\n";
    std::cout << "  -w, --width=COLS           set output width to COLS. 0 means no limit\n";
    std::cout << "  -x                         list entries by lines instead of by columns\n";
    std::cout << "  -X                         sort alphabetically by entry extension\n";
    std::cout << "  -Z, --context              print any security context of each file\n";
    std::cout << "  -1                         list one file per line\n";
    std::cout << "      --help                 display this help and exit\n";
    std::cout << "      --version              output version information and exit\n\n";
    
    std::cout << "The SIZE argument is an integer and optional unit (example: 10K is 10*1024).\n";
    std::cout << "Units are K,M,G,T,P,E,Z,Y (powers of 1024) or KB,MB,... (powers of 1000).\n\n";
    
    std::cout << "Written by Sergey Kolosov, based on the original ls++ by Marcos Oliveira.\n";
    std::cout << "Enhanced with full ls compatibility and modular architecture.\n";
}

void ArgumentParser::printVersion() const {
    std::cout << LSPP_VERSION << std::endl;
    std::cout << "Enhanced ls replacement with icons and colors.\n";
    std::cout << "Built with modular C++23 architecture.\n";
}