---
layout: default
title: Getting Started
nav_order: 2
---

## Getting Started

This guide walks you through setting up `cpp-tui`, integrating it into your build system, and creating your first interactive terminal UI application.

---

### System Requirements

Before using `cpp-tui`, ensure your environment meets the following baseline requirements:

| Component | Requirement |
|-----------|-------------|
| **C++ Standard** | C++17 or later |
| **Operating System** | Linux, macOS, or Windows 10/11 (with Virtual Terminal ANSI support) |
| **Compiler** | C++17 with `<filesystem>`: GCC 9+, Clang 9+, MSVC 2019+, or Xcode 11+ |
| **Terminal** | Any modern terminal emulator supporting ANSI escape codes |

---

### Installation

`cpp-tui` is distributed as a **single header file**. You do not need to compile a separate static or shared library.

Copy `cpptui.hpp` from the repository root into your project's include path:

```cpp
#include "cpptui.hpp"
using namespace cpptui;
```

> `cpp-tui` has zero external dependencies beyond standard C++17 runtime headers (`<iostream>`, `<memory>`, `<vector>`, `<string>`, `<thread>`, etc.). No ncurses or terminal driver libraries are needed.
{: .callout .callout-tip}

---

### Integration

You can integrate `cpp-tui` either directly with compiler flags or via CMake.

#### 1. Direct Compiler Invocation

If you prefer compiling directly without a build system:

```bash
# GCC
g++ -std=c++17 -I. main.cpp -o my_app

# Clang
clang++ -std=c++17 -I. main.cpp -o my_app

# MSVC (Developer Command Prompt)
cl /std:c++17 /utf-8 /I. main.cpp
```

#### 2. CMake Integration

##### Option A: CMake `FetchContent` (Recommended)

Add `cpp-tui` to your `CMakeLists.txt` to automatically download and configure it:

```cmake
include(FetchContent)
FetchContent_Declare(
    cpptui
    GIT_REPOSITORY https://github.com/jonoton/cpp-tui.git
    GIT_TAG        main # Or a specific release tag
)
FetchContent_MakeAvailable(cpptui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpptui::cpptui)
```

##### Option B: Git Submodule / `add_subdirectory`

If you keep `cpp-tui` as a submodule or nested folder in your repository:

```cmake
add_subdirectory(extern/cpp-tui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpptui::cpptui)
```

> When `cpp-tui` is included as a subproject via `add_subdirectory` or `FetchContent`, its built-in example executables are disabled by default. To build the examples anyway, pass `-DCPPTUI_BUILD_EXAMPLES=ON` to CMake.
{: .callout .callout-note}

---

### Minimal Working Example

Here is a complete, standalone program creating a window with a greeting and an exit button:

```cpp
#include "cpptui.hpp"
using namespace cpptui;

int main() {
    App app;
    Theme::set_theme(Theme::Dark());

    auto root = std::make_shared<Vertical>();
    auto label = std::make_shared<Label>("Hello, cpp-tui!");
    auto button = std::make_shared<Button>("Quit", []() {
        App::quit();
    });

    root->add(label);
    root->add(std::make_shared<VerticalSpacer>(1));
    root->add(button);

    // Press 'q' or click Quit to exit
    app.register_exit_key('q');
    app.run(root);
    return 0;
}
```

---

### Adding Widgets and Layouts

Widgets are managed with `std::shared_ptr<Widget>` and added to layout containers like `Vertical` or `Horizontal`. Layouts can be nested arbitrarily to form columns and rows:

```cpp
auto layout = std::make_shared<Vertical>();

// Read-only text
layout->add(std::make_shared<Label>("User Registration"));
layout->add(std::make_shared<VerticalSpacer>(1));

// Interactive text input
auto username = std::make_shared<Input>();
username->placeholder = "Enter username...";
layout->add(username);

// Horizontal row of buttons
auto button_row = std::make_shared<Horizontal>();
auto submit_btn = std::make_shared<Button>("Submit", [username]() {
    std::string val = username->get_value();
    // Process input value...
});
auto cancel_btn = std::make_shared<Button>("Cancel", []() {
    App::quit();
});

button_row->add(submit_btn);
button_row->add(std::make_shared<HorizontalSpacer>(2));
button_row->add(cancel_btn);

layout->add(button_row);
```

---

### Theming

Apply any built-in color scheme with a single call before or during the event loop:

