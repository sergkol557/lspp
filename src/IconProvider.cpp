#include "IconProvider.hpp"
#include "FileOperations.hpp"
#include <algorithm>
#include <unistd.h>
#include <unordered_set>

// Static color constants
const std::string IconProvider::RESET_COLOR = "\033[m";
const std::string IconProvider::BOLD = "\033[1m";
const std::string IconProvider::DIM = "\033[2m";
const std::string IconProvider::UNDERLINE = "\033[4m";
const std::string IconProvider::BLINK = "\033[5m";
const std::string IconProvider::REVERSE = "\033[7m";

IconProvider::IconProvider() : m_color_enabled(true) {
    initializeExtensionMap();
    initializeFilenameMap();
    initializeFiletypeMap();
}

void IconProvider::initializeExtensionMap() {
    m_extension_map = {
        // Programming languages
        {".asm", {"\uf471", "\033[38;2;250;109;63m"}},
        {".c", {"\ufb70", "\033[38;2;146;140;140m"}},
        {".cpp", {"\ufb71", "\033[38;2;241;81;126m"}},
        {".cxx", {"\ufb71", "\033[38;2;241;81;126m"}},
        {".cc", {"\ufb71", "\033[38;2;241;81;126m"}},
        {".cs", {"\uf81a", "\033[38;2;64;135;24m"}},
        {".css", {"\uf81b", "\033[38;2;86;156;214m"}},
        {".go", {"\ufcd1", "\033[38;2;42;174;216m"}},
        {".h", {"\u2261", "\033[38;2;146;140;140m"}},
        {".hpp", {"\u2261", "\033[38;2;173;151;185m"}},
        {".hxx", {"\u2261", "\033[38;2;173;151;185m"}},
        {".html", {"\uf13b", "\033[38;2;228;79;57m"}},
        {".htm", {"\uf13b", "\033[38;2;228;79;57m"}},
        {".java", {"\uf675", "\033[38;2;177;115;31m"}},
        {".js", {"\ue74e", "\033[38;2;241;224;89m"}},
        {".jsx", {"\ue74e", "\033[38;2;241;224;89m"}},
        {".php", {"\ue73d", "\033[38;2;77;92;150m"}},
        {".json", {"\ue60b", "\033[38;2;251;193;60m"}},
        {".lua", {"\ue620", "\033[38;2;8;49;129m"}},
        {".md", {"\uf853", "\033[38;2;8;219;239m"}},
        {".markdown", {"\uf853", "\033[38;2;8;219;239m"}},
        {".ps1", {"\ufcb5", "\033[38;2;5;169;244m"}},
        {".py", {"\uf81f", "\033[38;2;49;114;166m"}},
        {".pyw", {"\uf81f", "\033[38;2;49;114;166m"}},
        {".rs", {"\ue7a8", "\033[38;2;222;166;133m"}},
        {".rb", {"\ue739", "\033[38;2;176;43;28m"}},
        {".s", {"\uf471", "\033[38;2;110;75;16m"}},
        {".sh", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".bash", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".zsh", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".fish", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".swift", {"\ufbe3", "\033[38;2;240;79;53m"}},
        {".ts", {"\ue628", "\033[38;2;103;161;224m"}},
        {".tsx", {"\ue628", "\033[38;2;103;161;224m"}},
        {".xml", {"\uf72d", "\033[38;2;64;153;69m"}},
        {".yaml", {"\ue60b", "\033[38;2;244;68;62m"}},
        {".yml", {"\ue60b", "\033[38;2;244;68;62m"}},
        {".cmake", {"\uf425", "\033[38;2;178;178;179m"}},
        {".d", {"\ue7af", "\033[38;2;187;88;93m"}},
        {".dart", {"\ue798", "\033[38;2;87;182;240m"}},
        {".exs", {"\ue62d", "\033[38;2;110;72;37m"}},
        {".ex", {"\ue62d", "\033[38;2;110;72;37m"}},
        {".clj", {"\ue76a", "\033[38;2;220;86;83m"}},
        {".cljs", {"\ue76a", "\033[38;2;220;86;83m"}},
        {".gd", {"\ufba7", "\033[38;2;79;195;247m"}},
        {".pl", {"\ue769", "\033[38;2;31;153;196m"}},
        {".pm", {"\ue769", "\033[38;2;31;153;196m"}},
        {".vala", {"\uf7ab", "\033[38;2;149;117;205m"}},
        {".tex", {"\uf783", "\033[38;2;66;165;245m"}},
        {".vue", {"\ufd42", "\033[38;2;65;184;131m"}},
        {".vim", {"\ue62b", "\033[38;2;67;160;71m"}},
        {".r", {"\ufcd2", "\033[38;2;25;118;210m"}},
        {".sql", {"\ue704", "\033[38;2;1;94;134m"}},
        {".sqlite", {"\ue7c4", "\033[38;2;1;57;84m"}},
        {".nix", {"\uf313", "\033[38;2;80;117;193m"}},
        {".zig", {"\u2b4d", "\033[38;2;236;146;91m"}},
        {".hs", {"\ue61f", "\033[38;2;94;80;134m"}},
        {".kt", {"\ue70e", "\033[38;2;169;123;255m"}},
        {".kts", {"\ue70e", "\033[38;2;169;123;255m"}},
        {".lisp", {"\ue791", "\033[38;2;63;182;139m"}},
        {".lsp", {"\ue791", "\033[38;2;63;182;139m"}},
        {".el", {"\u03BB", "\033[38;2;63;182;139m"}},
        {".jl", {"\ue624", "\033[38;2;162;112;186m"}},
        {".nim", {"\uf6a4", "\033[38;2;255;233;83m"}},
        {".v", {"\u0056", "\033[38;2;93;135;189m"}},
        {".fs", {"\ue7a7", "\033[38;2;55;139;186m"}},
        {".scala", {"\uf313", "\033[38;2;194;45;64m"}},
        {".ml", {"\uf1ce", "\033[38;2;238;106;26m"}},
        {".erl", {"\ue7b1", "\033[38;2;184;57;152m"}},
        {".elm", {"\ue62c", "\033[38;2;96;181;204m"}},
        {".coffee", {"\uf0f4", "\033[38;2;113;156;179m"}},
        {".ejs", {"\ue618", "\033[38;2;241;224;89m"}},
        {".pug", {"\ue60c", "\033[38;2;193;109;58m"}},
        {".sass", {"\ue603", "\033[38;2;203;89;150m"}},
        {".scss", {"\ue603", "\033[38;2;203;89;150m"}},
        {".less", {"\ue60b", "\033[38;2;25;118;210m"}},
        {".styl", {"\ue600", "\033[38;2;203;89;150m"}},
        {".mustache", {"\ue60f", "\033[38;2;241;224;89m"}},
        {".hbs", {"\ue60f", "\033[38;2;241;224;89m"}},
        {".bat", {"\uf120", "\033[38;2;76;175;80m"}},
        {".psd", {"\ue7b8", "\033[38;2;74;144;226m"}},
        {".ai", {"\ue7b4", "\033[38;2;241;100;84m"}},
        {".sketch", {"\ue79f", "\033[38;2;241;100;84m"}},
        {".blend", {"\uf72a", "\033[38;2;240;141;54m"}},
        {".jsx", {"\ue7ba", "\033[38;2;97;175;239m"}},
        {".tsx", {"\ue7ba", "\033[38;2;97;175;239m"}},
        {".pp", {"\ue631", "\033[38;2;251;193;60m"}},
        {".dockerfile", {"\uf308", "\033[38;2;72;126;176m"}},
        
        // Archives
        {".zip", {"\uf410", "\033[38;2;175;180;43m"}},
        {".rar", {"\uf410", "\033[38;2;175;180;43m"}},
        {".7z", {"\uf410", "\033[38;2;175;180;43m"}},
        {".tar", {"\uf410", "\033[38;2;175;180;43m"}},
        {".gz", {"\uf410", "\033[38;2;175;180;43m"}},
        {".bz2", {"\uf410", "\033[38;2;175;180;43m"}},
        {".xz", {"\uf410", "\033[38;2;175;180;43m"}},
        {".deb", {"\uf306", "\033[38;2;240;85;142m"}},
        {".rpm", {"\uf30a", "\033[38;2;52;103;172m"}},
        {".dmg", {"\uf410", "\033[38;2;175;180;43m"}},
        {".pkg", {"\uf410", "\033[38;2;175;180;43m"}},
        {".msi", {"\uf410", "\033[38;2;175;180;43m"}},
        {".tgz", {"\uf410", "\033[38;2;175;180;43m"}},
        {".txz", {"\uf410", "\033[38;2;175;180;43m"}},
        {".tbz", {"\uf410", "\033[38;2;175;180;43m"}},
        {".tlz", {"\uf410", "\033[38;2;175;180;43m"}},
        
        // Media files
        {".mp3", {"\ufb75", "\033[38;2;239;83;80m"}},
        {".mp4", {"\ue271", "\033[38;2;253;154;62m"}},
        {".avi", {"\ue271", "\033[38;2;253;154;62m"}},
        {".mov", {"\uf72a", "\033[38;2;240;141;54m"}},
        {".mpeg", {"\uf72a", "\033[38;2;236;138;55m"}},
        {".mpg", {"\uf72a", "\033[38;2;236;138;55m"}},
        {".flv", {"\uf72a", "\033[38;2;242;148;59m"}},
        {".mkv", {"\ue271", "\033[38;2;253;154;62m"}},
        {".webm", {"\ue271", "\033[38;2;253;154;62m"}},
        {".ogg", {"\ufb75", "\033[38;2;199;70;67m"}},
        {".ogv", {"\uf72a", "\033[38;2;205;130;57m"}},
        {".flac", {"\ufb75", "\033[38;2;200;80;70m"}},
        {".wav", {"\ufb75", "\033[38;2;200;80;70m"}},
        {".aac", {"\ufb75", "\033[38;2;200;80;70m"}},
        {".m4a", {"\ufb75", "\033[38;2;200;80;70m"}},
        {".wma", {"\ufb75", "\033[38;2;200;80;70m"}},
        {".opus", {"\ufb75", "\033[38;2;200;80;70m"}},
        {".m3u", {"\uf910", "\033[38;2;239;83;80m"}},
        {".m3u8", {"\uf910", "\033[38;2;239;83;80m"}},
        {".pls", {"\uf910", "\033[38;2;239;83;80m"}},
        
        // Images
        {".jpg", {"\uf71e", "\033[38;2;45;165;154m"}},
        {".jpeg", {"\uf71e", "\033[38;2;45;165;154m"}},
        {".png", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".gif", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".bmp", {"\uf71e", "\033[38;2;35;155;145m"}},
        {".tiff", {"\uf71e", "\033[38;2;35;155;145m"}},
        {".tif", {"\uf71e", "\033[38;2;35;155;145m"}},
        {".svg", {"\ufc1f", "\033[38;2;48;166;154m"}},
        {".ico", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".webp", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".xcf", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".eps", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".raw", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".cr2", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".nef", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".arw", {"\uf71e", "\033[38;2;40;160;150m"}},
        {".dng", {"\uf71e", "\033[38;2;40;160;150m"}},
        
        // Documents
        {".pdf", {"\uf724", "\033[38;2;240;100;100m"}},
        {".doc", {"\uf724", "\033[38;2;100;150;240m"}},
        {".docx", {"\uf724", "\033[38;2;100;150;240m"}},
        {".xls", {"\uf724", "\033[38;2;100;200;100m"}},
        {".xlsx", {"\uf724", "\033[38;2;100;200;100m"}},
        {".ppt", {"\uf724", "\033[38;2;240;150;100m"}},
        {".pptx", {"\uf724", "\033[38;2;240;150;100m"}},
        {".odt", {"\uf724", "\033[38;2;100;150;240m"}},
        {".ods", {"\uf724", "\033[38;2;100;200;100m"}},
        {".odp", {"\uf724", "\033[38;2;240;150;100m"}},
        {".rtf", {"\uf724", "\033[38;2;100;150;240m"}},
        {".txt", {"\uf723", "\033[38;2;200;200;200m"}},
        {".epub", {"\uf724", "\033[38;2;240;100;100m"}},
        {".mobi", {"\uf724", "\033[38;2;240;100;100m"}},
        {".azw", {"\uf724", "\033[38;2;240;100;100m"}},
        {".azw3", {"\uf724", "\033[38;2;240;100;100m"}},
        {".djvu", {"\uf724", "\033[38;2;240;100;100m"}},
        
        // Config files
        {".ini", {"\uf013", "\033[38;2;3;136;209m"}},
        {".conf", {"\uf013", "\033[38;2;3;136;209m"}},
        {".config", {"\uf013", "\033[38;2;3;136;209m"}},
        {".toml", {"\uf013", "\033[38;2;3;136;209m"}},
        {".properties", {"\uf013", "\033[38;2;3;136;209m"}},
        {".cfg", {"\uf013", "\033[38;2;3;136;209m"}},
        {".env", {"\uf013", "\033[38;2;3;136;209m"}},
        {".settings", {"\uf013", "\033[38;2;3;136;209m"}},
        
        // Database files
        {".sql", {"\ue704", "\033[38;2;1;94;134m"}},
        {".db", {"\ue7c4", "\033[38;2;1;57;84m"}},
        {".sqlite", {"\ue7c4", "\033[38;2;1;57;84m"}},
        {".sqlite3", {"\ue7c4", "\033[38;2;1;57;84m"}},
        {".mdb", {"\ue7c4", "\033[38;2;1;57;84m"}},
        {".accdb", {"\ue7c4", "\033[38;2;1;57;84m"}},
        
        // Font files
        {".ttf", {"\uf031", "\033[38;2;100;150;200m"}},
        {".otf", {"\uf031", "\033[38;2;100;150;200m"}},
        {".woff", {"\uf031", "\033[38;2;100;150;200m"}},
        {".woff2", {"\uf031", "\033[38;2;100;150;200m"}},
        {".eot", {"\uf031", "\033[38;2;100;150;200m"}},
        {".fon", {"\uf031", "\033[38;2;100;150;200m"}},
        
        // Certificate files
        {".crt", {"\uf023", "\033[38;2;100;200;100m"}},
        {".pem", {"\uf023", "\033[38;2;100;200;100m"}},
        {".key", {"\uf023", "\033[38;2;100;200;100m"}},
        {".csr", {"\uf023", "\033[38;2;100;200;100m"}},
        {".der", {"\uf023", "\033[38;2;100;200;100m"}},
        
        // Misc
        {".iso", {"\ue271", "\033[38;2;253;154;62m"}},
        {".exe", {"\uf2d0", "\033[38;2;76;175;80m"}},
        {".msi", {"\uf2d0", "\033[38;2;76;175;80m"}},
        {".app", {"\uf2d0", "\033[38;2;76;175;80m"}},
        {".deb", {"\uf306", "\033[38;2;240;85;142m"}},
        {".ebuild", {"\uf30d", "\033[38;2;148;141;211m"}},
        {".log", {"\uf723", "\033[38;2;150;150;150m"}},
        {".tmp", {"\uf723", "\033[38;2;100;100;100m"}},
        {".temp", {"\uf723", "\033[38;2;100;100;100m"}},
        {".cache", {"\uf723", "\033[38;2;100;100;100m"}},
        {".bak", {"\uf723", "\033[38;2;150;150;150m"}},
        {".swp", {"\uf723", "\033[38;2;150;150;150m"}},
        {".lock", {"\uf023", "\033[38;2;150;150;150m"}},
    };
}

