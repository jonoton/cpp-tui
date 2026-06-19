---
layout: default
title: API Reference
nav_order: 4
---

## API Reference

<details class="api-toc" open>
<summary>📑 Table of Contents</summary>
<ul>
<li><a href="#app">App</a></li>
<li><a href="#base-widget-properties">Base Widget Properties</a></li>
<li class="toc-category">Layouts</li>
<li><a href="#vertical--horizontal">Vertical / Horizontal</a></li>
<li><a href="#verticalspacer--horizontalspacer">VerticalSpacer / HorizontalSpacer</a></li>
<li><a href="#align">Align</a></li>
<li><a href="#grid">Grid</a></li>
<li><a href="#splitpane">SplitPane</a></li>
<li><a href="#stack">Stack</a></li>
<li><a href="#scrollablevertical">ScrollableVertical</a></li>
<li><a href="#scrollablehorizontal">ScrollableHorizontal</a></li>
<li><a href="#scrollablecontainer">ScrollableContainer</a></li>

<li class="toc-category">Containers</li>
<li><a href="#border">Border</a></li>
<li><a href="#tabs">Tabs</a></li>
<li><a href="#carousel">Carousel</a></li>
<li><a href="#accordion">Accordion</a></li>
<li><a href="#dialog">Dialog</a></li>

<li class="toc-category">Basic Widgets</li>
<li><a href="#static">Static</a></li>
<li><a href="#label">Label</a></li>
<li><a href="#paragraph">Paragraph</a></li>
<li><a href="#textspan--styledtext">TextSpan / StyledText</a></li>
<li><a href="#button">Button</a></li>
<li><a href="#badge">Badge</a></li>
<li><a href="#checkbox">Checkbox</a></li>
<li><a href="#progressbar">ProgressBar</a></li>
<li><a href="#sparkline">Sparkline</a></li>
<li><a href="#spinner">Spinner</a></li>
<li><a href="#tooltip">Tooltip</a></li>
<li><a href="#notification">Notification</a></li>

<li class="toc-category">Advanced Inputs</li>
<li><a href="#input">Input</a></li>
<li><a href="#textarea">TextArea</a></li>
<li><a href="#searchinput">SearchInput</a></li>
<li><a href="#toggleswitch">ToggleSwitch</a></li>
<li><a href="#numberinput">NumberInput</a></li>
<li><a href="#slider">Slider</a></li>
<li><a href="#radioset">RadioSet</a></li>
<li><a href="#checkboxlist">CheckboxList</a></li>
<li><a href="#colorpicker">ColorPicker</a></li>
<li><a href="#dropdown">Dropdown</a></li>
<li><a href="#calendar">Calendar</a></li>

<li class="toc-category">Lists &amp; Tables</li>
<li><a href="#tablepaginated">TablePaginated</a></li>
<li><a href="#tablescrollable">TableScrollable</a></li>
<li><a href="#treeview">TreeView</a></li>
<li><a href="#textlist">TextList</a></li>

<li class="toc-category">Charts</li>
<li><a href="#linechart--scatterchart">LineChart / ScatterChart</a></li>
<li><a href="#barchart">BarChart</a></li>
<li><a href="#gauge">Gauge</a></li>
<li><a href="#proportionalbar">ProportionalBar</a></li>
<li><a href="#heatmap">Heatmap</a></li>

<li class="toc-category">Navigation &amp; Utility</li>
<li><a href="#menudialog">MenuDialog</a></li>
<li><a href="#menubar">MenuBar</a></li>
<li><a href="#breadcrumb">Breadcrumb</a></li>
<li><a href="#shortcutbar">ShortcutBar</a></li>
<li><a href="#statusbar">StatusBar</a></li>
<li><a href="#fileexplorer">FileExplorer</a></li>

<li class="toc-category">Helper Classes</li>
<li><a href="#color">Color</a></li>
<li><a href="#utf-8-and-wide-character-utilities">UTF-8 Utilities</a></li>
</ul>
</details>

---

### App
{: .api-section-header}

The `App` class is the main application controller. It initializes the terminal, manages the event loop, and provides methods for timers, dialogs, and cross-thread communication.

```cpp
App app;
auto root = std::make_shared<Vertical>();
// ... build UI ...
app.run(root);
```

| Method | Description |
|--------|-------------|
| `run(root)` | Start the event loop with the given root widget |
| `quit()` | Exit the event loop and restore the terminal (static) |
| `register_exit_key(key, ctrl, alt, shift)` | Register a key that will exit the application |

#### Timers

| Method | Returns | Description |
|--------|---------|-------------|
| `add_timer(interval_ms, callback)` | `TimerId` | Schedule a repeating callback at the given interval |
| `remove_timer(timer_id)` | `void` | Stop and remove a timer |

#### Threading

Thread-safe methods for updating the UI from background threads:

| Method | Description |
|--------|-------------|
| `update()` | Wake the event loop and trigger a redraw. Can be called from any thread. |
| `post(callback)` | Schedule a callback to run on the UI thread, then trigger a redraw. Use this to safely modify widgets from background threads. |

