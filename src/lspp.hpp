#pragma once

#include <memory>
#include "ArgumentParser.hpp"
#include "FileOperations.hpp"
#include "DisplayFormatter.hpp"
#include "IconProvider.hpp"

class Lspp {
public:
    Lspp();
    
    void run(int argc, char** argv);
    
private:
    std::unique_ptr<ArgumentParser> m_argument_parser;
    std::unique_ptr<FileOperations> m_file_operations;
    
    void processAndDisplay(const LsOptions& options);
};
