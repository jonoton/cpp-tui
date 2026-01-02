#include <iostream>
#include "cpptui.hpp"

int main()
{
    std::cout << "cpp-tui Version: " << cpptui::version() << std::endl;
    std::cout << "Major: " << cpptui::VERSION_MAJOR << std::endl;
    std::cout << "Minor: " << cpptui::VERSION_MINOR << std::endl;
    std::cout << "Patch: " << cpptui::VERSION_PATCH << std::endl;

    return 0;
}
