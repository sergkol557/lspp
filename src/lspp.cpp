#include "lspp.hpp"

Lspp::Lspp() {
    m_dots = false;
    m_list = false;
    m_one_per_line = false;
    m_sort_by_time = false;
    m_sort_by_size = false;
    m_reverse = false;
    m_add_file_type = false;
    dir = fs::current_path();
    entries = {};
    targets = {};
    full_paths = {};
}

void Lspp::run(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        const std::string param = argv[i];
        if (param == "-h" || param == "--help") {
            this->help();
            std::exit(0);
        } else if (param == "-v" || param == "--version") {
            std::cout << LSPP_VERSION << std::endl;
            std::exit(0);
        } else if (param == "-a" || param == "--all") {
            m_dots = true;
        } else if (param == "-l" || param == "--list") {
            m_list = true;
        } else if (param == "-1" || param == "--one-per-line") {
            m_one_per_line = true;
        } else if (param == "-t" || param == "--time") {
            m_sort_by_time = true;
        } else if (param == "-S" || param == "--size") {
            m_sort_by_size = true;
        } else if (param == "-r" || param == "--reverse") {
            m_reverse = true;
        } else if (param == "-F" || param == "--list") {
            m_add_file_type = true;
        } else {
            targets.push_back(param);
        }
    }

    if (targets.empty()) {
        show();
    } else {
        show_targets();
    }
}

void Lspp::show() {
    try {
        for (const auto &entry: fs::directory_iterator(dir)) {
            std::string name = entry.path().filename().string();
            if (!m_dots && name[0] == '.') {
                continue;
            }
            full_paths.push_back(entry.path());
        }

        if (m_dots) {
            full_paths.insert(full_paths.begin(), dir / "..");
            full_paths.insert(full_paths.begin(), dir / ".");
        }

        this->sort_by_greater_size_name();
        print();
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Erro: " << e.what() << '\n';
        std::exit(1);
    }
}

void Lspp::sort_by_greater_size_name() {
    std::locale loc("");

    if (m_sort_by_time) {
        std::ranges::sort(full_paths, [this](const fs::path &a, const fs::path &b) {
            try {
                const auto time_a = fs::last_write_time(a);
                const auto time_b = fs::last_write_time(b);
                return m_reverse ? (time_a > time_b) : (time_a < time_b);
            } catch (...) {
                return false;
            }
        });
    } else if (m_sort_by_size) {
        std::ranges::sort(full_paths, [this](const fs::path &a, const fs::path &b) {
            try {
                auto size_a = fs::file_size(a);
                auto size_b = fs::file_size(b);
                return m_reverse ? (size_a > size_b) : (size_a < size_b);
            } catch (...) {
                return false;
            }
        });
    } else {
        std::ranges::sort(full_paths, [this, &loc](const fs::path &a, const fs::path &b) {
            const std::string an = a.filename().string();
            const std::string bn = b.filename().string();
            const int result = std::use_facet<std::collate<char> >(loc).compare(
                an.data(), an.data() + an.size(),
                bn.data(), bn.data() + bn.size()
            );
            return m_reverse ? (result > 0) : (result < 0);
        });
    }
}

void Lspp::show_targets() {
    std::vector<fs::path> files;
    std::vector<fs::path> dirs;

    for (const auto &arg: targets) {
        if (fs::path p(arg); fs::is_regular_file(p)) {
            files.push_back(p);
        } else if (fs::is_directory(p)) {
            dirs.push_back(p);
        } else {
            std::cerr << "ls++: cannot access '" << arg << "': No such file or directory\n";
        }
    }

    if (!files.empty()) {
        full_paths = files;
        this->sort_by_greater_size_name();
        print();
    }

    bool show_headers = (files.size() + dirs.size()) > 1;

    for (const auto &d: dirs) {
        if (show_headers) {
            std::cout << "\n" << d.string() << ":\n";
        }

        std::vector<fs::path> temp_paths;

        try {
            for (const auto &entry: fs::directory_iterator(d)) {
                std::string name = entry.path().filename().string();
                if (!m_dots && name[0] == '.') {
                    continue;
                }
                temp_paths.push_back(entry.path());
            }

            if (m_dots) {
                temp_paths.insert(temp_paths.begin(), d / "..");
                temp_paths.insert(temp_paths.begin(), d / ".");
            }

            std::swap(full_paths, temp_paths);

            this->sort_by_greater_size_name();
            print();

            std::swap(full_paths, temp_paths);
        } catch (const fs::filesystem_error &e) {
            std::cerr << "ls++: cannot open directory '" << d.string() << "': " << e.what() << "\n";
        }
    }
}

void Lspp::print() {
    if (m_list) {
        this->list();
    } else {
        this->format();
    }
}

int Lspp::get_terminal_width() {
    winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col ? w.ws_col : 80;
}

