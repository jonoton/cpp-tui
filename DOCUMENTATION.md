# cpp-tui Documentation

## Introduction

cpp-tui is a lightweight, single-header C++ library for creating Text User Interfaces (TUIs). It is designed to be easy to integrate, using modern C++17 features, and provides a rich set of widgets and layout managers similar to web development or GUI frameworks.

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
- **Exit Keys**: `register_exit_key(char key)` adds a key that will trigger application exit.
- **Timers**:
  - `add_timer(interval_ms, callback)`: Schedule a repeating callback. Returns a `TimerId`.
  - `remove_timer(timer_id)`: Stop and remove a timer.
- **Dialogs**:
  - `open_dialog(dialog)`: Opens a dialog auto-centered on screen.
  - `open_dialog(dialog, x, y)`: Opens a dialog at specific coordinates.
  - `close_dialog(dialog)`: Closer the specified dialog.

### Widget

Every visual element in cpp-tui inherits from the `Widget` class. Widgets have:
- **Geometry**: `x`, `y`, `width`, `height`.
- **Sizing Policy**: `fixed_width`, `fixed_height` (0 or negative means flexible).
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
- **Key Events**: Pressing keys like `Enter`, `Esc`, `Tab`, or arrow keys.
- **Mouse Events**: Clicks, drags, and scrolling.
- **Focus**: `Tab` / `Shift+Tab` cycles focus between interactive widgets.

## API Reference

### Layouts

Layouts are container widgets that arrange their children.

#### `Vertical` / `Horizontal`
Arranges children in a vertical column or horizontal row.
- **Flexibility**: Children with `fixed_size <= 0` will expand to fill available space.

#### `VerticalSpacer` / `HorizontalSpacer`
Invisible widgets used to add spacing between other widgets in a layout.
- `VerticalSpacer(int height)`: Adds fixed vertical space (or flexible if height <= 0).
- `HorizontalSpacer(int width)`: Adds fixed horizontal space (or flexible if width <= 0).

#### `Align`
Aligns a single child within its available space.
- `Align::H`: Left, Center, Right.
- `Align::V`: Top, Center, Bottom.

#### `Grid`
Arranges children in a table-like grid.
- `add_item(widget, row, col, row_span, col_span)`

#### `SplitPane`
Resizable split panel container.
- `set_panes(pane1, pane2)`: Set child widgets.
- `ratio`: Divider position (0-1).
- `vertical`: Horizontal or vertical split.
- Mouse drag to resize.

#### `ScrollableVertical`
A vertical layout that scrolls if its content exceeds its height.
- **Clipping**: Content is clipped to the container's bounds.

#### `ScrollableHorizontal`
A horizontal layout that scrolls if its content exceeds its width.
- Supports horizontal scrollbar and keyboard navigation.

#### `ScrollableContainer`
Bi-directional scrollable container for panning over large 2D content.
- `scroll_x`, `scroll_y`: Current scroll offsets.
- Supports both horizontal and vertical scrollbars.

### Containers

All container widgets inherit from `Container` and support:
- `add(widget)`: Add a child widget to the container.
- `clear_children()`: Remove all child widgets.

#### `Border`
Wraps a child widget with a border and optional title.
- `set_title(text, alignment)`
- `BorderStyle`: Single, Double, Rounded.

#### `Tabs`
Displays content in switchable tabs.
- `add_tab(title, widget)`
- **Overflow**: Displays navigation buttons if tabs exceed width.
- **Focus**: Tab buttons are focusable and can be selected via keyboard (Enter/Space).
- `on_change`: Callback fired when the active tab changes.

#### `Carousel`
Shows one page (widget) at a time, with optional navigation arrows.
- `add_page(widget)`
- `show_dots`, `show_arrows`: Configure navigation controls.

#### `Accordion`
Collapsible content sections.
- `add_section(title, content, expanded)`: Add section.
- `allow_multiple`: Allow multiple expanded sections.
- Arrow keys + Enter/Space to navigate and toggle.

#### `Dialog`
A general purpose popup dialog with border and shadow.
- `show(x, y)` / `hide()`: Open and close the dialog.
- `modal`: If true, blocks input to widgets behind it.
- `shadow`: Renders a drop shadow effect.
- Use `App::open_dialog()` to display.

### Basic Widgets