**Using `post()` (recommended):**

```cpp
// From a background thread — callback runs on the UI thread
app.post([&]() {
    label->set_text("Updated safely!");
    progress->value = 0.75f;
});
```

**Using `update()`:**

```cpp
// For advanced use when you manage your own synchronization
label->set_text("New value");  // Caller ensures thread safety
app.update();                  // Wakes the event loop to redraw
```

#### Dialogs

| Method | Description |
|--------|-------------|
| `open_dialog(dialog)` | Open a dialog, auto-centered on screen |
| `open_dialog(dialog, x, y)` | Open a dialog at specific screen coordinates |
| `close_dialog(dialog)` | Close a specific dialog |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Base Widget Properties
{: .api-section-header}

All widgets inherit from the base `Widget` class and share these common properties:

| Property | Type | Description |
|----------|------|-------------|
| `x` / `y` | `int` | Position relative to parent container |
| `width` / `height` | `int` | Current dimensions (read-only for most widgets) |
| `fixed_width` | `int` | `0` = flexible, `> 0` = fixed width request |
| `fixed_height` | `int` | `0` = flexible, `> 0` = fixed height request |
| `min_width` | `int` | `0` = no minimum, `> 0` = minimum width constraint |
| `min_height` | `int` | `0` = no minimum, `> 0` = minimum height constraint |
| `max_width` | `int` | `0` = no maximum, `> 0` = maximum width constraint |
| `max_height` | `int` | `0` = no maximum, `> 0` = maximum height constraint |
| `fg_color` | `Color` | Foreground color override |
| `bg_color` | `Color` | Background color override |
| `focusable` | `bool` | Whether the widget can receive keyboard/mouse focus |
| `tab_stop` | `bool` | Whether the widget is included in `Tab` navigation |
| `set_responsive` | method | Toggle visibility based on SM/MD/LG screen sizes |
| `set_responsive_width` | method | Toggle visibility based on screen width |
| `set_responsive_height` | method | Toggle visibility based on screen height |
| `on_hover` | callback | Triggered when mouse enters/exits (`void(bool)`) |

---

### Layouts
{: .api-section-header}

Layouts are container widgets that arrange their children.

---

#### `Vertical` / `Horizontal`

Arranges children in a vertical column or horizontal row.

```cpp
auto col = std::make_shared<Vertical>();
col->add(std::make_shared<Label>("Row 1"));
col->add(std::make_shared<Label>("Row 2"));

auto row = std::make_shared<Horizontal>();
row->add(std::make_shared<Label>("Col A"));
row->add(std::make_shared<Label>("Col B"));
```

| Property | Description |
|----------|-------------|
| `fixed_width` / `fixed_height` | Set to `> 0` for fixed size; `<= 0` for flexible. Flexible children automatically receive equal shares of the remaining space. |
| `min_width` / `max_width` / `min_height` / `max_height` | Constraints applied during flexible space distribution (multi-pass clamping) and cross-axis alignment. |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `VerticalSpacer` / `HorizontalSpacer`

Invisible widgets used to add spacing between other widgets.

| Constructor | Description |
|-------------|-------------|
| `VerticalSpacer(int height)` | Fixed vertical space (flexible if `height <= 0`) |
| `HorizontalSpacer(int width)` | Fixed horizontal space (flexible if `width <= 0`) |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Align`

Aligns a single child within its available space.

| Enum | Values |
|------|--------|
| `Align::H` | `Left`, `Center`, `Right` |
| `Align::V` | `Top`, `Center`, `Bottom` |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Grid`

Arranges children in a table-like grid.

```cpp
auto grid = std::make_shared<Grid>();
grid->add_item(widget, /*row=*/0, /*col=*/0, /*row_span=*/1, /*col_span=*/2);
```

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `SplitPane`

Resizable split panel container with mouse-drag support.

```cpp
auto split = std::make_shared<SplitPane>();
split->set_panes(left_panel, right_panel);
split->ratio = 0.3;     // 30% / 70% split
split->vertical = false; // horizontal split
```

