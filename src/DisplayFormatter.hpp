#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "ArgumentParser.hpp"
#include "FileOperations.hpp"
#include "IconProvider.hpp"

class DisplayFormatter {
public:
    DisplayFormatter(const LsOptions& options);
    
    void displayFiles(const std::vector<FileInfo>& files, std::ostream& out = std::cout);
    void displayLongFormat(const std::vector<FileInfo>& files, std::ostream& out = std::cout);
    void displayColumnar(const std::vector<FileInfo>& files, std::ostream& out = std::cout);
    void displayOnePerLine(const std::vector<FileInfo>& files, std::ostream& out = std::cout);
    void displayCommaSeparated(const std::vector<FileInfo>& files, std::ostream& out = std::cout);
    void displayAcross(const std::vector<FileInfo>& files, std::ostream& out = std::cout);
    
    static int getTerminalWidth();
    static size_t getDisplayWidth(const std::string& str);
    
private:
    const LsOptions& m_options;
    IconProvider m_icon_provider;
    
    std::string formatFileName(const FileInfo& file) const;
    std::string formatFileSize(off_t size, bool human_readable = false, bool si_units = false) const;
    std::string formatTime(const std::chrono::system_clock::time_point& time, const std::string& style = "locale") const;
    std::string formatPermissions(mode_t mode) const;
    std::string formatColoredPermissions(mode_t mode) const;
    std::string formatInode(ino_t inode) const;
    std::string formatBlockSize(off_t size, const std::string& block_size = "1024") const;
    
    std::string getColorCode(const FileInfo& file) const;
    std::string getIconAndColor(const FileInfo& file) const;
    void resetColor(std::ostream& out) const;
    
    std::string escapeFileName(const std::string& name) const;
    std::string quoteFileName(const std::string& name) const;
    
    // Layout calculation for columnar display
    struct ColumnLayout {
        size_t rows;
        size_t cols;
        std::vector<size_t> col_widths;
        size_t total_width;
    };
    
    ColumnLayout calculateLayout(const std::vector<FileInfo>& files, int terminal_width) const;
    std::vector<std::string> formatFilesForLayout(const std::vector<FileInfo>& files) const;
    
    // Long format helpers
    void displayLongHeader(const std::vector<FileInfo>& files, std::ostream& out) const;
    size_t calculateMaxWidths(const std::vector<FileInfo>& files) const;
    
    struct LongFormatWidths {
        size_t inode_width = 0;
        size_t blocks_width = 0;
        size_t links_width = 0;
        size_t owner_width = 0;
        size_t group_width = 0;
        size_t size_width = 0;
        size_t date_width = 0;
    };
    
    LongFormatWidths calculateLongFormatWidths(const std::vector<FileInfo>& files) const;
    void displaySingleFileLong(const FileInfo& file, const LongFormatWidths& widths, std::ostream& out) const;
};