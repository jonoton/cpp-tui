#include <iostream>
#include "cpptui.hpp"

int main() {
    std::cout << "cpp-tui Version: " << cpptui::version() << std::endl;
    std::cout << "Major: " << cpptui::VERSION_MAJOR << std::endl;
    std::cout << "Minor: " << cpptui::VERSION_MINOR << std::endl;
    std::cout << "Patch: " << cpptui::VERSION_PATCH << std::endl;
    
    if (cpptui::version() == "1.0.0") {
        return 0;
    } else {
        return 1;
    }
}
