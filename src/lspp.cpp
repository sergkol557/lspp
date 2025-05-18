#include "lspp.hpp"

Lspp::Lspp(){
  m_dots = false;
  m_list = false;
  dir = fs::current_path();
  entries = {};
  targets = {};
  names   = {};
  full_paths = {};
}

void Lspp::run(int argc, char** argv){
  for(int i = 1; i < argc; ++i){
    const std::string param = argv[i];
    if(param == "-h" || param == "--help") {
      this->help();
      std::exit(0);
    }else if(param == "-v" || param == "--version"){
      std::cout << LSPP_VERSION << std::endl;
      std::exit(0);
    } else if(param == "-a" || param == "--all"){
      m_dots = true;
    }else if(param == "-l" || param == "--list"){
      m_list = true;
    }else{
      targets.push_back(param);
    }
  }

  if(targets.empty()){
    show();
  }else{
    show_targets();
  }
}

void Lspp::show(){
  try{
    for(const auto& entry : fs::directory_iterator(dir)){
      std::string name = entry.path().filename().string();
      if (!m_dots && name[0] == '.'){
        continue;
      }
      names.push_back(name);
      full_paths.push_back(entry.path());
    }

    if (m_dots) {
      names.insert(names.begin(), "..");
      full_paths.insert(full_paths.begin(), dir / "..");
      names.insert(names.begin(), ".");
      full_paths.insert(full_paths.begin(), dir / ".");
    }

    this->sort_by_greater_size_name();
    print();

  }catch(const fs::filesystem_error& e){
    std::cerr << "Erro: " << e.what() << '\n';
    std::exit(1);
  }
}

void Lspp::sort_by_greater_size_name(){
  std::locale loc("");
  std::vector<std::pair<std::string, fs::path>> combined;
  for (size_t i = 0; i < names.size(); ++i) {
    combined.emplace_back(names[i], full_paths[i]);
  }
  std::sort(combined.begin(), combined.end(), [&loc](const auto& a, const auto& b) {
      return std::use_facet<std::collate<char>>(loc).compare(
          a.first.data(), a.first.data() + a.first.size(),
          b.first.data(), b.first.data() + b.first.size()
          ) < 0;
      });
  for (size_t i = 0; i < combined.size(); ++i) {
    names[i] = combined[i].first;
    full_paths[i] = combined[i].second;
  }
}

void Lspp::show_targets(){
  std::vector<fs::path> files;
  std::vector<fs::path> dirs;

  for(const auto& arg : targets){
    fs::path p(arg);
    if (fs::is_regular_file(p)){
      files.push_back(p);
    }else if(fs::is_directory(p)){
      dirs.push_back(p);
    }else{
      std::cerr << "ls++: cannot access '" << arg << "': No such file or directory\n";
    }
  }

  if(!files.empty()){
    for(const auto& f : files){
      names.push_back(f.filename().string());
      full_paths.push_back(f);
    }
    this->sort_by_greater_size_name();
    print();
  }

  bool show_headers = (files.size() + dirs.size()) > 1;

  for(const auto& d : dirs){
    if(show_headers){
      std::cout << "\n" << d.string() << ":\n";
    }

    std::vector<std::string> temp_names;
    std::vector<fs::path> temp_paths;

    try {
      for (const auto& entry : fs::directory_iterator(d)) {
        std::string name = entry.path().filename().string();
        if (!m_dots && name[0] == '.'){
          continue;
        }
        temp_names.push_back(name);
        temp_paths.push_back(entry.path());
      }

      if (m_dots) {
        temp_names.insert(temp_names.begin(), "..");
        temp_paths.insert(temp_paths.begin(), d / "..");
        temp_names.insert(temp_names.begin(), ".");
        temp_paths.insert(temp_paths.begin(), d / ".");
      }

      std::swap(names, temp_names);
      std::swap(full_paths, temp_paths);

      this->sort_by_greater_size_name();
      print();

      std::swap(names, temp_names);
      std::swap(full_paths, temp_paths);

    } catch (const fs::filesystem_error& e) {
      std::cerr << "ls++: cannot open directory '" << d.string() << "': " << e.what() << "\n";
    }
  }
}

void Lspp::print(){
  if(m_list){
    this->list();
  }else{
    this->format();
  }
}

int Lspp::get_terminal_width(){
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col ? w.ws_col : 80;
}

size_t Lspp::display_width(const std::string& s){
  size_t width = 0;
  const char* ptr = s.c_str();
  size_t remaining = s.size();
  std::mbstate_t state = std::mbstate_t();

  while(remaining > 0) {
    wchar_t wc;
    size_t len = std::mbrtowc(&wc, ptr, remaining, &state);
    if( len == static_cast<size_t>(-1) || len == static_cast<size_t>(-2) ){
      width += remaining;
      break;
    }else if(len == 0){
      len = 1;
    }
    int w = wcwidth(wc);
    if(w >= 0){
      width += static_cast<size_t>(w);
    }else{
      width += 1;
    }
    ptr += len;
    remaining -= len;
  }
  return width;
}