| Property | Type | Description |
|----------|------|-------------|
| `ratio` | `float` | Divider position (0–1) |
| `vertical` | `bool` | `true` = vertical split, `false` = horizontal |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Stack`

Stacks children on top of each other (z-order). Useful for overlays or background layers.

All visible children are resized to fill the stack (unless fixed size is set).

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `ScrollableVertical`

A vertical layout that scrolls if content exceeds its height. Content is clipped to the container's bounds.

- **Sizing**: Children without an explicit `fixed_height` are treated as flexible and will share the visible height of the container equally (instead of defaulting to a height of 1).

---

#### `ScrollableHorizontal`

A horizontal layout that scrolls if content exceeds its width. Supports a horizontal scrollbar.

- **Sizing**: Children without an explicit `fixed_width` are treated as flexible and will share the visible width of the container equally.

---

#### `ScrollableContainer`

Bi-directional scrollable container for panning over large 2D content.

| Property | Type | Description |
|----------|------|-------------|
| `scroll_x` | `int` | Horizontal scroll offset |
| `scroll_y` | `int` | Vertical scroll offset |

**Keyboard:** Arrow keys, PageUp/PageDown (vertical). Ctrl+Arrow (horizontal).

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Containers
{: .api-section-header}

All container widgets inherit from the base `Container` class, which provides children management and size propagation features.

| Property / Method | Type | Description |
|-------------------|------|-------------|
| `add(widget)` | method | Add a child widget to the container |
| `clear_children()` | method | Remove all child widgets |
| `get_children()` | method | Returns a list of shared pointers to the children |
| `auto_shrink` | `bool` | If `true`, the container automatically calculates and shrinks its requested `fixed_width`/`fixed_height` to wrap its children's constraints plus padding. Defaults to `false` (fills parent space). |

---

#### `Border`

Wraps a child widget with a border and optional title.

```cpp
auto border = std::make_shared<Border>(BorderStyle::Rounded);
border->set_title("Settings", Alignment::Center);
border->add(content);
```

| Property | Values |
|----------|--------|
| `BorderStyle` | `Single`, `Double`, `Rounded` |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Tabs`

Displays content in switchable tabs with keyboard navigation.

```cpp
auto tabs = std::make_shared<Tabs>();
tabs->add_tab("General", general_panel);
tabs->add_tab("Advanced", advanced_panel);
tabs->on_change = [](int index) { /* tab changed */ };
```

| Feature | Description |
|---------|-------------|
| Overflow | Navigation buttons appear when tabs exceed width |
| Focus | Tab buttons are focusable via keyboard (Enter/Space) |
| `on_change` | Callback fired when the active tab changes |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Carousel`

Shows one page (widget) at a time, with optional navigation arrows and dot indicators.

| Property | Type | Description |
|----------|------|-------------|
| `show_dots` | `bool` | Show page indicator dots |
| `show_arrows` | `bool` | Show left/right navigation arrows |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Accordion`

Collapsible content sections with keyboard navigation.

```cpp
auto acc = std::make_shared<Accordion>();
acc->add_section("Details", details_widget, /*expanded=*/true);
acc->allow_multiple = false; // only one section open at a time
```

**Keyboard:** Arrow keys + Enter/Space to navigate and toggle.

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Dialog`

A general-purpose popup dialog with border and shadow.

```cpp
auto dialog = std::make_shared<Dialog>(content);
dialog->modal = true;
dialog->shadow = true;
app.open_dialog(dialog);       // auto-centered
app.open_dialog(dialog, 5, 3); // at specific coordinates
```

| Property | Type | Description |
|----------|------|-------------|
| `modal` | `bool` | Block input to widgets behind the dialog |
| `shadow` | `bool` | Render a drop shadow effect |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Basic Widgets
{: .api-section-header}

---

#### `Static`

Displays simple static text. Minimal overhead — useful when you only need basic text rendering.

---

#### `Label`

Displays read-only text with color support.

| Property | Type | Description |
|----------|------|-------------|
| `text` | `string` | The text content (see `set_text` / `get_text`) |
| `selectable` | `bool` | If `true`, text can be selected and copied with `Ctrl+C` |
| `underline` | `bool` | If `true`, text will be rendered with an underline |

---

#### `Paragraph`

Multi-line text widget with word wrapping and indentation.

```cpp
auto para = std::make_shared<Paragraph>();
para->text = "A long paragraph of text that will word-wrap...";
para->first_line_indent = 4;
para->word_wrap = true;
para->bold = true;
```

| Property | Type | Description |
|----------|------|-------------|
| `text` | `string` | Plain text content |
| `styled_content` | `StyledText` | Mixed formatting (see StyledText below) |
| `first_line_indent` | `int` | Indentation for the first line |
| `hanging_indent` | `int` | Indentation for subsequent lines |
| `word_wrap` | `bool` | Enable/disable word wrapping |
| `preserve_whitespace` | `bool` | Preserve exact spacing during wrapping (default `false`) |
| `bold` / `italic` / `underline` | `bool` | Global style toggles |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `TextSpan` / `StyledText`

`TextSpan` is a structure representing a single segment of styled text. `StyledText` is a builder class used to collect multiple `TextSpan` objects into a single formatted block.

```cpp
StyledText styled;
styled.bold("Bold text ")
      .italic("italic text ")
      .colored("colored", Color::Red());

auto para = std::make_shared<Paragraph>(styled);
```

| Method | Description |
|--------|-------------|
| `bold(text)` | Append bold text |
| `italic(text)` | Append italic text |
| `underline(text)` | Append underlined text |
| `colored(text, color)` | Append colored text |
| `colored_bold(text, color)` | Append bold + colored text |
| `add(text, ...)` | Base method for custom spans |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Button`

Clickable button with a callback.

```cpp
auto btn = std::make_shared<Button>("Save", []() {
    // handle click
});
```

| Property | Type | Description |
|----------|------|-------------|
| `alignment` | `Alignment` | Text alignment within the button |

