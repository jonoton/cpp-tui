---
layout: default
title: Core Concepts
nav_order: 3
---

## Core Concepts

### App

The `App` class is the entry point of your application. It initializes the terminal, handles input events, and manages the main event loop.

```cpp
#include "cpptui.hpp"
using namespace cpptui;

int main() {
    App app;
    auto root = std::make_shared<Vertical>();
    // ... add widgets to root ...
    app.run(root);
    return 0;
}
```

- **Graceful Exit**: `App::quit()` can be called to exit the event loop and restore the terminal state.
- **Exit Keys**: `register_exit_key(int key, bool ctrl=false, bool alt=false, bool shift=false)` adds a key that will trigger application exit.
- **Key Bindings**: `register_key(int key, callback, bool ctrl=false, bool alt=false, bool shift=false, bool consume=true)` registers a global keyboard shortcut with optional modifiers. The callback fires before widget-level dispatch. Supports unregistration via `unregister_key()`. See `examples/key_events.cpp` for a working demo.
- **Timers**:
  - `add_timer(interval_ms, callback)`: Schedule a repeating callback. Returns a `TimerId`.
  - `remove_timer(timer_id)`: Stop and remove a timer.
- **Thread-Safe Updates**:
  - `update()`: Wake the event loop and trigger a redraw. Thread-safe — can be called from any thread.
  - `post(callback)`: Schedule a callback to run on the main UI thread, then redraw. Thread-safe — use this to safely modify widgets from background threads without data races.
- **Dialogs**:
  - `open_dialog(dialog)`: Opens a dialog auto-centered on screen.
  - `open_dialog(dialog, x, y)`: Opens a dialog at specific coordinates.
  - `close_dialog(dialog)`: Closes the specified dialog.

### Threading

Background threads can update the UI without polling. Use `post()` to schedule widget modifications on the main thread:

```cpp
App app;
auto label = std::make_shared<Label>("Waiting...");

std::thread worker([&]() {
    // Do work...
    app.post([&]() {
        label->set_text("Done!");  // Runs safely on the UI thread
    });
});
worker.detach();

app.run(root);
```

For advanced use where you manage your own synchronization, `update()` provides a lighter alternative:

```cpp
label->set_text("Updated");  // Caller is responsible for thread safety
app.update();                 // Immediately wakes the event loop to redraw
```

### Widget

Every visual element in cpp-tui inherits from the `Widget` class. Widgets have:
- **Geometry**: `x`, `y`, `width`, `height`.
- **Sizing Policy**: `fixed_width`, `fixed_height` (0 or negative means flexible) and size constraints: `min_width`, `max_width`, `min_height`, `max_height`.
- **Rendering**: `render(Buffer& buffer)` method to draw to the screen.
- **Visibility**: `visible` property to show/hide.
- **Responsive Width**: `set_responsive(sm, md, lg)` or `set_responsive_width(sm, md, lg)` to auto-toggle based on screen width.
- **Responsive Height**: `set_responsive_height(sm, md, lg)` to auto-toggle based on screen height.
- **Events**: `on_click`, `on_key`, `on_hover`, etc.
- **Focus**: `set_focus(bool)` to manually set focus, `has_focus()` to check state.
- **Tab Navigation**: `tab_stop` (bool) determines if the widget is included in Tab cycle.
- **Hover**: `on_hover(bool)` callback triggered when mouse enters/exits.
- **Tooltips**:
  - `set_tooltip(text)`: Quickly attach a text tooltip.
  - `set_tooltip(std::shared_ptr<Tooltip>)`: Attach a custom or shared `Tooltip` instance.

### Layouts & Sizing Flow

Widgets in `cpp-tui` participate in a two-phase layout negotiation:

1. **Responsive and Size Constraint Resolution (`update_responsive()`)**:
   - Containers and widgets compute their size requests before layout.
   - If a container has `auto_shrink = true`, it recursively calculates its fixed size (`fixed_width`/`fixed_height`) based on the sum or maximum of its children's fixed sizes plus container padding.
   - During auto-shrink size calculation, constraints are propagated: the computed size is clamped using the container's own `min_width`/`min_height` and `max_width`/`max_height`.
   - If `auto_shrink = false` (the default), the container behaves flexibly and expands to fill the available space allocated by its parent.

2. **Actual Placement (`layout()`)**:
   - The parent container allocates actual positions (`x`, `y`) and dimensions (`width`, `height`) to its children.
   - **Flexible Space Distribution (Multi-Pass Clamping)**: In layouts like `Vertical`, `Horizontal`, `ScrollableVertical`, and `ScrollableHorizontal`, remaining available space is dynamically distributed among flexible children. This is calculated using a multi-pass algorithm: space is initially split evenly, then children that violate their `min_*` or `max_*` constraints are clamped and settled, and the remaining space is redistributed among the remaining unsettled flex children.
   - **Precedence (Min Wins)**: In case of conflicting constraints (e.g. `min_width > max_width`), the `min` constraint overrides the `max` constraint (matching standard CSS behavior).
   - **Explicit Size Preservation**: Containers respect child constraints; a child inside a container with an explicit fixed size will not be stretched to fill the container's entire area. Additionally, a child's cross-axis size is always clamped using its `min` and `max` constraints.

### Theme

The `Theme` class manages the color palette of the application.

- **Set Theme**: `Theme::set_theme(Theme::Dark())` or any built-in theme.
- **Built-in Themes**:
  - `Theme::Dark()` - Catppuccin Mocha (soothing pastel dark theme)
  - `Theme::Light()` - Catppuccin Latte (soothing pastel light theme)
  - `Theme::Nord()` - Arctic, north-bluish color palette
  - `Theme::TokyoNight()` - Modern dark theme inspired by Tokyo's nightscape
  - `Theme::SolarizedLight()` - Classic scientific color palette by Ethan Schoonover
- **Custom Theme**: You can create a `Theme` struct and assign custom `Color` values (including `scrollbar_track`, `input_placeholder`, etc).

### Events

Events are handled by the `App` and propagated to widgets.
- **Key Events**: Pressing keys like `Enter`, `Esc`, `Tab`, or arrow keys. Global shortcuts can be registered with `register_key()` — these take priority over widget-level key handling.
- **Mouse Events**: Clicks, drags, and scrolling.
- **Focus**: `Tab` / `Shift+Tab` cycles focus between interactive widgets.
- **Resize**: `g_resize_pending` signal handling for terminal resize events.

### Clipboard & History

Global default keybindings are available for text editing widgets (`Input`, `TextArea`, and selectable `Label`).
- **Copy**: `Ctrl+C` or `Ctrl+Shift+C`
- **Cut**: `Ctrl+X` or `Ctrl+Shift+X` (Editable widgets only)
- **Paste**: `Ctrl+V` or `Ctrl+Shift+V` (Editable widgets only)
- **Undo**: `Ctrl+Z` (Editable widgets only)
- **Redo**: `Ctrl+Shift+Z` or `Ctrl+Y` (Editable widgets only)
- **Select All**: `Ctrl+A` or **Triple-click**
