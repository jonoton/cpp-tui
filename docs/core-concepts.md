---
layout: default
title: Core Concepts
nav_order: 3
---

## Core Concepts

How a cpp-tui program is put together: the `App` event loop, threading, the widget model, the two-phase layout engine, theming, events, and clipboard support.

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
#include <thread>

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

`update()` is a lighter alternative when the shared state is already thread-safe. It only wakes the event loop so it redraws; it does not make widget changes safe. For example, a widget that reads an atomic counter while rendering:

```cpp
#include <atomic>

std::atomic<int> downloaded{0};  // read by a widget during render

// On the worker thread:
downloaded = 42;
app.update();  // wake the event loop so it redraws now
```

To change widgets themselves from another thread, always use `post()`.
{: .callout .callout-warning}

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
- **Custom Theme**: You can create a `Theme` struct and assign custom `Color` values (including `scrollbar_track`, `input_placeholder`, etc).

| Built-in theme | Palette |
|----------------|---------|
| `Theme::Dark()` | Catppuccin Mocha — soothing pastel dark (default) |
| `Theme::Light()` | Catppuccin Latte — soothing pastel light |
| `Theme::Nord()` | Arctic, north-bluish palette |
| `Theme::TokyoNight()` | Modern dark theme inspired by Tokyo's nightscape |
| `Theme::SolarizedLight()` | Classic scientific palette by Ethan Schoonover |

### Events

Events are handled by the `App` and propagated to widgets.
- **Key Events**: Pressing keys like <kbd>Enter</kbd>, <kbd>Esc</kbd>, <kbd>Tab</kbd>, or arrow keys. Global shortcuts can be registered with `register_key()` — these take priority over widget-level key handling.
- **Mouse Events**: Clicks, drags, and scrolling.
- **Focus**: <kbd>Tab</kbd> / <kbd>Shift</kbd>+<kbd>Tab</kbd> cycles focus between interactive widgets.
- **Resize**: when the terminal is resized, the `App` re-runs layout and redraws automatically.

#### Default controls

| Input | Action |
|-------|--------|
| <kbd>Ctrl</kbd>+<kbd>C</kbd> | Force quit (copy when a text widget has a selection). Use `App::quit()` for a graceful exit. |
| <kbd>Tab</kbd> / <kbd>Shift</kbd>+<kbd>Tab</kbd> | Move focus forward / backward |
| Arrow keys | Move between items and rows, adjust sliders |
| <kbd>Enter</kbd> / <kbd>Space</kbd> | Activate buttons, toggle checkboxes, submit |
| <kbd>PageUp</kbd> / <kbd>PageDown</kbd> | Scroll a page in scrollable containers and tables |
| Mouse wheel | Vertical scroll |
| <kbd>Ctrl</kbd> + mouse wheel | Horizontal scroll (scrollable containers, `TextArea`) |
| Click | Focus a widget, press a button, place the cursor |
| Drag | Select text, drag a slider thumb, resize a `SplitPane` |

### Clipboard & History

Global default keybindings are available for text editing widgets (`Input`, `TextArea`, and selectable `Label`).

| Action | Keys |
|--------|------|
| Copy | <kbd>Ctrl</kbd>+<kbd>C</kbd> or <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>C</kbd> |
| Cut ¹ | <kbd>Ctrl</kbd>+<kbd>X</kbd> or <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd> |
| Paste ¹ | <kbd>Ctrl</kbd>+<kbd>V</kbd> or <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>V</kbd> |
| Undo ¹ | <kbd>Ctrl</kbd>+<kbd>Z</kbd> |
| Redo ¹ | <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>Z</kbd> or <kbd>Ctrl</kbd>+<kbd>Y</kbd> |
| Select all | <kbd>Ctrl</kbd>+<kbd>A</kbd> or triple-click |

¹ Editable widgets only.