| Method | Description |
|--------|-------------|
| `get_label()` | Returns the button text (plain) |
| `set_label(StyledText)` | Updates the button label (supports mixed formatting) |
| `set_on_click(callback)` | Sets the click callback |

---

#### `Badge`

Compact inline status indicator.

| Property | Type | Description |
|----------|------|-------------|
| `styled_text` | `StyledText` | Badge label with formatting support |
| `badge_bg` | `Color` | Background color |
| `text_color` | `Color` | Text color |
| `style` | enum | `Pill`, `Square`, or `Outline` |

---

#### `Checkbox`

Toggleable boolean state.

| Method | Description |
|--------|-------------|
| `is_checked()` | Returns current checked state |

---

#### `ProgressBar`

Visualizes a float value from `0.0` to `1.0`.

```cpp
auto bar = std::make_shared<ProgressBar>();
bar->value = 0.75;
bar->show_text = true;
bar->color = Color::Green();
bar->char_filled = "█";
bar->char_empty = "░";
```

| Property | Type | Description |
|----------|------|-------------|
| `value` | `float` | Progress value (0.0–1.0) |
| `max_height` | `int` | Limit vertical size |
| `show_text` | `bool` | Show progress percentage or custom text |
| `text_formatter` | `std::function<string(float)>` | Custom progress text |
| `text_color` | `Color` | Progress text color |
| `char_filled` / `char_empty` | `string` | Custom bar characters |
| `color` | `Color` | Main bar color (filled part) |
| `empty_color` | `Color` | Empty bar color |
| `text_alignment` | enum | `Alignment::Left`, `Center`, `Right` |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Sparkline`

Displays a simple line graph of a data series. Supports generic widget tooltips (`set_tooltip`).

---

#### `Spinner`

Animated activity indicator with multiple styles.

```cpp
auto spinner = std::make_shared<Spinner>(&app);
spinner->set_style(Spinner::StyleBrailleSpin());
spinner->value = -1; // indeterminate (spinning)
spinner->color = Color::Cyan();
```

**Indeterminate styles** (spins continuously):
`StyleBrailleSpin()`, `StyleLineSpin()`, `StylePieSpin()`, `StyleBounceBar()`, `StylePulseAscii()`

**Determinate styles** (maps value 0–1 to frames):
`StylePieProgress()`, `StyleBarProgress()`

| Property | Type | Description |
|----------|------|-------------|
| `value` | `float` | `>= 0` for determinate, `< 0` for spinning |
| `color` | `Color` | Spinner character color |

> **Note:** Requires an `App*` pointer for timer-based animation.

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Tooltip`

Hover-triggered popup with text selection support.

| Property | Type | Description |
|----------|------|-------------|
| `text` | `string` | Tooltip content |
| `position` | enum | `Top`, `Bottom`, `Left`, `Right`, or `Manual` |

| Method | Description |
|--------|-------------|
| `attach(widget)` | Attach to a target widget |
| `show()` / `hide()` | Manual visibility control |

