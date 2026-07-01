#include <memory>
#include <sstream>
#include <string>

#include "cpptui.hpp"

using namespace cpptui;

// =============================================================================
// Helper: build a simple info dialog and return it
// =============================================================================
static std::shared_ptr<Dialog> make_dialog(App *app, const std::string &title,
                                           const std::string &message) {
  auto dlg = std::make_shared<Dialog>(app, BorderStyle::Rounded);
  dlg->set_title(" " + title + " ");
  dlg->width = 38;
  dlg->height = 8;
  dlg->fixed_width = 38;  // needed by App::open_dialog auto-centering
  dlg->fixed_height = 8;
  dlg->modal = true;
  dlg->shadow = true;

  auto content = std::make_shared<Vertical>();

  content->add(std::make_shared<VerticalSpacer>(1));

  // Split message by '\n' and add each line as a centered label or spacer
  std::istringstream iss(message);
  std::string line;
  while (std::getline(iss, line)) {
    if (line.empty()) {
      content->add(std::make_shared<VerticalSpacer>(1));
    } else {
      auto row = std::make_shared<Align>(Align::H::Center, Align::V::Top);
      row->fixed_height = 1;
      auto lbl = std::make_shared<Label>(line);
      lbl->fixed_width = TextHelper::utf8_display_width(line);
      row->add(lbl);
      content->add(row);
    }
  }

  content->add(std::make_shared<VerticalSpacer>(1));

  auto btn_row = std::make_shared<Align>(Align::H::Center, Align::V::Center);
  auto ok = std::make_shared<Button>("OK", [dlg]() { dlg->close(); });
  ok->fixed_width = 8;
  btn_row->add(ok);
  content->add(btn_row);

  dlg->add(content);
  return dlg;
}

