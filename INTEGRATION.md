# Integration Guide

`cpptui` is designed to be dropped into any C++ project.

## CMake

If using CMake, you can add it as an interface library:

```cmake
add_library(cpptui INTERFACE)
target_include_directories(cpptui INTERFACE include/) # or wherever cpptui.hpp is

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpptui)
```

## Direct File

Since it's a single header, you can just download `cpptui.hpp` and put it next to your source file.

```bash
g++ -std=c++17 -I. main.cpp -o my_app
```

## Requirements

- **C++ Standard**: C++17 or later.
- **OS**: Windows 10/11 (for Virtual Terminal support) or Linux/macOS.
- **Terminal**: A terminal emulator that supports ANSI escape codes (most modern terminals).