**Interactions:**
- Tooltip stays visible when mouse moves into it
- Click-drag to select text, double-click for a word, triple-click for all
- `Ctrl+C` to copy selected text

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Notification`

Toast-style auto-dismissing messages with text selection support.

```cpp
auto notif = std::make_shared<Notification>(&app);
notif->position = Notification::TopRight;
notif->show("File saved!", Notification::Success, 3000);
```

| Property | Type | Description |
|----------|------|-------------|
| `position` | enum | `TopRight` (default), `TopLeft`, `BottomRight`, `BottomLeft`, `TopCenter`, `BottomCenter` |
| `max_visible` | `int` | Maximum concurrent notifications |
| `Type` | enum | `Info`, `Success`, `Warning`, `Error` |

**Interactions:**
- Notifications pause auto-dismiss when hovered
- Click-drag to select text, double-click for a word, triple-click for all
- `Ctrl+C` to copy selected text

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Advanced Inputs
{: .api-section-header}

---

#### `Input`

Single-line text input field. Focusable.

```cpp
auto input = std::make_shared<Input>();
input->placeholder = "Enter your name...";
input->regex_pattern = "^[A-Za-z ]+$";
input->is_password = true;
input->on_change = [](std::string val) { /* value changed */ };
```

| Property | Type | Description |
|----------|------|-------------|
| `placeholder` | `string` | Text shown when empty |
| `regex_pattern` | `string` | Validation regex |
| `empty_char` | `string` | Character filler (defaults to space) |
| `is_password` | `bool` | Obscure input text |
| `password_char` | `string` | Character used for masking (default `*`) |
| `accepts_tab` | `bool` | If `true`, Tab inserts spaces instead of moving focus |
| `tab_size` | `int` | Number of spaces for Tab key |

| Color Property | Description |
|----------------|-------------|
| `fg_color` / `bg_color` | Base colors |
| `focused_fg_color` / `focused_bg_color` | Colors when focused |
| `placeholder_color` | Placeholder text color |
| `cursor_color` | Cursor character color |
| `error_fg_color` / `error_bg_color` | Colors when invalid |

| Method | Description |
|--------|-------------|
| `get_text()` | Returns current text |
| `set_text(string)` | Sets the text |

**Features:** Auto-scrolls horizontally · Copy/Cut/Paste · Undo/Redo

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `TextArea`

Multi-line text editor with full scrolling and cursor support.

```cpp
auto editor = std::make_shared<TextArea>();
editor->show_line_numbers = true;
editor->word_wrap = true;
editor->set_text("Hello, world!");
editor->on_change = [](std::string text) { /* ... */ };
editor->on_cursor_move = [](int lx, int ly) { /* ... */ };
```

| Property | Type | Description |
|----------|------|-------------|
| `show_line_numbers` | `bool` | Toggle line number gutter |
| `word_wrap` | `bool` | Enable soft wrapping |
| `show_scrollbar` | `bool` | Toggle scrollbars |
| `accepts_tab` | `bool` | If `true`, Tab inserts spaces instead of moving focus |
| `tab_size` | `int` | Number of spaces for Tab key |

| Color Property | Description |
|----------------|-------------|
| `line_number_fg` / `line_number_bg` | Gutter colors |
| `cursor_color` | Cursor character color |
| `selection_bg` | Background color for selected text |

| Method | Description |
|--------|-------------|
| `set_text(string)` | Set content |
| `get_text()` | Get content |

**Interactions:** Click to move cursor · Mouse wheel to scroll (Ctrl+Wheel for horizontal) · Scrollbar dragging · Copy/Cut/Paste · Unlimited Undo/Redo · Triple-click to select all

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `SearchInput`

Input field with autocomplete dropdown.

| Property | Type | Description |
|----------|------|-------------|
| `suggestions` | `vector<StyledText>` | Formatted suggestion strings |
| `placeholder` | `StyledText` | Formatted placeholder text |
| `suggestion_limit` | `int` | Max suggestions shown (default 5) |
| `on_search` | callback | Fired on search submit (`void(string)`) |
| `on_change` | callback | Fired on text change (`void(string)`) |

**Keyboard:** Arrow keys to navigate suggestions · Enter to confirm · `Ctrl+Space` to open the suggestions list manually

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `ToggleSwitch`

A modern alternative to checkboxes.

| Property | Type | Description |
|----------|------|-------------|
| `active_color` | `Color` | Color when on |
| `inactive_color` | `Color` | Color when off |
| `on_label` / `off_label` | `string` | Custom text (default `"[ON]"` / `"[OFF]"`) |

---

#### `NumberInput`

Integer input with stepper buttons.

| Property | Type | Description |
|----------|------|-------------|
| `min_value` / `max_value` | `int` | Value range |
| `step` | `int` | Increment step |

| Method | Description |
|--------|-------------|
| `set_position(ButtonPos)` | `Left`, `Right`, or `Split` |

---

#### `Slider`

Horizontal or vertical range slider for numeric values.

```cpp
auto slider = std::make_shared<Slider>();
slider->min_value = 0;
slider->max_value = 100;
slider->value = 50;
slider->step = 5;
slider->on_change = [](double val) { /* value changed */ };
```

| Property | Type | Description |
|----------|------|-------------|
| `min_value` / `max_value` | `double` | Range |
| `value` | `double` | Current value |
| `step` | `double` | Step increment |
| `vertical` | `bool` | Vertical orientation |
| `on_change` | callback | Value change callback (`double`) |

| Styling Property | Description |
|------------------|-------------|
| `track_color` | Background track color |
| `thumb_color` | Slider handle color |
| `fill_color` | Filled segment color |
| `thumb_char` | Handle character (default `●`) |
| `track_char` / `track_char_v` | Track characters |
| `fill_char` / `fill_char_v` | Fill characters |

**Interaction:** Mouse drag and arrow keys.

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `RadioSet`

Mutually exclusive selection from a list of strings.

| Property | Type | Description |
|----------|------|-------------|
| `selected_color` | `Color` | Color for the selected item |

| Method | Description |
|--------|-------------|
| `get_selected_index()` | Returns selected index |
| `set_style(selected, unselected)` | Custom prefix markers |

---

#### `CheckboxList`

Multiple selection from a list.

| Property | Type | Description |
|----------|------|-------------|
| `checked_prefix` / `unchecked_prefix` | `string` | Custom markers |
| `checked_color` | `Color` | Color for checked items |
| `cursor_color` | `Color` | Color for focused item |

| Method | Description |
|--------|-------------|
| `get_checked_indices()` | Returns vector of checked indices |

---

#### `ColorPicker`

HSV/RGB color selection widget.

| Property | Type | Description |
|----------|------|-------------|
| `show_values` | `bool` | Show editable Hex/RGB inputs |
| `on_change` | callback | Real-time color change callback |

| Method | Description |
|--------|-------------|
| `get_color()` / `set_color(Color)` | Get/set selected color |
| `get_hex()` / `set_hex(string)` | Hex string support |

**Interactions:** Click/drag the gradient for Hue/Saturation · Click/drag the slider for Brightness · Edit Hex/RGB inputs directly

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Dropdown`