```cpp
Theme::set_theme(Theme::Dark());           // Catppuccin Mocha (Default)
Theme::set_theme(Theme::Light());          // Catppuccin Latte
Theme::set_theme(Theme::Nord());           // Arctic blue palette
Theme::set_theme(Theme::TokyoNight());     // Vibrant Tokyo neon dark
Theme::set_theme(Theme::SolarizedLight()); // Classic scientific palette
```

> You can switch themes at any point during runtime — for example, inside a button click callback. `cpp-tui` will immediately trigger a redraw with the new color palette.
{: .callout .callout-tip}

---

### Examples

The repository ships with runnable examples in `examples/`. They build by default when cpp-tui is the top-level CMake project:

```bash
cmake -S . -B build
cmake --build build
./build/widget_demo
```

<div class="shot-grid">
<figure class="shot"><a href="{{ '/assets/img/widget_demo.png' | relative_url }}"><img src="{{ '/assets/img/widget_demo.png' | relative_url }}" alt="widget_demo: tabs, inputs, TextArea, slider, toggle switch, checkboxes and a progress bar" width="1052" height="868" loading="eager"></a><figcaption><code>widget_demo</code></figcaption></figure>
<figure class="shot"><a href="{{ '/assets/img/system_debug_sim.png' | relative_url }}"><img src="{{ '/assets/img/system_debug_sim.png' | relative_url }}" alt="system_debug_sim: control panel, live metric bars, system info and a log viewer" width="1052" height="708" loading="eager"></a><figcaption><code>system_debug_sim</code></figcaption></figure>
<figure class="shot"><a href="{{ '/assets/img/htop_sim.png' | relative_url }}"><img src="{{ '/assets/img/htop_sim.png' | relative_url }}" alt="htop_sim: CPU meters, memory bars, CPU history and a process table" width="1052" height="708" loading="eager"></a><figcaption><code>htop_sim</code></figcaption></figure>
<figure class="shot"><a href="{{ '/assets/img/spectrum_sim.png' | relative_url }}"><img src="{{ '/assets/img/spectrum_sim.png' | relative_url }}" alt="spectrum_sim: real-time spectrum analyzer line chart with a hover tooltip" width="1052" height="708" loading="eager"></a><figcaption><code>spectrum_sim</code></figcaption></figure>
</div>

> These screenshots are generated from the real example binaries by `scripts/screenshots/generate.sh`; re-run it after changing an example or the library.
{: .callout .callout-note}

| Target | Description |
|--------|-------------|
| `widget_demo` | Comprehensive showcase of all widgets, inputs, containers, and theme toggling |
| `chart_demo` | Every chart widget: line, scatter, bar, pie, box plot, heatmap, gauge, sparkline |
| `htop_sim` | Live system process monitor simulation using `TableScrollable` |
| `editor` | Text editor with `MenuBar`, `FileExplorer`, `SplitPane`, and line numbers |
| `docker_manager_sim` | Container manager dashboard with live stats and toast notifications |
| `key_events` | Global keyboard shortcuts with Ctrl/Alt/Shift modifiers and dialog triggers |
| `event_logger` | Real-time keyboard, mouse, and resize event inspector |
| `thread_update_demo` | Thread-safe UI updates from background worker threads using `app.post()` |
| `spectrum_sim` | Real-time RF spectrum analyzer simulation using `LineChart` |
| `system_debug_sim` | System diagnostics monitor with live logs and simulation controls |
| `test_tool_sim` | Test runner simulator with a results table and progress bars |
| `selection_test` | UTF-8 text selection, styled spans, and word wrapping across text widgets |
| `color_test` | Terminal 24-bit RGB truecolor spectrum rendering test |
| `layout_test` | Flex space distribution and constraint clamping layout verification |
| `nested_border_test` | Nested `Border` containers and auto-shrink sizing tests |
| `shadow_test` | Menu drop-shadow rendering over different backgrounds |
| `hello_world` | Minimal custom widget with a theme switcher |
| `version_check` | Prints `cpptui::version()` and the version macros |

---

### Next Steps

Continue exploring the library architecture and API reference:

- **[Core Concepts]({{ '/core-concepts' | relative_url }})** — Learn about the App lifecycle, the flex layout engine, event dispatch, and cross-thread updates.
- **[API Reference]({{ '/api-reference' | relative_url }})** — Complete property and method listings for all 50+ widgets and containers.
- **[Troubleshooting]({{ '/troubleshooting' | relative_url }})** — Practical solutions for terminal encoding, CJK fonts, and mouse tracking.
