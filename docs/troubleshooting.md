---
layout: default
title: Troubleshooting
nav_order: 5
---

## Troubleshooting

This guide provides solutions for common configuration challenges with terminal emulators, Unicode fonts, mouse tracking, and window resizing.

---

### Wide Characters and CJK Alignment

`cpp-tui` includes a dedicated UTF-8 character measurement engine (`TextHelper`) that computes terminal cell widths according to standard Unicode conventions:

| Category | Display Width | Examples |
|----------|---------------|----------|
| **ASCII & Latin** | 1 cell | `A`, `z`, `0`, `!`, `ñ` |
| **CJK Ideographs** | 2 cells | `中`, `文`, `あ`, `가` |
| **Common Emoji** | 2 cells | `🚀`, `🎉`, `📦` |
| **Combining Marks** | 0 cells | `◌́`, `◌̂` |

If text appears clipped, misaligned, or overlaps neighboring borders:

1. **Verify Terminal Font**: Use a high-quality monospace font with broad Unicode coverage, such as **JetBrains Mono**, **Fira Code**, **Cascadia Code**, or **Noto Sans Mono CJK**.
2. **Double-Width Font Rendering**: Ensure your terminal emulator does not treat CJK characters as single-width or force variable-width glyphs into 1-cell slots.

> In Windows Terminal and modern Linux terminals, ensure your terminal font profile includes a fallback font for CJK glyphs so character advance widths remain consistent.
{: .callout .callout-tip}

---

### Mouse Input Not Responding

`cpp-tui` automatically sends ANSI escape sequences to enable mouse button tracking, dragging, and wheel events.

If mouse interactions (clicks, text selection, slider adjustment) do not function:

- **Check Terminal Settings**: Ensure your terminal emulator has mouse reporting enabled. In some terminals, this is labeled *"Mouse Tracking"*, *"Send Mouse Events to Applications"*, or *"Report Mouse Clicks"*.
- **Holding Shift**: In many Linux and macOS terminals, holding <kbd>Shift</kbd> bypasses application mouse tracking and activates the native terminal selection clipboard. If you want `cpp-tui` to handle the click, release <kbd>Shift</kbd>.
- **tmux Users**: If running inside `tmux`, ensure mouse mode is active in your `~/.tmux.conf`:
  ```bash
  set -g mouse on
  ```

> When running inside nested SSH sessions or terminal multiplexers, verify that your outer terminal correctly forwards extended mouse coordinates (`SGR 1006` mode).
{: .callout .callout-note}

---

### Ambiguous-Width Unicode Characters

Certain Unicode symbols (such as `✓` U+2713, `⚠` U+26A0, `✗` U+2717, and various box/arrow markers) are designated as **Ambiguous Width** in Unicode Annex #11.

- **cpp-tui Default**: To guarantee predictable layout across the widest variety of platforms, `cpp-tui` treats ambiguous symbols as **Width 1 (Narrow)**.
- **Font Mismatch Artifacts**: If a specific terminal emulator or custom font forcibly renders these ambiguous symbols with double width, you may observe minor character overlap with the next cell.
- **Remedy**: Switch to a font that conforms to standard 1-cell glyph spacing for technical symbols (e.g., JetBrains Mono), or substitute ASCII/single-width Unicode alternatives.

---

### Windows Terminal Configuration

When compiling and running on Windows:

1. **Windows 10/11 Virtual Terminal**: `cpp-tui` enables `ENABLE_VIRTUAL_TERMINAL_PROCESSING` on Windows console handles so that standard ANSI sequences and 24-bit TrueColor work natively.
2. **UTF-8 Code Page**: `cpp-tui` switches the console input/output code pages to UTF-8 (`CP_UTF8`) at startup and restores the originals on exit, so no `chcp 65001` is required.
3. **MSVC Compilation Flag**: If building with MSVC, ensure source and execution character sets are set to UTF-8 using the `/utf-8` compiler flag (configured by default in `CMakeLists.txt`):
   ```cmd
   cl /std:c++17 /utf-8 /I. main.cpp
   ```

> On Windows, running `cpp-tui` applications within the modern **Windows Terminal** (rather than legacy `conhost.exe`) provides the best font rendering, mouse tracking, and truecolor fidelity.
{: .callout .callout-tip}

---

### Terminal Resizing & Layout Bounds

When a terminal window is resized, `cpp-tui` catches the `SIGWINCH` signal on POSIX systems or queries console screen buffer events on Windows, dynamically re-evaluating container flex constraints.

- **Minimum Size Constraints**: If a container has explicit `min_width` or `min_height` constraints that exceed the physical terminal dimensions, content may be clipped.
- **Overflow Panes**: Wrap complex dashboards in a `ScrollableVertical`, `ScrollableHorizontal`, or `ScrollableContainer` if the user might run the application in a small terminal window.

> If an application appears cramped on small screens, use `set_responsive_width()` to automatically collapse secondary panels or labels when the terminal falls below specified width thresholds.
{: .callout .callout-note}