#### `Static`
Displays simple static text, useful for minimal text rendering.

#### `Label`
Displays read-only text with color support.

#### `Paragraph`
Multi-line text widget with word wrapping and indentation.
- `text`: Plain text content.
- `styled_content`: Use `StyledText` for mixed formatting.
- `first_line_indent`: Indentation for the first line.
- `hanging_indent`: Indentation for subsequent lines.
- `word_wrap`: Enable/disable word wrapping.
- **Styles**: `bold`, `italic`, `underline` bool properties for global styling.

#### `TextSpan` / `StyledText`
Builder pattern for creating inline styled text with mixed formatting.
```cpp
StyledText styled;
styled.add_bold("Bold text ")
      .add_italic("italic text ")
      .add_color("colored", Color::Red());

auto para = std::make_shared<Paragraph>(styled);
```
- **Bold**: `add_bold(text)`
- **Italic**: `add_italic(text)`
- **Underline**: `add_underline(text)`
- **Colored**: `add_color(text, color)`, `add_bold_color(text, color)`

#### `Button`
Clickable button with a callback.
- `on_click`: `std::function<void()>`

#### `Badge`
Compact inline status indicator.
- `text`: Badge label.
- `badge_bg`, `text_color`: Styling.
- `style`: `Pill`, `Square`, or `Outline`.

#### `Checkbox`
Toggleable state.
- `is_checked()`

#### `ProgressBar`
Visualizes a float value from 0.0 to 1.0.
- `max_height`: Limit vertical size.
- `vertical`: Render vertically instead of horizontally.
- **Styling**:
  - `show_text`: Show progress percentage/custom text.
  - `text_formatter`: `std::function<std::string(float)>` for custom text.
  - `text_color`: Color of the progress text.
  - `char_filled`, `char_empty`: Custom characters for loading bar (e.g. "█", "░").
- **color**: Main progress bar color.

#### `Sparkline`
Displays a simple line graph of a data series.
- **Tooltips**: Supports generic widget tooltips (`set_tooltip`) only.

#### `Spinner`
Animated activity indicator with multiple styles.
- **Indeterminate**: Spins continuously for unknown-duration tasks.
  - `StyleBrailleSpin()`, `StyleLineSpin()`, `StylePieSpin()`, `StyleBounceBar()`, `StylePulseAscii()`
- **Determinate**: Maps value 0-1 to frames for progress display.
  - `StylePieProgress()`, `StyleBarProgress()`
- `value`: Set >= 0 for determinate mode, < 0 for spinning.
- `color`: Spinner character color.
- Requires `App*` pointer for timer-based animation.

#### `Tooltip`
Hover-triggered popup.
- `text`: Tooltip content.
- `attach(widget)`: Attach to target widget.
- `position`: Top, Bottom, Left, or Right.
- `show()`, `hide()`: Manual control.

#### `Notification`
Toast-style auto-dismissing messages.
- `show(text, type, duration_ms)`: Display notification.
- `Type`: Info, Success, Warning, Error.
- `position`: TopRight (default), TopLeft, BottomRight, BottomLeft, TopCenter, BottomCenter.
- `max_visible`: Maximum concurrent notifications.

### Advanced Inputs

#### `Input`
Text input field. Focusable.
- `get_value()` / `set_value()`
- `placeholder`: Text to show when empty.
- `regex_pattern`: Validation regex (e.g. `^\\d+$`).
- `empty_char`: Character filler (defaults to space).

#### `SearchInput`
Input with autocomplete dropdown.
- `suggestions`: Vector of suggestion strings.
- `on_search`, `on_change`: Callbacks.
- `suggestion_limit`: Maximum number of suggestions to display (default 5).
- **Navigation**: Arrow keys to select suggestions, Enter to confirm.
- **Triggers**: Typing filters suggestions. `Ctrl+Space` manually opens the suggestions list.

#### `ToggleSwitch`
A modern alternative to checkboxes.
- `active_color`, `inactive_color`.
- `on_label`, `off_label`: Custom text for states (default "[ON]", "[OFF]").

#### `NumberInput`
Integer input with stepper buttons.
- `min_value`, `max_value`, `step`.
- `set_position(ButtonPos::Left|Right|Split)`: Configure button layout.