Collapsible selection list.

| Property | Type | Description |
|----------|------|-------------|
| `options` | `vector<StyledText>` | Formatted options |
| `selected_index` | `int` | Current selection index |
| `placeholder` | `StyledText` | Formatted placeholder |
| `on_change` | callback | Selection change callback (`void(int, string)`) |
| `hover_bg` / `hover_fg` | `Color` | Colors when hovered |

| Method | Description |
|--------|-------------|
| `set_options(vector)`| Sets options and resets selection if needed |
| `toggle()` | Manually open/close the dropdown |

---

#### `Calendar`

Month-view calendar widget with button navigation.

| Property | Type | Description |
|----------|------|-------------|
| `year` / `month` | `int` | Current month |
| `selected_day` | `int` | Selected date |
| `highlighted_days` | `vector<int>` | Highlighted dates |
| `on_select` | callback | Date selection callback (`year, month, day`) |

| Styling Property | Description |
|------------------|-------------|
| `header_color` | Color for the month/year header |
| `day_color` | Default color for day numbers |
| `selected_color` | Color for the selected day |
| `highlight_color` | Color for highlighted days |
| `show_border` | Toggle container border |
| `border_color` | Color for the border |

**Keyboard:** Tab to `<`/`>` buttons or day grid, arrow keys to navigate days.

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Lists & Tables
{: .api-section-header}

---

#### `TablePaginated`

Tabular data with automatic pagination controls in the footer.

```cpp
auto table = std::make_shared<TablePaginated>();
table->columns = {"Name", "Age", "City"};
table->rows = {
    {"Alice", "30", "NYC"},
    {"Bob", "25", "LA"}
};
table->auto_page_size = true; // fit rows to height
```

| Property | Type | Description |
|----------|------|-------------|
| `columns` | `vector<string>` | Column headers |
| `rows` | `vector<vector<StyledText>>` | Row data (supports formatting per cell) |
| `page_size` | `int` | Number of rows per page |
| `auto_page_size` | `bool` | Auto-fit rows to container height |
| `current_page` | `int` | Current page index (0-based) |
| `header_bg_color` / `header_fg_color` | `Color` | Header colors |
| `selected_bg_color` / `selected_fg_color` | `Color` | Selection colors |
| `alternate_colors` | `bool` | Toggle zebra-striping |
| `row_color_a` / `row_color_b` | `Color` | Custom zebra-stripe colors |
| `border_color` | `Color` | Table inner border color |
| `selection_indicator` | `string` | Custom selection prefix (default `"> "`) |

| Method | Description |
|--------|-------------|
| `next_page()` | Navigate to the next page |
| `prev_page()` | Navigate to the previous page |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `TableScrollable`

Efficient scrollable list for large datasets (e.g., process lists). Supports row selection.

| Property | Type | Description |
|----------|------|-------------|
| `columns` | `vector<string>` | Column headers |
| `rows` | `vector<vector<StyledText>>` | Row data (supports formatting per cell) |
| `col_widths` | `vector<int>` | Optional explicit column widths |
| `selected_index` | `int` | Currently selected row |
| `scroll_offset` | `int` | First visible row index |
| `header_color` | `Color` | Header background color |
| `selected_bg_color` / `selected_fg_color` | `Color` | Selection colors |
| `on_change` | callback | Fired on selection change (`void(int)`) |
| `row_fg_color` | `Color` | Default text color for data rows |

| Scrollbar Property | Type | Description |
|--------------------|------|-------------|
| `scrollbar_track_color` | `Color` | Color of the scrollbar track |
| `scrollbar_thumb_color` | `Color` | Color of the scrollbar handle |
| `scrollbar_track_char` | `string` | Track character (default `░`) |
| `scrollbar_thumb_char` | `string` | Handle character (default `█`) |

**Interactions:** Mouse click or Up/Down arrows to select · Mouse wheel or PageUp/PageDown to scroll · Interactive scrollbar dragging

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `TreeView`

Hierarchical data display with keyboard navigation and selection.

```cpp
TreeNode root;
root.label = "Documents";
root.icon = "📁";
root.children = {
    {"README.md", false, {}, false, "📄", {}},
    {"src", true, { /* nested children */ }, false, "📁", {}}
};

auto tree = std::make_shared<TreeView>();
tree->root_nodes = {root};
tree->on_submit = [](const TreeNode& node) { /* double-click or Enter */ };
```

| Property | Type | Description |
|----------|------|-------------|
| `root_nodes` | `vector<TreeNode>` | Top-level nodes |
| `indent_unit` | `string` | Indentation string (default: two spaces) |
| `icon_expanded` / `icon_collapsed` / `icon_leaf` | `string` | Custom icons |
| `selected_bg` / `selected_fg` | `Color` | Selection colors |

| TreeNode Property | Type | Description |
|-------------------|------|-------------|
| `label` | `string` | Display text |
| `children` | `vector<TreeNode>` | Nested children |
| `expanded` | `bool` | Current expansion state |
| `icon` | `string` | Per-node custom icon |
| `color` | `Color` | Per-node custom color |