// =============================================================================
// main
// =============================================================================
int main() {
  App app;

  // ---- Root layout (flex spacers centre everything vertically) ----
  auto root = std::make_shared<Vertical>();

  // Helper: a horizontally-centred label row.
  // fixed_width must be set on the Label so that the Align container can
  // actually offset it — otherwise Align gives the child the full container
  // width and the centring calculation becomes a no-op.
  auto make_hint = [](const std::string &keys, const std::string &desc) {
    auto row = std::make_shared<Align>(Align::H::Center, Align::V::Top);
    row->fixed_height = 1;

    std::string full_text = keys + "  —  " + desc;
    auto lbl = std::make_shared<Label>(
        StyledText()
            .colored_bold(keys, Theme::current().warning)
            .add("  —  ")
            .add(desc));
    lbl->fixed_width = TextHelper::utf8_display_width(full_text);
    row->add(lbl);
    return row;
  };

  // Flexible spacer above — pushes the block to the vertical centre
  root->add(std::make_shared<VerticalSpacer>());

  // Title (centred)
  {
    auto row = std::make_shared<Align>(Align::H::Center, Align::V::Top);
    row->fixed_height = 1;
    std::string title_text = "Key-Event Registration Demo";
    auto title_lbl =
        std::make_shared<Label>(title_text, Theme::current().primary);
    title_lbl->fixed_width = TextHelper::utf8_display_width(title_text);
    row->add(title_lbl);
    root->add(row);
  }
  root->add(std::make_shared<VerticalSpacer>(1));

  // Shortcut hints
  root->add(make_hint("Ctrl+Shift+H", "Command palette (ctrl+shift)"));
  root->add(make_hint("Ctrl+O", "Open file      (ctrl only)"));
  root->add(make_hint("Ctrl+S", "Save file      (ctrl only)"));
  root->add(make_hint("Ctrl+F", "Find text      (ctrl only)"));
  root->add(make_hint("Ctrl+G", "Go to line     (ctrl only)"));
  root->add(make_hint("Alt+X", "Run command    (alt only)"));
  root->add(make_hint("Ctrl+Alt+D", "Debug info     (ctrl+alt)"));

  root->add(std::make_shared<VerticalSpacer>(1));

  // Footer (centred)
  {
    auto row = std::make_shared<Align>(Align::H::Center, Align::V::Top);
    row->fixed_height = 1;
    std::string footer_text = "Press  q  to quit";
    auto footer_lbl =
        std::make_shared<Label>(footer_text, Theme::current().border);
    footer_lbl->fixed_width = TextHelper::utf8_display_width(footer_text);
    row->add(footer_lbl);
    root->add(row);
  }

  // Flexible spacer below — matches the one above for true centring
  root->add(std::make_shared<VerticalSpacer>());

  // ---- Build dialogs ----
  auto dlg_palette = make_dialog(&app, "Command Palette",
                                 "Search commands and actions.\n\n"
                                 "Modifiers:  Ctrl + Shift");

  auto dlg_open = make_dialog(&app, "Open File",
                              "Would open a file picker.\n\n"
                              "Modifiers:  Ctrl only");

  auto dlg_save = make_dialog(&app, "Save File",
                              "File saved to disk.\n\n"
                              "Modifiers:  Ctrl only");

  auto dlg_find = make_dialog(&app, "Find",
                              "Search for text in the buffer.\n\n"
                              "Modifiers:  Ctrl only");

  auto dlg_goto = make_dialog(&app, "Go to Line",
                              "Jump to a specific line number.\n\n"
                              "Modifiers:  Ctrl only");

  auto dlg_run = make_dialog(&app, "Run Command",
                             "Execute a shell command.\n\n"
                             "Modifiers:  Alt only");

  auto dlg_debug = make_dialog(&app, "Debug Info",
                               "Toggle diagnostic overlay.\n\n"
                               "Modifiers:  Ctrl + Alt");

  // ---- Register key bindings ----
  //
  // register_key(key_code, callback, ctrl=false, alt=false, shift=false)
  //
  // Each shortcut calls the zero-argument dlg->open(), which delegates to
  // App::open_dialog() — the framework auto-centres the dialog using
  // Terminal::getSize() and dlg->fixed_width / fixed_height.
  //
  // Both lowercase and uppercase are registered for caps-lock robustness.

  // Ctrl+Shift+H
  app.register_key(
      'h', [dlg_palette] { dlg_palette->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/true);
  app.register_key(
      'H', [dlg_palette] { dlg_palette->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/true);

  // Ctrl+O       (ctrl only, no shift)
  app.register_key(
      'o', [dlg_open] { dlg_open->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);
  app.register_key(
      'O', [dlg_open] { dlg_open->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);

  // Ctrl+S       (ctrl only, no shift)
  app.register_key(
      's', [dlg_save] { dlg_save->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);
  app.register_key(
      'S', [dlg_save] { dlg_save->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);

  // Ctrl+F       (ctrl only, no shift)
  app.register_key(
      'f', [dlg_find] { dlg_find->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);
  app.register_key(
      'F', [dlg_find] { dlg_find->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);

  // Ctrl+G       (ctrl only, no shift)
  app.register_key(
      'g', [dlg_goto] { dlg_goto->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);
  app.register_key(
      'G', [dlg_goto] { dlg_goto->open(); },
      /*ctrl=*/true, /*alt=*/false, /*shift=*/false);

  // Alt+X        (alt only, no ctrl/shift)
  app.register_key(
      'x', [dlg_run] { dlg_run->open(); },
      /*ctrl=*/false, /*alt=*/true, /*shift=*/false);
  app.register_key(
      'X', [dlg_run] { dlg_run->open(); },
      /*ctrl=*/false, /*alt=*/true, /*shift=*/false);

  // Ctrl+Alt+D   (ctrl + alt, no shift)
  app.register_key(
      'd', [dlg_debug] { dlg_debug->open(); },
      /*ctrl=*/true, /*alt=*/true, /*shift=*/false);
  app.register_key(
      'D', [dlg_debug] { dlg_debug->open(); },
      /*ctrl=*/true, /*alt=*/true, /*shift=*/false);

  // Exit
  app.register_exit_key('q');

  app.run(root);
  return 0;
}
