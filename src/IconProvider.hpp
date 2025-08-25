#pragma once

#include <string>
#include <unordered_map>
#include <tuple>
#include <filesystem>
#include <utility>

// Forward declaration to avoid circular dependency
struct FileInfo;

class IconProvider {
public:
    IconProvider();
    
    std::string getIcon(const FileInfo& file) const;
    std::string getColorCode(const FileInfo& file) const;
    std::pair<std::string, std::string> getIconAndColor(const FileInfo& file) const;
    
    void setColorEnabled(bool enabled);
    bool isColorEnabled() const;
    
    // Static color codes
    static const std::string RESET_COLOR;
    static const std::string BOLD;
    static const std::string DIM;
    static const std::string UNDERLINE;
    static const std::string BLINK;
    static const std::string REVERSE;
    
private:
    bool m_color_enabled;
    
    // Extension-based mappings
    std::unordered_map<std::string, std::pair<std::string, std::string>> m_extension_map;
    
    // Filename-based mappings
    std::unordered_map<std::string, std::pair<std::string, std::string>> m_filename_map;
    
    // File type mappings
    std::unordered_map<std::string, std::pair<std::string, std::string>> m_filetype_map;
    
    void initializeExtensionMap();
    void initializeFilenameMap();
    void initializeFiletypeMap();
    
    std::string getFileTypeKey(const FileInfo& file) const;
    std::string normalizeExtension(const std::string& ext) const;
    
    // Color code generators
    std::string getRGB256Color(int r, int g, int b) const;
    std::string getNamedColor(const std::string& name) const;
    
    // File type detection helpers
    bool isArchive(const std::string& extension) const;
    bool isAudio(const std::string& extension) const;
    bool isVideo(const std::string& extension) const;
    bool isImage(const std::string& extension) const;
    bool isDocument(const std::string& extension) const;
    bool isCode(const std::string& extension) const;
    bool isConfig(const std::string& extension) const;
    bool isDatabase(const std::string& extension) const;
    bool isFont(const std::string& extension) const;
};