size_t Lspp::display_width(const std::string &s) {
    size_t width = 0;
    const char *ptr = s.c_str();
    size_t remaining = s.size();
    std::mbstate_t state = std::mbstate_t();

    while (remaining > 0) {
        wchar_t wc;
        size_t len = std::mbrtowc(&wc, ptr, remaining, &state);
        if (len == static_cast<size_t>(-1) || len == static_cast<size_t>(-2)) {
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

void Lspp::format() {
    setlocale(LC_ALL, "");

    if (full_paths.empty()) return;

    if (m_one_per_line) {
        for (const auto &path: full_paths) {
            output(path.filename().string(), path);
            std::cout << '\n';
        }
        return;
    }

    int term_width = get_terminal_width() - 15;
    size_t total = full_paths.size();

    std::vector<size_t> lengths(total);
    std::vector<std::string> display_names(total);

    for (size_t i = 0; i < total; ++i) {
        std::string name = full_paths[i].filename().string();
        display_names[i] = name;
        lengths[i] = display_width(name);
    }

    size_t rows = total, cols = 1;
    std::vector<size_t> col_widths;

    for (size_t r = 1; r <= total; ++r) {
        size_t c = (total + r - 1) / r;
        std::vector<size_t> widths(c, 0);
        for (size_t i = 0; i < total; ++i) {
            size_t col = i / r;
            widths[col] = std::max(widths[col], lengths[i]);
        }

        size_t total_width = 0;
        for (size_t w: widths) {
            total_width += w + 2;
        }

        if (total_width <= static_cast<size_t>(term_width)) {
            rows = r;
            cols = c;
            col_widths = widths;
            break;
        }
    }

    if (col_widths.empty()) {
        rows = total;
        cols = 1;
        size_t maxw = *std::max_element(lengths.begin(), lengths.end());
        col_widths.assign(1, maxw);
    }

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            size_t i = c * rows + r;
            if (i < total) {
                const std::string &name = display_names[i];
                size_t width = lengths[i];
                size_t column_width = col_widths[c];
                output(name, full_paths[i]);
                size_t padding = column_width + 2 - width;
                std::cout << std::string(padding, ' ');
            }
        }
        std::cout.put('\n');
    }
}

void Lspp::print_permissions(const mode_t mode) {
    std::array<char, 11> perms = {'-'};
    perms[0] = S_ISDIR(mode) ? 'd' : (S_ISLNK(mode) ? 'l' : '-');
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';
    std::cout << perms.data() << ' ';
}

void Lspp::list_detailed(const std::vector<fs::path> &paths) {
    for (const auto &path: paths) {
        struct stat st{};
        if (stat(path.c_str(), &st) == -1) {
            perror(("stat: " + path.string()).c_str());
            continue;
        }

        print_permissions(st.st_mode);
        std::cout << std::setw(2) << st.st_nlink << ' ';
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        std::cout << (pw ? pw->pw_name : "???") << ' '
                << (gr ? gr->gr_name : "???") << ' ';
        std::cout << std::setw(8) << st.st_size << ' ';
        std::array<char, 20> timebuf{};
        strftime(timebuf.data(), sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        std::cout << timebuf.data() << ' ';
        output(path.filename().string(), path);
        std::cout.put('\n');
    }
}

void Lspp::list() {
    list_detailed(full_paths);
}

void Lspp::output(const std::string &name, const fs::path &full_path) const {
    const std::string filetype = full_path.extension().string();
    auto [icon, color] = unicode(filetype);

    if (filetype.empty()) {
        if (is_executable(full_path)) {
            auto [bin_icon, bin_color] = filename("bin");
            std::cout << bin_color << bin_icon << off << " " << name;
        } else if (fs::is_directory(full_path) && name != ".git") {
            auto [dir_icon, dir_color] = filename("dir");
            std::cout << dir_color << dir_icon << off << " " << name;
        } else {
            auto [make_icon, make_color] = filename(name);
            std::cout << make_color << make_icon << off << " " << name;
        }
    } else {
        if (is_executable(full_path)) {
            auto [exe_icon, exe_color] = filename("bin");
            std::cout << exe_color << exe_icon << " " << name << off;
        } else {
            std::cout << color << icon << off << " " << name;
        }
    }

    if (m_add_file_type) {
        if (fs::is_directory(full_path)) {
            std::cout << "/";
        } else if (is_executable(full_path)) {
            std::cout << "*";
        } else if (fs::is_symlink(full_path)) {
            std::cout << "@";
        }
    }
}

void Lspp::help() {
    std::cout << "Usage: ";
    std::cout << "ls++ [options] [file(s)/folder(s)]\n\n";
    std::cout << std::setw(4) << "--all, -a      Does not ignore entries starting with '.'\n";
    std::cout << std::setw(4) << "--list, -l     Use a long listing format\n";
    std::cout << std::setw(4) << "--one-per-line, -1 One file per line\n";
    std::cout << std::setw(4) << "--time, -t     Sort by modification time\n";
    std::cout << std::setw(4) << "--size, -S     Sort by file size\n";
    std::cout << std::setw(4) << "--reverse, -r  Reverse order\n";
    std::cout << std::setw(4) << "--file-type, -F Add file type indicators\n";
    std::cout << std::setw(4) << "--help, -h     This help\n";
    std::cout << std::setw(4) << "--version, -v  Check version\n\n";
    std::cout << "Written by Marcos Oliveira.\nWeb: https://github.com/terroo/lspp\n";
}

bool Lspp::is_executable(const std::filesystem::path &p) {
    return fs::is_regular_file(p) && access(p.c_str(), X_OK) == 0;
}
