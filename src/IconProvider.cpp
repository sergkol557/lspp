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
        
        // Config files
        {".ini", {"\uf013", "\033[38;2;3;136;209m"}},
        {".conf", {"\uf013", "\033[38;2;3;136;209m"}},
        {".config", {"\uf013", "\033[38;2;3;136;209m"}},
        {".toml", {"\uf013", "\033[38;2;3;136;209m"}},
        {".properties", {"\uf013", "\033[38;2;3;136;209m"}},
        
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
    
    // Check filename mappings
    const std::string& filename = file.display_name.string();
    auto filename_it = m_filename_map.find(filename);
    if (filename_it != m_filename_map.end()) {
        return filename_it->second;
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
        ".deb", ".rpm", ".dmg", ".pkg", ".msi", ".cab", ".iso"
    };
    return archive_extensions.count(extension) > 0;
}

bool IconProvider::isAudio(const std::string& extension) const {
    static const std::unordered_set<std::string> audio_extensions = {
        ".mp3", ".wav", ".flac", ".ogg", ".aac", ".m4a", ".wma", ".opus"
    };
    return audio_extensions.count(extension) > 0;
}

bool IconProvider::isVideo(const std::string& extension) const {
    static const std::unordered_set<std::string> video_extensions = {
        ".mp4", ".avi", ".mov", ".mkv", ".wmv", ".flv", ".webm", ".m4v", ".3gp"
    };
    return video_extensions.count(extension) > 0;
}

bool IconProvider::isImage(const std::string& extension) const {
    static const std::unordered_set<std::string> image_extensions = {
        ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".tif", ".svg", ".ico", ".webp"
    };
    return image_extensions.count(extension) > 0;
}

bool IconProvider::isDocument(const std::string& extension) const {
    static const std::unordered_set<std::string> document_extensions = {
        ".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".odt", ".ods", ".odp", ".rtf"
    };
    return document_extensions.count(extension) > 0;
}

bool IconProvider::isCode(const std::string& extension) const {
    static const std::unordered_set<std::string> code_extensions = {
        ".c", ".cpp", ".cxx", ".cc", ".h", ".hpp", ".hxx", ".java", ".py", ".js", ".ts",
        ".jsx", ".tsx", ".cs", ".php", ".rb", ".go", ".rs", ".swift", ".kt", ".scala"
    };
    return code_extensions.count(extension) > 0;
}

bool IconProvider::isConfig(const std::string& extension) const {
    static const std::unordered_set<std::string> config_extensions = {
        ".ini", ".conf", ".config", ".toml", ".yaml", ".yml", ".json", ".xml"
    };
    return config_extensions.count(extension) > 0;
}

bool IconProvider::isDatabase(const std::string& extension) const {
    static const std::unordered_set<std::string> database_extensions = {
        ".sql", ".sqlite", ".db", ".mdb", ".accdb"
    };
    return database_extensions.count(extension) > 0;
}