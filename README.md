# cpp-tui

A lightweight, single-header C++ TUI (Text User Interface) library.

<img alt="widget_demo" src="https://drive.google.com/uc?id=1NM2GmkXnicG2QvCZTlvF4OLFBKGiHMv4" height="200px" style="margin:1px;" /> <img alt="spectrum_sim" src="https://drive.google.com/uc?id=1lulziaocb7SiDdzGJ11ZvxNgNskBsy7j" height="200px" style="margin:1px;" />

## Features

- **Single Header**: Easy integration with just `#include "cpptui.hpp"`.
- **Modern C++**: Designed for C++17.
- **Cross-Platform**: Works on Windows/POSIX.
- **Theme System**:
    - **Built-in Themes**: `Theme::Dark()` (Catppuccin Mocha), `Theme::Light()` (Catppuccin Latte), `Theme::Nord()`, `Theme::TokyoNight()`, `Theme::SolarizedLight()`.
    - **Dynamic Toggling**: Switch themes at runtime with `Theme::set_theme()`.
    - **Semantic Colors**: Widgets use abstract names (`primary`, `background`, `error`) for automatic styling.
    - **Global Background**: Application window automatically fills with the theme's background color.
- **Advanced Event System**:
    - **Keyboard**: Full support for arrow keys, function keys, modifiers (Shift/Ctrl/Alt). Global key bindings via `register_key()` with Ctrl/Alt/Shift modifier support — callbacks fire before widget-level dispatch.
    - **Focus Management**: Robust tab-key navigation (forward/reverse via Shift+Tab), click-to-focus support, and safe handling of nested/hidden widgets.
    - **Mouse**: Full tracking support including click, drag, scroll wheel, and hover.
    - **Resize**: Automatic layout updates on window resize (SIGWINCH).
    - **Cross-Thread Updates**: Thread-safe `update()` and `post()` methods to trigger redraws or schedule work from background threads.
- **Rich Widget Library**:
    - **Core**: `Label`, `Paragraph`, `Button`, `Input` (with Regex & Placeholders), `TextArea` (Multiline Editor), `Checkbox`, `ProgressBar`, `Sparkline`, `Spinner`, `Border`.
    - **Advanced Inputs**:
        - `ToggleSwitch`: Boolean toggle with custom on/off labels.
        - `NumberInput`: Integer input with stepper buttons (+/-) and validation.
        - `RadioSet`: Exclusive selection from a group of options.
        - `CheckboxList`: Manage multiple boolean options in a list.
        - `ColorPicker`: HSV/RGB color selection widget with gradient and hex input.
        - `Dropdown`: Collapsible selection menu.
    - **Lists & Tables**:
        - `TablePaginated`: Classic data table with automated pagination and footer controls.
        - `TableScrollable`: Efficient, infinite-scroll list with keyboard/mouse navigation (row selection, scrollbar, wheel support).
        - `TreeView`: Hierarchical tree structure.
        - `TextList`: Formatted lists with bullets or numbering and text selection.
    - **Containers**: `Tabs` (tabbed interface with overflow navigation), `Carousel` (slide-show), `Stack` (z-ordering), `ScrollableVertical`, `ScrollableHorizontal`, `ScrollableContainer` (2D panning), `Dialog`, `FileExplorer` (File system browser).
    - **Charting**:
        - `LineChart`: Multi-series line graphs (Points, Lines, Braille styles supported) with interactive tooltips.
        - `ScatterChart`: Plot points with custom markers and interactive tooltips.
        - `BarChart`: Vertical bar graphs with support for axes, ticks, and interactive tooltips.
    - **Menus**: Popup menu system with keyboard/mouse support.
- **Layouts**:
    - **Containers**: `Vertical`, `Horizontal`, `Grid`, and `Align`.
        - `VerticalSpacer(int height)`: Add fixed or flexible vertical space.
        - `HorizontalSpacer(int width)`: Add fixed or flexible horizontal space.
        - **Dynamic Flex Allocation**: Layouts divide remaining available space among non-fixed widgets (supported in `Vertical`, `Horizontal`, `ScrollableVertical`, and `ScrollableHorizontal`), applying a multi-pass clamping algorithm to satisfy `min_*`/`max_*` constraints.
        - **Min/Max Constraints**: Global `min_width`, `max_width`, `min_height`, and `max_height` properties are respected by layout containers during space negotiation and cross-axis alignment, with `min` overriding `max` (min-wins precedence).
        - **Auto-Shrink**: Containers can automatically shrink to wrap their children's size constraints via the `auto_shrink` property, propagating constraints recursively while respecting the container's own min/max limits.
        - **Explicit Sizing Preservation**: Container widgets (like `Border`) respect child fixed width/height constraints and avoid stretching them.
    - **Visibility & Responsive**:
        - Global `.visible` property on all widgets.
        - **Responsive**: Auto-hide/show based on terminal width (`set_responsive()`).

## Build and Run

This project uses **CMake**.

1. **Build**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
   *Note: Examples are built by default when compiling this repository directly. You can disable them by passing `-DCPPTUI_BUILD_EXAMPLES=OFF` to CMake.*

2. **Run Examples**:
   - **Widget Demo** (Feature Showcase):
     ```bash
     ./widget_demo
     ```
     - Shows all widgets, **Theme Toggling**, and Layouts.
   
   - **Htop Simulator**:
     ```bash
     ./htop_sim
     ```
     - Simulates a process monitor using `TableScrollable`.

   - **Chart Demo**:
     ```bash
     ./chart_demo
     ```

   - **Key Events Demo**:
     ```bash
     ./key_events
     ```
     - Demonstrates global keyboard shortcut registration with Ctrl/Alt/Shift modifiers.

## Quick Start

You can use `cpp-tui` by simply copying `cpptui.hpp` to your project. Alternatively, if you use **CMake**, you can integrate it via `add_subdirectory` or `FetchContent`. The examples are disabled by default when imported as a subproject.

```cmake
# Example using FetchContent
include(FetchContent)
FetchContent_Declare(
    cpptui
    GIT_REPOSITORY https://github.com/jonoton/cpp-tui.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(cpptui)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE cpptui::cpptui)
```

Then, include the header and create your `App`:

```cpp
#include "cpptui.hpp"
using namespace cpptui;

int main() {
    // Optional: Set Theme
    Theme::set_theme(Theme::Light());

    App app;
    auto root = std::make_shared<Vertical>();
    
    auto btn = std::make_shared<Button>("Click Me", [](){
        Theme::set_theme(Theme::Dark()); // Toggle theme on click
    });
    
    root->add(btn);
    // Add a spacer
    root->add(std::make_shared<VerticalSpacer>(1));
    
    app.run(root);
    return 0;
}
```

## Controls
- **Global**: `Ctrl+C` to force quit (or specific keys like `q` in demos). Use `App::quit()` for graceful exit.
- **Navigation**: Tab / Shift+Tab to switch focus, Arrow Keys for selection.
- **Scrolling**: Mouse wheel (Vertical), Ctrl+Wheel (Horizontal). Keys: PageUp/PageDown (Vertical).
- **Mouse**: Click to focus/activate, Scroll wheel for lists/text.
- **Clipboard & History**:
    - **Copy/Cut/Paste**: Standard shortcuts (`Ctrl+C/X/V`) supported on `Input`, `TextArea`, and `Label`.
    - **Undo/Redo**: `Ctrl+Z` / `Ctrl+Y` supported on editable text widgets.
