---
layout: default
title: Home
nav_order: 1
toc: false
hero:
  eyebrow: Single header · C++17 · MIT licensed
  title: Beautiful terminal UIs in modern C++
  lede: cpp-tui is a lightweight, single-header library with 50+ widgets, a flex layout engine, built-in themes, and full mouse and keyboard support — include it and start building.
---

### Why cpp-tui

<div class="feature-grid">

<div class="feature">
  <strong>Single Header</strong>
  <p>Drop <code>cpptui.hpp</code> into your source directory. Zero dependencies, no extra build steps.</p>
</div>

<div class="feature">
  <strong>50+ Widgets</strong>
  <p>Form controls, editors, paginated tables, Braille charts, tree views, and modal dialogs.</p>
</div>

<div class="feature">
  <strong>Flex Layout Engine</strong>
  <p>Vertical, Horizontal, Grid, SplitPane, and auto-shrink sizing with multi-pass constraint clamping.</p>
</div>

<div class="feature">
  <strong>Dynamic Theming</strong>
  <p>Includes Catppuccin Mocha, Latte, Nord, Tokyo Night, and Solarized Light with runtime switching.</p>
</div>

<div class="feature">
  <strong>Complete Event Model</strong>
  <p>Mouse tracking, scroll wheel, focus cycling, global shortcuts with modifiers, and cross-thread updates.</p>
</div>

<div class="feature">
  <strong>Unicode &amp; CJK Precision</strong>
  <p>Built-in cell width calculation for multi-byte UTF-8, double-width CJK ideographs, and emoji.</p>
</div>

</div>

### Quick start

A complete program — a label, a button, and <kbd>q</kbd> to quit:

```cpp
#include "cpptui.hpp"
using namespace cpptui;

int main() {
    App app;
    Theme::set_theme(Theme::Dark());

    auto root = std::make_shared<Vertical>();
    root->add(std::make_shared<Label>("Hello, cpp-tui!"));
    root->add(std::make_shared<Button>("Click me", []() {
        // Handle button click
    }));

    app.register_exit_key('q');
    app.run(root);
    return 0;
}
```

### See it in action

Real output from two of the 18 bundled examples: a live system monitor and a spectrum analyzer.

<div class="shot-grid">
<figure class="shot"><a href="{{ '/assets/img/htop_sim.gif' | relative_url }}"><picture><source media="(prefers-reduced-motion: reduce)" srcset="{{ '/assets/img/htop_sim.png' | relative_url }}" width="2104" height="1416"><img src="{{ '/assets/img/htop_sim.gif' | relative_url }}" width="1112" height="708" alt="htop_sim: CPU meters, memory bars, CPU history and a process table" loading="eager"></picture></a><figcaption><code>examples/htop_sim.cpp</code></figcaption></figure>
<figure class="shot"><a href="{{ '/assets/img/spectrum_sim.gif' | relative_url }}"><picture><source media="(prefers-reduced-motion: reduce)" srcset="{{ '/assets/img/spectrum_sim.png' | relative_url }}" width="2104" height="1416"><img src="{{ '/assets/img/spectrum_sim.gif' | relative_url }}" width="1112" height="708" alt="spectrum_sim: real-time spectrum analyzer line chart with a hover tooltip" loading="eager"></picture></a><figcaption><code>examples/spectrum_sim.cpp</code></figcaption></figure>
</div>

[See all examples →]({{ '/getting-started' | relative_url }}#examples)

### Explore the docs

<div class="doc-cards">

<a class="doc-card" href="{{ '/getting-started' | relative_url }}">
<svg class="doc-card-icon" viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M5 12h14"/><path d="M13 6l6 6-6 6"/></svg>
<h3>Getting Started</h3>
<p>Install the single header, compile your first application, and explore CMake integration.</p>
</a>

<a class="doc-card" href="{{ '/core-concepts' | relative_url }}">
<svg class="doc-card-icon" viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M12 3l9 5-9 5-9-5 9-5z"/><path d="M3 13l9 5 9-5"/></svg>
<h3>Core Concepts</h3>
<p>Understand the App lifecycle, widget hierarchy, flex layout engine, themes, and threading.</p>
</a>

<a class="doc-card" href="{{ '/api-reference' | relative_url }}">
<svg class="doc-card-icon" viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M8 6l-5 6 5 6"/><path d="M16 6l5 6-5 6"/></svg>
<h3>API Reference</h3>
<p>Complete documentation for all 50+ widgets, layout containers, charts, and helper utilities.</p>
</a>

<a class="doc-card" href="{{ '/troubleshooting' | relative_url }}">
<svg class="doc-card-icon" viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><circle cx="12" cy="12" r="9"/><path d="M12 8v5"/><path d="M12 16.5v.01"/></svg>
<h3>Troubleshooting</h3>
<p>Diagnose common issues with wide characters, CJK fonts, mouse tracking, and ambiguous symbols.</p>
</a>

</div>