#### `Slider`
A horizontal or vertical range slider for selecting numeric values.
- `min_value`, `max_value`, `value`, `step`: Range configuration.
- `vertical`: Set to `true` for vertical orientation.
- `on_change`: Callback when value changes.
- Mouse drag and arrow keys supported.

#### `RadioSet`
Mutually exclusive selection from a list of strings.
- `get_selected_index()`
- `set_style(selected_prefix, unselected_prefix)`: Custom markers.
- `selected_color`: Color for the selected item.

#### `CheckboxList`
Multiple selection from a list.
- `get_checked_indices()`
- `checked_prefix`, `unchecked_prefix`: Custom markers.
- `checked_color`: Color for checked items.
- `cursor_color`: Color for the focused item.

#### `ColorPicker`
HSV/RGB color selection widget.
- `get_color()` / `set_color(Color)`: Get/Set selected color.
- `set_hex(string)` / `get_hex()`: Hex string support.
- **Interactions**:
    - **Gradient**: Click or drag to pick Hue/Saturation.
    - **Slider**: Click or drag to pick Value (Brightness).
    - **Inputs**: Edit Hex/RGB components (if `show_values` is true).
- `on_change`: Callback for real-time updates.

#### `Dropdown`
Collapsible selection list.

#### `Calendar`
Month-view calendar widget with button navigation.
- `year`, `month`, `selected_day`: Current date.
- `highlighted_days`: Vector of highlighted dates.
- `on_select`: Callback on date selection.
- Tab to `<`/`>` buttons or day grid, arrow keys to navigate days.

### Lists & Tables

#### `TablePaginated`
Displays tabular data with automatic pagination controls in the footer.
- `columns`: `std::vector<std::string>`
- `rows`: `std::vector<std::vector<std::string>>`
- `auto_page_size`: `true` (default) to fit rows to height.

#### `TableScrollable`
Efficient list for large datasets (like process lists). Supports scrolling and row selection.
- `columns`: `std::vector<std::string>`
- `rows`: `std::vector<std::vector<std::string>>`
- `selected_index`: Index of the currently selected row.
- `scroll_offset`: Index of the first visible row.
- **Interactions**:
    - **Selection**: Mouse click or Arrow keys (Up/Down).
    - **Scrolling**: Mouse wheel or PageUp/PageDown.
    - **Scrollbar**: Interactive scrollbar for dragging.

#### `TreeView`
Displays hierarchical data with keyboard navigation and selection.
- `root_nodes`: `std::vector<TreeNode>`
- `TreeNode`: `label`, `expanded`, `children`, `selected`, `icon`, `color`.
- **Navigation**: Up/Down arrows, Left (collapse/parent), Right (expand), Enter/Space (toggle/submit).
- **Styling Options**:
  - `indent_unit`: String for indentation (default: two spaces).
  - `icon_expanded`, `icon_collapsed`, `icon_leaf`: Custom icons.
  - `selected_bg`, `selected_fg`: Colors for selected node.
- **Callbacks**:
  - `on_select`: Called when selection changes.
  - `on_submit`: Called on Enter/Space or double-click.
  - `on_expand`, `on_collapse`: Called when nodes expand/collapse.

### Charts

#### `LineChart` / `ScatterChart`
Plots numerical data (x, y). **Supports interactive data-point tooltips.**
- `add_series(data, label, color, style, marker)`
- **Styles**: `LineStyle::Points`, `LineStyle::Lines`, `LineStyle::Braille`.
- **Properties**: `show_legend`, `min_val`, `max_val`.
- **Axis Customization**:
  - `x_tick_count`, `y_tick_count`: Number of tick marks.
  - `x_tick_precision`, `y_tick_precision`: Decimal places for labels.
  - `x_tick_formatter`, `y_tick_formatter`: Custom label functions.
  - `label_intermediate_x_ticks`, `label_intermediate_y_ticks`: Show labels for intermediate ticks.
- **Tooltips**:
  - `show_tooltip`: Enable/disable interactive tooltips (on hover).
  - `tooltip_formatter`: `std::function<std::string(const Series&, int, double)>` to customize tooltip text.