void IconProvider::initializeFilenameMap() {
    m_filename_map = {
        {"Makefile", {"\uf728", "\033[38;2;239;83;80m"}},
        {"makefile", {"\uf728", "\033[38;2;239;83;80m"}},
        {"GNUmakefile", {"\uf728", "\033[38;2;239;83;80m"}},
        {"CMakeLists.txt", {"\uf425", "\033[38;2;178;178;179m"}},
        {"Dockerfile", {"\uf308", "\033[38;2;72;126;176m"}},
        {"docker-compose.yml", {"\uf308", "\033[38;2;72;126;176m"}},
        {"docker-compose.yaml", {"\uf308", "\033[38;2;72;126;176m"}},
        {".gitignore", {"\ue702", "\033[38;2;250;111;66m"}},
        {".gitmodules", {"\ue702", "\033[38;2;250;111;66m"}},
        {".gitattributes", {"\ue702", "\033[38;2;250;111;66m"}},
        {"README.md", {"\uf853", "\033[38;2;8;219;239m"}},
        {"README.txt", {"\uf853", "\033[38;2;8;219;239m"}},
        {"README", {"\uf853", "\033[38;2;8;219;239m"}},
        {"LICENSE", {"\uf718", "\033[38;2;255;215;0m"}},
        {"COPYING", {"\uf718", "\033[38;2;255;215;0m"}},
        {"COPYRIGHT", {"\uf718", "\033[38;2;255;215;0m"}},
        {"package.json", {"\ue60b", "\033[38;2;251;193;60m"}},
        {"package-lock.json", {"\ue60b", "\033[38;2;251;193;60m"}},
        {"yarn.lock", {"\ue60b", "\033[38;2;251;193;60m"}},
        {"requirements.txt", {"\uf81f", "\033[38;2;49;114;166m"}},
        {"Pipfile", {"\uf81f", "\033[38;2;49;114;166m"}},
        {"Cargo.toml", {"\ue7a8", "\033[38;2;222;166;133m"}},
        {"Cargo.lock", {"\ue7a8", "\033[38;2;222;166;133m"}},
        {"go.mod", {"\ufcd1", "\033[38;2;42;174;216m"}},
        {"go.sum", {"\ufcd1", "\033[38;2;42;174;216m"}},
        {".env", {"\uf013", "\033[38;2;3;136;209m"}},
        {".env.example", {"\uf013", "\033[38;2;3;136;209m"}},
        {".bashrc", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".zshrc", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".profile", {"\uf68c", "\033[38;2;137;224;79m"}},
        {".vimrc", {"\ue62b", "\033[38;2;67;160;71m"}},
        {".npmrc", {"\ue71e", "\033[38;2;215;77;84m"}},
        {".eslintrc.js", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".eslintrc.json", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".eslintrc.yaml", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".eslintrc.yml", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".stylelintrc", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".stylelintrc.json", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".stylelintrc.yaml", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".prettierrc", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".prettierignore", {"\ufb75", "\033[38;2;156;109;208m"}},
        {".babelrc", {"\ue635", "\033[38;2;253;216;53m"}},
        {".travis.yml", {"\uf298", "\033[38;2;52;103;172m"}},
        {".gitlab-ci.yml", {"\uf296", "\033[38;2;226;68;55m"}},
        {".jenkinsfile", {"\ue767", "\033[38;2;244;100;63m"}},
        {".vscode", {"\ue70c", "\033[38;2;3;136;209m"}},
        {".github", {"\uf408", "\033[38;2;255;255;255m"}},
        {".circleci", {"\uf298", "\033[38;2;52;103;172m"}},
        {".dockerignore", {"\uf308", "\033[38;2;72;126;176m"}},
        {"Gemfile", {"\ue739", "\033[38;2;176;43;28m"}},
        {"Gemfile.lock", {"\ue739", "\033[38;2;176;43;28m"}},
        {"Vagrantfile", {"\uf21a", "\033[38;2;3;136;209m"}},
        {"Rakefile", {"\ue791", "\033[38;2;63;182;139m"}},
        {"Procfile", {"\uf013", "\033[38;2;3;136;209m"}},
        {"webpack.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"webpack.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"yarn.json", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"yarn-error.log", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"yarnrc.yml", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"tsconfig.json", {"\ue628", "\033[38;2;103;161;224m"}},
        {"jsconfig.json", {"\ue74e", "\033[38;2;241;224;89m"}},
        {"composer.json", {"\uf8d5", "\033[38;2;153;204;255m"}},
        {"composer.lock", {"\uf8d5", "\033[38;2;153;204;255m"}},
        {"nginx.conf", {"\uf0ad", "\033[38;2;76;175;80m"}},
        {"httpd.conf", {"\uf0ad", "\033[38;2;76;175;80m"}},
        {"fstab", {"\uf0ad", "\033[38;2;76;175;80m"}},
        {"passwd", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"shadow", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"hosts", {"\uf0ac", "\033[38;2;76;175;80m"}},
        {"crontab", {"\uf017", "\033[38;2;100;150;200m"}},
        {"authorized_keys", {"\uf084", "\033[38;2;255;215;0m"}},
        {"known_hosts", {"\uf084", "\033[38;2;255;215;0m"}},
        {".ssh", {"\uf084", "\033[38;2;255;215;0m"}},
        {"TODO", {"\uf046", "\033[38;2;76;175;80m"}},
        {"TODO.md", {"\uf046", "\033[38;2;76;175;80m"}},
        {"TODO.txt", {"\uf046", "\033[38;2;76;175;80m"}},
        {"CHANGELOG", {"\uf723", "\033[38;2;200;200;200m"}},
        {"CHANGELOG.md", {"\uf723", "\033[38;2;200;200;200m"}},
        {"CHANGELOG.txt", {"\uf723", "\033[38;2;200;200;200m"}},
        {"CONTRIBUTING", {"\uf723", "\033[38;2;200;200;200m"}},
        {"CONTRIBUTING.md", {"\uf723", "\033[38;2;200;200;200m"}},
        {"CONTRIBUTING.txt", {"\uf723", "\033[38;2;200;200;200m"}},
        {"MAINTAINERS", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"MAINTAINERS.md", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"MAINTAINERS.txt", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"CODEOWNERS", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"OWNERS", {"\uf0c0", "\033[38;2;150;150;150m"}},
        {"THIRD_PARTY_LICENSES", {"\uf718", "\033[38;2;255;215;0m"}},
        {"THIRD_PARTY_LICENSES.txt", {"\uf718", "\033[38;2;255;215;0m"}},
        {"THIRD_PARTY_LICENSES.md", {"\uf718", "\033[38;2;255;215;0m"}},
        {"THIRD-PARTY-LICENSES", {"\uf718", "\033[38;2;255;215;0m"}},
        {"THIRD-PARTY-LICENSES.txt", {"\uf718", "\033[38;2;255;215;0m"}},
        {"THIRD-PARTY-LICENSES.md", {"\uf718", "\033[38;2;255;215;0m"}},
        {"karma.conf.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"karma.conf.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"karma.conf.coffee", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"rollup.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"rollup.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"rollup.config.coffee", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gruntfile.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gruntfile.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gruntfile.coffee", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gulpfile.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gulpfile.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gulpfile.coffee", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"mix.exs", {"\ue62d", "\033[38;2;110;72;37m"}},
        {"mix.lock", {"\ue62d", "\033[38;2;110;72;37m"}},
        {"bsconfig.json", {"\ue733", "\033[38;2;244;100;63m"}},
        {"bsb.lock", {"\ue733", "\033[38;2;244;100;63m"}},
        {"next.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"next.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"nuxt.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"nuxt.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gatsby-config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gatsby-config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gatsby-node.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"gatsby-node.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"firebase.json", {"\uf023", "\033[38;2;250;111;66m"}},
        {"firestore.rules", {"\uf023", "\033[38;2;250;111;66m"}},
        {"firestore.indexes.json", {"\uf023", "\033[38;2;250;111;66m"}},
        {"database.rules.json", {"\uf023", "\033[38;2;250;111;66m"}},
        {"storage.rules", {"\uf023", "\033[38;2;250;111;66m"}},
        {"functions/package.json", {"\ue60b", "\033[38;2;251;193;60m"}},
        {"functions/index.js", {"\ue74e", "\033[38;2;241;224;89m"}},
        {"functions/index.ts", {"\ue628", "\033[38;2;103;161;224m"}},
        {"netlify.toml", {"\uf313", "\033[38;2;80;117;193m"}},
        {"now.json", {"\uf313", "\033[38;2;80;117;193m"}},
        {"vercel.json", {"\uf313", "\033[38;2;80;117;193m"}},
        {"wrangler.toml", {"\uf313", "\033[38;2;80;117;193m"}},
        {"apollo.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"apollo.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"vite.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"vite.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"vitest.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"vitest.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"jest.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"jest.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"jest.config.cjs", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"jest.config.mjs", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"babel.config.js", {"\ue635", "\033[38;2;253;216;53m"}},
        {"babel.config.json", {"\ue635", "\033[38;2;253;216;53m"}},
        {"babel.config.cjs", {"\ue635", "\033[38;2;253;216;53m"}},
        {"babel.config.mjs", {"\ue635", "\033[38;2;253;216;53m"}},
        {"postcss.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"postcss.config.cjs", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"postcss.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"tailwind.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"tailwind.config.cjs", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"tailwind.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"snowpack.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"snowpack.config.cjs", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"snowpack.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"snowpack.config.json", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"windi.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"windi.config.cjs", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"windi.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"windi.config.json", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"stylelint.config.js", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"stylelint.config.cjs", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"stylelint.config.ts", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"prettier.config.js", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"prettier.config.cjs", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"prettier.config.ts", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"eslint.config.js", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"eslint.config.cjs", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"eslint.config.ts", {"\ufb75", "\033[38;2;156;109;208m"}},
        {"rome.json", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"biome.json", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"bun.lockb", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"bunfig.toml", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"deno.json", {"\ufcd2", "\033[38;2;25;118;210m"}},
        {"deno.jsonc", {"\ufcd2", "\033[38;2;25;118;210m"}},
        {"deno.lock", {"\ufcd2", "\033[38;2;25;118;210m"}},
        {"pnpm-lock.yaml", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"pnpm-workspace.yaml", {"\ue6a7", "\033[38;2;215;77;84m"}},
        {"turbo.json", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"lefthook.yml", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"lefthook-local.yml", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"knip.json", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"knip.jsonc", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"knip.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"knip.config.ts", {"\ufc29", "\033[38;2;103;161;224m"}},
        {"knip.config.js", {"\ufc29", "\033[38;2;103;161;224m"}},
        
        // Special folders
        {"Desktop", {"\ufcbe", "\033[38;2;85;170;255m"}},
        {"desktop", {"\ufcbe", "\033[38;2;85;170;255m"}},
        {"Documents", {"\uf718", "\033[38;2;255;215;0m"}},
        {"documents", {"\uf718", "\033[38;2;255;215;0m"}},
        {"Downloads", {"\uf6d0", "\033[38;2;100;200;255m"}},
        {"downloads", {"\uf6d0", "\033[38;2;100;200;255m"}},
        {"Music", {"\uf832", "\033[38;2;255;100;150m"}},
        {"music", {"\uf832", "\033[38;2;255;100;150m"}},
        {"Audio", {"\uf832", "\033[38;2;255;100;150m"}},
        {"audio", {"\uf832", "\033[38;2;255;100;150m"}},
        {"Pictures", {"\uf753", "\033[38;2;200;100;255m"}},
        {"pictures", {"\uf753", "\033[38;2;200;100;255m"}},
        {"Photos", {"\uf753", "\033[38;2;200;100;255m"}},
        {"photos", {"\uf753", "\033[38;2;200;100;255m"}},
        {"Videos", {"\uf72a", "\033[38;2;255;150;100m"}},
        {"videos", {"\uf72a", "\033[38;2;255;150;100m"}},
        {"Movies", {"\uf72a", "\033[38;2;255;150;100m"}},
        {"movies", {"\uf72a", "\033[38;2;255;150;100m"}},
        {"Public", {"\uf0ac", "\033[38;2;100;200;150m"}},
        {"public", {"\uf0ac", "\033[38;2;100;200;150m"}},
        {"Templates", {"\ufac6", "\033[38;2;150;200;100m"}},
        {"templates", {"\ufac6", "\033[38;2;150;200;100m"}},
    };
}

void IconProvider::initializeFiletypeMap() {
    m_filetype_map = {
        {"directory", {"\uf74a", "\033[38;2;224;177;77m"}},
        {"executable", {"\uf713", "\033[38;2;76;175;80m"}},
        {"symlink", {"\uf838", "\033[38;2;76;175;255m"}},
        {"socket", {"\uf9e6", "\033[38;2;255;100;255m"}},
        {"pipe", {"\uf731", "\033[38;2;255;255;100m"}},
        {"block_device", {"\uf0a0", "\033[38;2;255;150;100m"}},
        {"char_device", {"\uf0a0", "\033[38;2;255;200;100m"}},
        {"unknown", {"\uf723", "\033[38;2;150;150;150m"}},
        {"git", {"\ue702", "\033[38;2;250;111;66m"}},
        {"hidden", {"\uf023", "\033[38;2;150;150;150m"}},
        {"backup", {"\uf723", "\033[38;2;150;150;150m"}},
        {"temp", {"\uf723", "\033[38;2;100;100;100m"}},
        {"log", {"\uf723", "\033[38;2;150;150;150m"}},
        {"config", {"\uf013", "\033[38;2;3;136;209m"}},
        {"lock", {"\uf023", "\033[38;2;150;150;150m"}},
        {"cache", {"\uf723", "\033[38;2;100;100;100m"}},
    };
}

std::string IconProvider::getIcon(const FileInfo& file) const {
    auto [icon, color] = getIconAndColor(file);
    return icon;
}

std::string IconProvider::getColorCode(const FileInfo& file) const {
    auto [icon, color] = getIconAndColor(file);


    return m_color_enabled ? color : "";
}

std::pair<std::string, std::string> IconProvider::getIconAndColor(const FileInfo& file) const {
    // Check for specific file types first
    if (file.is_directory) {
        // Check for special directory names
        const std::string& dirname = file.display_name.string();
        
        // Convert dirname to lowercase for case-insensitive comparison
        std::string lower_dirname = dirname;
        std::transform(lower_dirname.begin(), lower_dirname.end(), lower_dirname.begin(), ::tolower);
        
        // Check for special system folders
        if (dirname == "Desktop" || dirname == "desktop" || lower_dirname == "desktop") {
            auto it = m_filename_map.find("Desktop");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == "Documents" || dirname == "documents" || lower_dirname == "documents") {
            auto it = m_filename_map.find("Documents");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == "Downloads" || dirname == "downloads" || lower_dirname == "downloads") {
            auto it = m_filename_map.find("Downloads");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == "Music" || dirname == "music" || lower_dirname == "music" || 
                   lower_dirname.find("music") != std::string::npos || lower_dirname.find("audio") != std::string::npos) {
            // Use musical note for music folders - check if we have a better icon
            auto it = m_filename_map.find("Music");
            if (it != m_filename_map.end()) {
                return it->second;
            }
            // Fallback to standard musical note
            return {"\ufb2c", "\033[38;2;255;100;150m"};
        } else if (dirname == "Pictures" || dirname == "pictures" || lower_dirname == "pictures" ||
                   lower_dirname.find("picture") != std::string::npos || lower_dirname.find("photo") != std::string::npos) {
            auto it = m_filename_map.find("Pictures");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == "Videos" || dirname == "videos" || lower_dirname == "videos" ||
                   lower_dirname.find("video") != std::string::npos || lower_dirname.find("movie") != std::string::npos) {
            auto it = m_filename_map.find("Videos");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == "Public" || dirname == "public" || lower_dirname == "public") {
            auto it = m_filename_map.find("Public");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == "Templates" || dirname == "templates" || lower_dirname == "templates") {
            auto it = m_filename_map.find("Templates");
            if (it != m_filename_map.end()) {
                return it->second;
            }
        } else if (dirname == ".git" || dirname == ".github" || dirname == ".gitlab" || dirname == ".svn" || dirname == ".hg") {
            auto it = m_filetype_map.find("git");
            if (it != m_filetype_map.end()) {
                return it->second;
            }
        } else if (dirname == ".ssh" || dirname == ".gnupg" || dirname == ".config" || dirname == ".cache" || dirname == ".local") {
            auto it = m_filetype_map.find("hidden");
            if (it != m_filetype_map.end()) {
                return it->second;
            }
        }
        
        auto it = m_filetype_map.find("directory");
        if (it != m_filetype_map.end()) {
            return it->second;
        }
    }
    
    if (file.is_symlink) {
        auto it = m_filetype_map.find("symlink");
        if (it != m_filetype_map.end()) {
            return it->second;
        }
    }
    
    if (file.is_executable && !file.is_directory) {
        auto it = m_filetype_map.find("executable");
        if (it != m_filetype_map.end()) {
            return it->second;
        }
    }
    
    // Check for special file categories
    const std::string& filename = file.display_name.string();
    if (filename == "TODO" || filename == "TODO.md" || filename == "TODO.txt") {
        auto it = m_filename_map.find("TODO");
        if (it != m_filename_map.end()) {
            return it->second;
        }
    } else if (filename == "LICENSE" || filename == "LICENSE.md" || filename == "LICENSE.txt" || 
               filename == "COPYING" || filename == "COPYRIGHT") {
        auto it = m_filename_map.find("LICENSE");
        if (it != m_filename_map.end()) {
            return it->second;
        }
    } else if (filename == "README" || filename == "README.md" || filename == "README.txt") {
        auto it = m_filename_map.find("README.md");
        if (it != m_filename_map.end()) {
            return it->second;
        }
    }
    
    // Check filename mappings
    auto filename_it = m_filename_map.find(filename);
    if (filename_it != m_filename_map.end()) {
        return filename_it->second;
    }
    
    // Check for hidden or backup files
    if (FileOperations::isHidden(filename)) {
        auto it = m_filetype_map.find("hidden");
        if (it != m_filetype_map.end()) {
            return it->second;
        }
    }
    
    if (FileOperations::isBackupFile(filename)) {
        auto it = m_filetype_map.find("backup");
        if (it != m_filetype_map.end()) {
            return it->second;
        }
    }
    
    // Check extension mappings
    std::string extension = file.path.extension().string();
    if (!extension.empty()) {
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        auto ext_it = m_extension_map.find(extension);
        if (ext_it != m_extension_map.end()) {
            return ext_it->second;
        }
    }
    
    // Default fallback
    auto it = m_filetype_map.find("unknown");
    if (it != m_filetype_map.end()) {
        return it->second;
    }
    
    return {"\uf723", ""};
}

void IconProvider::setColorEnabled(bool enabled) {
    m_color_enabled = enabled;
}

bool IconProvider::isColorEnabled() const {
    return m_color_enabled;
}

std::string IconProvider::getRGB256Color(int r, int g, int b) const {
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string IconProvider::getNamedColor(const std::string& name) const {
    static const std::unordered_map<std::string, std::string> named_colors = {
        {"black", "\033[30m"},
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"},
        {"bright_black", "\033[90m"},
        {"bright_red", "\033[91m"},
        {"bright_green", "\033[92m"},
        {"bright_yellow", "\033[93m"},
        {"bright_blue", "\033[94m"},
        {"bright_magenta", "\033[95m"},
        {"bright_cyan", "\033[96m"},
        {"bright_white", "\033[97m"},
    };
    
    auto it = named_colors.find(name);
    return it != named_colors.end() ? it->second : "";
}

bool IconProvider::isArchive(const std::string& extension) const {
    static const std::unordered_set<std::string> archive_extensions = {
        ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz", ".lz", ".lzma",
        ".deb", ".rpm", ".dmg", ".pkg", ".msi", ".cab", ".iso", ".tgz", ".txz", ".tbz", ".tlz"
    };
    return archive_extensions.count(extension) > 0;
}

bool IconProvider::isAudio(const std::string& extension) const {
    static const std::unordered_set<std::string> audio_extensions = {
        ".mp3", ".wav", ".flac", ".ogg", ".aac", ".m4a", ".wma", ".opus", ".m3u", ".m3u8", ".pls"
    };
    return audio_extensions.count(extension) > 0;
}

bool IconProvider::isVideo(const std::string& extension) const {
    static const std::unordered_set<std::string> video_extensions = {
        ".mp4", ".avi", ".mov", ".mkv", ".wmv", ".flv", ".webm", ".m4v", ".3gp", ".ogv", ".mpeg", ".mpg"
    };
    return video_extensions.count(extension) > 0;
}

bool IconProvider::isImage(const std::string& extension) const {
    static const std::unordered_set<std::string> image_extensions = {
        ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".tif", ".svg", ".ico", ".webp",
        ".xcf", ".eps", ".raw", ".cr2", ".nef", ".arw", ".dng"
    };
    return image_extensions.count(extension) > 0;
}

bool IconProvider::isDocument(const std::string& extension) const {
    static const std::unordered_set<std::string> document_extensions = {
        ".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".odt", ".ods", ".odp", ".rtf",
        ".epub", ".mobi", ".azw", ".azw3", ".djvu"
    };
    return document_extensions.count(extension) > 0;
}

bool IconProvider::isCode(const std::string& extension) const {
    static const std::unordered_set<std::string> code_extensions = {
        ".c", ".cpp", ".cxx", ".cc", ".h", ".hpp", ".hxx", ".java", ".py", ".js", ".ts",
        ".jsx", ".tsx", ".cs", ".php", ".rb", ".go", ".rs", ".swift", ".kt", ".scala",
        ".elm", ".coffee", ".ejs", ".pug", ".sass", ".scss", ".less", ".styl", ".mustache",
        ".hbs", ".bat", ".ps1", ".sh", ".bash", ".zsh", ".fish", ".pl", ".pm", ".tcl",
        ".r", ".sql", ".lua", ".m", ".ml", ".hs", ".lhs", ".erl", ".hrl", ".lisp", ".lsp",
        ".clj", ".cljs", ".cljc", ".edn", ".ex", ".exs", ".eex", ".leex", ".heex", ".vim",
        ".psd", ".ai", ".sketch", ".blend"
    };
    return code_extensions.count(extension) > 0;
}

bool IconProvider::isConfig(const std::string& extension) const {
    static const std::unordered_set<std::string> config_extensions = {
        ".ini", ".conf", ".config", ".toml", ".yaml", ".yml", ".json", ".xml", ".cfg", ".env",
        ".settings", ".crt", ".pem", ".key", ".csr", ".der"
    };
    return config_extensions.count(extension) > 0;
}

bool IconProvider::isDatabase(const std::string& extension) const {
    static const std::unordered_set<std::string> database_extensions = {
        ".sql", ".sqlite", ".sqlite3", ".db", ".mdb", ".accdb"
    };
    return database_extensions.count(extension) > 0;
}

bool IconProvider::isFont(const std::string& extension) const {
    static const std::unordered_set<std::string> font_extensions = {
        ".ttf", ".otf", ".woff", ".woff2", ".eot", ".fon"
    };
    return font_extensions.count(extension) > 0;
}

