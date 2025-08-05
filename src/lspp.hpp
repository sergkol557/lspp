#pragma once

#include <iostream>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cwchar>
#include <clocale>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <ctime>
#include <array>
#include "resources.hpp"

namespace fs = std::filesystem;

class Lspp {
    bool m_dots, m_list;
    bool m_one_per_line = false;
    bool m_sort_by_time = false;
    bool m_sort_by_size = false;
    bool m_reverse = false;
    bool m_add_file_type = false;

    fs::path dir;
    std::vector<fs::directory_entry> entries;
    std::vector<std::string> targets;
    //std::vector<std::string> names;
    std::vector<fs::path> full_paths;

    void show(), show_targets(), print(), help(),
            format(), list(); //output(const std::string&);
    void output(const std::string &name, const fs::path &full_path) const;

    static int get_terminal_width();

    static size_t display_width(const std::string &);

    static void print_permissions(mode_t);

    void list_detailed(const std::vector<fs::path> &);

    static bool is_executable(const std::filesystem::path &);

public:
    Lspp();

    void run(int, char **);

    void sort_by_greater_size_name();
};
