---
layout: default
title: Getting Started
nav_order: 2
---

## Getting Started

### Requirements

- **C++ Standard**: C++17 or later.
- **OS**: Windows 10/11 (for Virtual Terminal support) or Linux/macOS.
- **Terminal**: A terminal emulator that supports ANSI escape codes (most modern terminals).

### Installation

cpp-tui is a **single-header library**. Copy `cpptui.hpp` (located in the project root) into your project and include it:

```cpp
#include "cpptui.hpp"
using namespace cpptui;
```

No external dependencies are required.

### Integration

#### CMake

*Note: The `cpp-tui` examples are not built by default when imported as a subproject.*

**Using `add_subdirectory`**

If you include `cpp-tui` as a Git submodule or copy the repository into your project, you can add it directly:

```cmake
add_subdirectory(path/to/cpp-tui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpptui::cpptui)
```

**Using `FetchContent`**

You can fetch `cpp-tui` directly from GitHub using CMake's `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    cpptui
    GIT_REPOSITORY https://github.com/jonoton/cpp-tui.git
    GIT_TAG        main # or a specific version tag
)
FetchContent_MakeAvailable(cpptui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpptui::cpptui)
```

#### Direct File

Since it's a single header, you can just download `cpptui.hpp` and put it next to your source file.

```bash
g++ -std=c++17 -I. main.cpp -o my_app
```

### Minimal Example

```cpp
#include "cpptui.hpp"
using namespace cpptui;

int main() {
    App app;
    Theme::set_theme(Theme::Dark());

    auto root = std::make_shared<Vertical>();
    auto label = std::make_shared<Label>("Hello, cpp-tui!");
    auto button = std::make_shared<Button>("Quit", [&app]() {
        app.quit();
    });

    root->add(label);
    root->add(button);

    app.register_exit_key('q');
    app.run(root);
    return 0;
}
```

### Compiling

Compile with any C++17 compiler:

```bash
# g++
g++ -std=c++17 -o myapp main.cpp

# clang++
clang++ -std=c++17 -o myapp main.cpp
```

### Adding Widgets

Widgets are created as `std::shared_ptr` and added to layout containers:

```cpp
auto layout = std::make_shared<Vertical>();

// Text
layout->add(std::make_shared<Label>("Status: OK"));

// Input
auto input = std::make_shared<Input>();
input->placeholder = "Type here...";
layout->add(input);

// Button
layout->add(std::make_shared<Button>("Submit", [&input]() {
    std::string value = input->get_value();
    // process value...
}));
```

Layouts can be nested — put a `Horizontal` inside a `Vertical` to create columns within rows.

### Theming

Switch themes with a single call:

```cpp
Theme::set_theme(Theme::Dark());       // Catppuccin Mocha
Theme::set_theme(Theme::Light());      // Catppuccin Latte
Theme::set_theme(Theme::Nord());       // Arctic palette
Theme::set_theme(Theme::TokyoNight()); // Tokyo Night
```

### Next Steps

- **[Core Concepts](core-concepts)** — Understand the App lifecycle, Widget model, and event system
- **[API Reference](api-reference)** — Browse every widget and its properties
- **[Troubleshooting](troubleshooting)** — Solutions for common issues