#### `BarChart`
A vertical bar chart for categorical data.
- **Axes**: `show_x_axis`, `show_y_axis`, `show_x_ticks`, `show_y_ticks`.
- **Tick Customization**: Same options as LineChart.
- **Tooltips**:
  - `show_tooltip`: Enable/disable interactive tooltips (on hover).
  - `tooltip_formatter`: `std::function<std::string(const std::string& series, const std::string& category, double value)>` to customize tooltip text.

#### `Gauge`
Semi-circular progress meter.
- `value`: 0.0 to 1.0.
- `min_label`, `max_label`: Endpoint labels.
- `show_value`, `value_format`: Center value display.
- **Tooltips**: Supports generic widget tooltips (`set_tooltip`) only.

#### `ProportionalBar`
Horizontal bar visualization for proportional data.
- `add_segment(value, label, color)`: Add data segment.
- `show_legend`, `show_percentages`: Display options.
- **Tooltips**: Supports generic widget tooltips (`set_tooltip`) only.

#### `Heatmap`
2D color intensity grid.
- `data`: 2D vector of values (0-1).
- `row_labels`, `col_labels`: Axis labels.
- `low_color`, `high_color`: Gradient endpoints.
- **Tooltips**: Supports generic widget tooltips (`set_tooltip`) only.

### Navigation & Utility

#### `MenuDialog`
A popup menu widget with keyboard and mouse support.
- `items`: List of `MenuItem` with `{label, action, sub_items}`.
- `mouse_hover_select`: Highlight items on hover.
- `show(x, y)`: Display at screen position.

#### `MenuBar`
Horizontal top-level menu bar.
- `items`: List of top-level menu items.
- Integrates with `MenuDialog` for dropdown submenus.

#### `Breadcrumb`
Navigation trail widget.
- `add(label, on_click)`: Add breadcrumb item.
- `separator`: Custom separator string (default: " > ").
- Click items to navigate.

#### `ShortcutBar`
Formatted keyboard shortcut display.
- `add(key, description)`: Add shortcut.
- `spacing`: Gap between items.
- Keys displayed in inverse colors.

#### `StatusBar`
Multi-section footer bar.
- `add_section(content, width, alignment)`: Add section.
- `separator`: Section separator string.

## Helper Classes

### `Color`
Represents an RGB color.
- `Color(r, g, b)`
- Predefined: `Color::White()`, `Color::Red()`, etc.
- **HSV Support**:
  - `Color::hsv_to_rgb(h, s, v)`: Create color from Hue/Saturation/Value (0.0-1.0).
  - `Color::rgb_to_hsv(color, h, s, v)`: Convert RGB to HSV.

### UTF-8 and Wide Character Utilities

cpp-tui includes built-in support for Unicode and wide characters (CJK, emoji, etc.):

#### `utf8_decode_codepoint(string, pos, out_codepoint, out_len)`
Decodes a UTF-8 character from a string at the given position.
- Returns the Unicode codepoint and byte length consumed.

#### `char_display_width(codepoint)`
Returns the terminal display width of a Unicode codepoint:
- `0` for combining marks and zero-width characters
- `1` for narrow characters (ASCII, Latin, Greek, etc.)
- `2` for wide characters (CJK ideographs, Hangul, Hiragana, Katakana, emoji)

#### `utf8_display_width(string)`
Calculates the total display width of a UTF-8 string in terminal cells.

```cpp
// Example usage
std::string text = "Hello中文🎉";
int width = cpptui::utf8_display_width(text); // Returns 12 (5 + 4 + 2)
```

## Troubleshooting

### Wide Characters Display Issues
cpp-tui has built-in support for wide characters including CJK (Chinese, Japanese, Korean) and emoji. The library correctly calculates display widths for:
- ASCII and Latin characters (1 cell wide)
- CJK Unified Ideographs, Hiragana, Katakana, Hangul (2 cells wide)
- Common emoji (2 cells wide)
- Combining diacritical marks (0 cells wide)

If you are experiencing display issues with wide characters, ensure:
1. Your terminal uses a monospace font with good Unicode support (e.g., **Consolas**, **DejaVu Sans Mono**, **JetBrains Mono**, **Noto Sans Mono CJK**)
2. Your terminal correctly renders wide characters as double-width

### Mouse Input Not Working
If mouse input is not being recognized by the application, check your terminal's mouse settings to ensure that mouse input is being passed through to applications. Some terminals have options like "Mouse Tracking" or "Send mouse events to application" that need to be enabled.

