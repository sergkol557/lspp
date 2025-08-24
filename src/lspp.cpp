#include "lspp.hpp"
#include <iostream>
#include <memory>

Lspp::Lspp() {
    m_argument_parser = std::make_unique<ArgumentParser>();
    m_file_operations = std::make_unique<FileOperations>();
}

void Lspp::run(int argc, char** argv) {
    try {
        LsOptions options = m_argument_parser->parse(argc, argv);
        processAndDisplay(options);
    } catch (const std::exception& e) {
        std::cerr << "ls++: " << e.what() << std::endl;
        std::exit(1);
    }
}

void Lspp::processAndDisplay(const LsOptions& options) {
    std::vector<FileInfo> all_files;
    
    if (options.paths.size() == 1 && options.paths[0] == ".") {
        // Single directory case - current directory
        all_files = m_file_operations->listDirectory(".", options);
    } else {
        // Multiple targets case
        all_files = m_file_operations->processTargets(options.paths, options);
    }
    
    // Create formatter and display results
    DisplayFormatter formatter(options);
    formatter.displayFiles(all_files, std::cout);
}