| Callback | Description |
|----------|-------------|
| `on_select` | Selection changes |
| `on_submit` | Enter/Space or double-click |
| `on_expand` / `on_collapse` | Node toggled |

**Keyboard:** Up/Down arrows · Left (collapse/go to parent) · Right (expand) · Enter/Space (toggle/submit)

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `TextList`

Displays a list of formatted strings with optional bullets or numbering.

```cpp
auto list = std::make_shared<TextList>();
list->style = ListStyle::Bullet;
list->add_item("First item", 0);
list->add_item("Sub-item", 1);
list->selectable = true;
```

| Property | Type | Description |
|----------|------|-------------|
| `items` | `vector<ListItem>` | List items |
| `style` | enum | `ListStyle::Bullet` or `ListStyle::Numbered` |
| `bullet_markers` | `vector<string>` | Custom markers per level (e.g. `{"- ", "o ", "> "}`) |
| `selectable` | `bool` | Enable text selection |

**Features:** Click-drag to select text · Double-click for word · Triple-click for all · `Ctrl+C` to copy · Automatic word wrapping

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Charts
{: .api-section-header}

---

#### `LineChart` / `ScatterChart`

Plots numerical (x, y) data with interactive data-point tooltips.

```cpp
auto chart = std::make_shared<LineChart>();
chart->add_series(data, "Temperature", Color::Red(), LineStyle::Braille);
chart->show_legend = true;
chart->show_tooltip = true;
```

| Property | Type | Description |
|----------|------|-------------|
| `series` | `vector<Series>` | List of data series (see below) |
| `show_legend` | `bool` | Show series legend |
| `min_val` / `max_val` | `float` | Axis range |
| `x_tick_count` / `y_tick_count` | `int` | Number of tick marks |
| `x_tick_precision` / `y_tick_precision` | `int` | Decimal places for labels |
| `label_all_x_ticks` / `label_all_y_ticks` | `bool` | Force label on every tick |
| `x_tick_formatter` / `y_tick_formatter` | `function` | Custom label formatters |
| `show_tooltip` | `bool` | Interactive tooltips on hover |
| `tooltip_duration_ms` | `int` | Linger duration (default 1000ms) |
| `tooltip_formatter` | `function` | Custom tooltip text |

| Series Property | Type | Description |
|-----------------|------|-------------|
| `data` | `vector<double>` | Data points |
| `label` | `string` | Legend label |
| `color` | `Color` | Series color |
| `marker` | `string` | Marker character (for Points style, default `*`) |
| `style` | `LineStyle` | `Points`, `Lines`, or `Braille` |
| `fill_gaps` | `bool` | Interpolate missing values (default `true`) |

**Styles:** `LineStyle::Points`, `LineStyle::Lines`, `LineStyle::Braille`

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `BarChart`

Vertical bar chart for categorical data.

| Property | Type | Description |
|----------|------|-------------|
| `show_x_axis` / `show_y_axis` | `bool` | Axis visibility |
| `show_x_ticks` / `show_y_ticks` | `bool` | Tick visibility |
| `show_tooltip` | `bool` | Interactive tooltips on hover |
| `tooltip_formatter` | `function` | Custom tooltip text (`void(int, double)`) |

Tick customization options are the same as LineChart.

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `Gauge`

Semi-circular progress meter.

| Property | Type | Description |
|----------|------|-------------|
| `value` | `float` | 0.0 to 1.0 |
| `min_label` / `max_label` | `string` | Endpoint labels |
| `show_value` | `bool` | Show center value |
| `value_format` | `string` | Value display format (e.g. `"%d%%"`) |
| `arc_color` | `Color` | Color of the arc border |
| `fill_color` | `Color` | Color of the progress fill |
| `text_color` | `Color` | Color of the labels |

---

#### `ProportionalBar`

Horizontal bar visualization for proportional data.

```cpp
auto bar = std::make_shared<ProportionalBar>();
bar->add_segment(0.6, "Used", Color::Red());
bar->add_segment(0.4, "Free", Color::Green());
bar->show_legend = true;
bar->show_percentages = true;
```

| Property | Type | Description |
|----------|------|-------------|
| `show_legend` | `bool` | Show segment legend |
| `show_percentages` | `bool` | Show percentage labels in legend |

---

#### `Heatmap`

2D color intensity grid.

| Property | Type | Description |
|----------|------|-------------|
| `data` | `vector<vector<float>>` | 2D values (0–1) |
| `row_labels` / `col_labels` | `vector<string>` | Axis labels |
| `low_color` / `high_color` | `Color` | Gradient endpoints |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Navigation & Utility
{: .api-section-header}

---

#### `MenuDialog`

Popup menu widget with keyboard and mouse support.

```cpp
auto menu = std::make_shared<MenuDialog>();
menu->items = {
    {"Open", []() { /* ... */ }},
    {"Save", []() { /* ... */ }, {
        {"Save As...", []() { /* ... */ }}  // sub-item
    }}
};
menu->show(10, 5);
```

