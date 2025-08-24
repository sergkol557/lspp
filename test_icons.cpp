#include <iostream>
#include <string>

int main() {
    // Test some of the Unicode icons used in ls++
    std::cout << "Testing icons:\n";
    std::cout << "Directory icon: \uf74a\n";
    std::cout << "File icon: \uf723\n";
    std::cout << "Executable icon: \uf713\n";
    std::cout << "Symlink icon: \uf838\n";
    std::cout << "C++ file icon: \ufb71\n";
    std::cout << "Python file icon: \uf81f\n";
    
    // Test if terminal supports Unicode
    std::cout << "\nTesting basic Unicode: α β γ δ ε\n";
    
    return 0;
}