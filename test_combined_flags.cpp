#include "src/ArgumentParser.hpp"
#include <iostream>
#include <cassert>

int main() {
    ArgumentParser parser;
    
    // Simulate command line arguments for testing -la flag
    char* argv1[] = {(char*)"ls++", (char*)"-la", (char*)"."};
    LsOptions options1 = parser.parse(3, argv1);
    
    // Test that both -l and -a flags are set
    assert(options1.format == ListFormat::LONG);
    assert(options1.show_all == true);
    
    // Simulate command line arguments for testing -laF flag
    char* argv2[] = {(char*)"ls++", (char*)"-laF", (char*)"."};
    LsOptions options2 = parser.parse(3, argv2);
    
    // Test that -l, -a, and -F flags are set
    assert(options2.format == ListFormat::LONG);
    assert(options2.show_all == true);
    assert(options2.show_file_type == true);
    
    std::cout << "All tests passed! Combined short flags functionality works correctly." << std::endl;
    
    return 0;
}