void Lspp::format(){
  setlocale(LC_ALL, "");

  if(names.empty()) return;

  int term_width = get_terminal_width() - 15;
  //int term_width = get_terminal_width();
  size_t total = names.size();

  std::vector<size_t> lengths(total);
  std::vector<std::string> display_names(total);

  for(size_t i = 0; i < total; ++i) {
    const std::string& name = names[i];
    display_names[i] = name;
    lengths[i] = display_width(display_names[i]);
  }

  size_t rows = total;
  size_t cols = 1;
  std::vector<size_t> col_widths;

  for(size_t r = 1; r <= total; ++r){
    size_t c = (total + r - 1) / r;
    std::vector<size_t> widths(c, 0);

    for(size_t i = 0; i < total; ++i){
      size_t col = i / r;
      widths[col] = std::max(widths[col], lengths[i]);
    }

    size_t total_width = 0;
    for(size_t w : widths){
      total_width += w + 2;
    }

    if(total_width <= static_cast<size_t>(term_width)){
      rows = r;
      cols = c;
      col_widths = widths;
      break;
    }
  }

  for(size_t r = 0; r < rows; ++r){
    for(size_t c = 0; c < cols; ++c){
      size_t i = c * rows + r;
      if(i < total){
        const std::string& name = display_names[i];
        size_t width = lengths[i];
        size_t column_width = col_widths[c];

        //this->output(name);
        this->output(name, full_paths[i]);
        size_t padding = column_width + 2 - width;
        std::cout << std::string(padding, ' ');
      }
    }
    std::cout.put('\n');
  }
}

void Lspp::print_permissions(mode_t mode) {
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

void Lspp::list_detailed(const std::vector<fs::path>& paths) {
  for(const auto& path : paths){
    struct stat st;
    if(stat(path.c_str(), &st) == -1){
      perror(("stat: " + path.string()).c_str());
      continue;
    }

    this->print_permissions(st.st_mode);
    std::cout << std::setw(2) << st.st_nlink << ' ';

    struct passwd* pw = getpwuid(st.st_uid);
    struct group* gr = getgrgid(st.st_gid);
    std::cout << (pw ? pw->pw_name : "???") << ' '
      << (gr ? gr->gr_name : "???") << ' ';

    std::cout << std::setw(8) << st.st_size << ' ';

    std::array<char, 20> timebuf;
    strftime(timebuf.data(), sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
    std::cout << timebuf.data() << ' ';

    //output(path.filename().string());
    output(path.filename().string(), path);
    std::cout.put('\n');
  }
}

void Lspp::list(){
  this->list_detailed(full_paths);
}

void Lspp::output(const std::string& name, const fs::path& full_path){
  const std::string filetype = full_path.extension().string();
  auto [icon, color] = unicode(filetype);

  if(filetype.empty()){
    if(is_executable(full_path)){
      auto [bin_icon, bin_color] = filename("bin");
      std::cout << bin_color << bin_icon << off << " " << name;
    //}else if(fs::is_directory(name) && name != ".git") {
    }else if(fs::is_directory(full_path) && name != ".git") {
      auto [dir_icon, dir_color] = filename("dir");
      std::string display_name = name;
      /*if(display_name != "." && display_name != "..") {
        display_name += "/";
      }*/
      std::cout << dir_color << dir_icon << off << " " << display_name;
    }else{
      auto [make_icon, make_color] = filename(name);
      std::cout << make_color << make_icon << off << " " << name;
    }
  }else{
    if(is_executable(full_path)){
      auto [exe_icon, exe_color] = filename("bin");
      std::cout << exe_color << exe_icon << " " << name << off;
    }else{
      std::cout << color << icon << off << " " << name;
    }
  }
}

void Lspp::help(){
  std::cout << "Usage: ";
  std::cout << std::setw(4) << "" << "ls++ [options] [file(s)/folder(s)]\n\n";
  std::cout << std::setw(4) << "" << "--all, -a      Does not ignore entries starting with '.'\n";
  std::cout << std::setw(4) << "" << "--list, -l     Use a long listing format\n";
  std::cout << std::setw(4) << "" << "--help, -h     This help\n";
  std::cout << std::setw(4) << "" << "--version, -v  Check version\n\n";
  std::cout << "Written by Marcos Oliveira.\nWeb: https://github.com/terroo/lspp\n";
}

bool Lspp::is_executable(const std::filesystem::path& p) {
    /*using std::filesystem::perms;
    auto fstatus = std::filesystem::status(p);
    auto pms = fstatus.permissions();
    return ((pms & perms::owner_exec) != perms::none);*/
  return fs::is_regular_file(p) && access(p.c_str(), X_OK) == 0;
}