| Property | Type | Description |
|----------|------|-------------|
| `items` | `vector<MenuItem>` | Menu items with `{label, action, sub_items}` |
| `mouse_hover_select` | `bool` | Highlight items on hover |

> **Note:** Clicking outside closes the menu and propagates the event (enabling fast menu switching).

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### `MenuBar`

Horizontal top-level menu bar. Integrates with `MenuDialog` for dropdown submenus.

| Property | Type | Description |
|----------|------|-------------|
| `items` | `vector<MenuItem>` | Top-level menu items |
| `bg_color` / `text_color` | `Color` | Base bar colors |
| `hover_bg` / `hover_fg` | `Color` | Colors when an item is hovered/selected |
| `selection_indicator` | `string` | Prefix for selected items (default `"> "`) |

---

#### `Breadcrumb`

Navigation trail widget.

```cpp
auto bc = std::make_shared<Breadcrumb>();
bc->add("Home", []() { /* navigate */ });
bc->add("Settings", []() { /* navigate */ });
bc->separator = " › ";
```

| Property | Type | Description |
|----------|------|-------------|
| `separator` | `string` | Custom separator (default `" > "`) |
| `separator_color` | `Color` | Color of the separator string |
| `item_color` | `Color` | Default color for breadcrumb items |
| `current_color` | `Color` | Color for the current (last) item |

---

#### `ShortcutBar`

Formatted keyboard shortcut display. Keys are rendered in inverse colors.

| Property | Type | Description |
|----------|------|-------------|
| `spacing` | `int` | Gap between items |
| `key_bg` / `key_fg` | `Color` | Background/Foreground for the key badge |
| `desc_fg` | `Color` | Foreground color for the description text |

| Method | Description |
|--------|-------------|
| `add(key, StyledText description)` | Add a shortcut display with formatting |

---

#### `StatusBar`

Multi-section footer bar.

```cpp
auto status = std::make_shared<StatusBar>();
status->add_section(StyledText().bold("STATUS").colored(" OK", Color::Green()));
status->add_section("Ln 1, Col 1");
```

| Property | Type | Description |
|----------|------|-------------|
| `separator` | `string` | Section separator string |

| Method | Description |
|--------|-------------|
| `add_section(StyledText content, int width = 0, Alignment alignment = Alignment::Left)` | Add a formatted section |

---

#### `FileExplorer`

A complete file system browser widget built on `TreeView`.

```cpp
auto explorer = std::make_shared<FileExplorer>("/home/user");
explorer->on_file_selected = [](const std::string& path) {
    // absolute path of the selected file
};
```

| Property | Type | Description |
|----------|------|-------------|
| `on_file_selected` | `function<void(string)>` | File selection callback |

**Features:** Expand/collapse directories · Folder and file icons (Nerd Fonts / Unicode) · Directories sorted first

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

### Helper Classes
{: .api-section-header}

---

#### `Color`

Represents an RGB color.

```cpp
Color custom(100, 200, 150);
Color red = Color::Red();
Color hsv = Color::hsv_to_rgb(0.6, 0.8, 1.0);
```

**Predefined colors:** `White()`, `Black()`, `Red()`, `Green()`, `Blue()`, `Yellow()`, `Cyan()`, `Magenta()`

| Method | Description |
|--------|-------------|
| `Color(r, g, b)` | Construct from RGB (0–255) |
| `Color::hsv_to_rgb(h, s, v)` | Create from HSV (0.0–1.0) |
| `Color::rgb_to_hsv(color, h, s, v)` | Convert RGB → HSV |

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>

---

#### UTF-8 and Wide Character Utilities

Built-in support for Unicode and wide characters (CJK, emoji, etc.). Most functions are part of the `TextHelper` utility class, though many have standalone aliases in the `cpptui` namespace for convenience.

| Function | Description |
|----------|-------------|
| `TextHelper::utf8_decode_codepoint(str, pos, out_cp, out_len)` | Decode a UTF-8 character at position |
| `TextHelper::char_display_width(codepoint)` | Terminal display width: `0` (combining), `1` (narrow), `2` (wide) |
| `TextHelper::utf8_display_width(str)` | Total display width of a UTF-8 string |
| `TextHelper::utf8_substr(str, start, count)` | Safe UTF-8 substring by character index |
| `TextHelper::count_codepoints(str)` | Count number of UTF-8 characters in a string |
| `TextHelper::utf8_char_byte_length(str, pos)` | Get byte length of UTF-8 character at position |
| `TextHelper::prepare_text_for_render(str)` | Pre-process text into `CharInfo` vector |
| `TextHelper::visual_to_char_pos(chars, x)`| Convert visual X to character index |
| `TextHelper::char_to_byte_pos(str, idx)` | Convert character index to byte offset |

```cpp
std::string text = "Hello中文🎉";
int width = cpptui::utf8_display_width(text); // Returns 12 (5 + 4 + 2)
```

<p class="back-to-top"><a href="#api-reference">↑ Back to top</a></p>
