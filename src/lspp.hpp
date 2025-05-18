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

  fs::path dir;
  std::vector<fs::directory_entry> entries;
  std::vector<std::string> targets;
  std::vector<std::string> names;
  std::vector<fs::path> full_paths;

  void show(), show_targets(), print(), help(), 
       format(), list(); //output(const std::string&);
  void output(const std::string& name, const fs::path& full_path);

  int get_terminal_width();
  size_t display_width(const std::string&);
  void print_permissions(mode_t);
  void list_detailed(const std::vector<fs::path>&);

  bool is_executable(const std::filesystem::path&);
   
public:
  Lspp();
  void run(int, char**);

  void sort_by_greater_size_name();
};

