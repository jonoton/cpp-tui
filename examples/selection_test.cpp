// selection_test.cpp
//
// Demonstrates UTF-8 text selection, styled text spans, and word wrapping
// across the Static, Label, Paragraph, and TextList widgets.
//
// Each widget displays roughly 80+ characters of text where the first half
// is plain ASCII and the second half contains multi-byte UTF-8 characters
// (accented letters, CJK text, emoji) inside styled spans. The Paragraph
// and TextList word wrap, so the styled unicode tail lands on wrapped
// lines.
//
// What to verify:
//   - Mouse selection over multi-byte UTF-8 characters does not corrupt
//     the selection range or the copied text.
//   - Ctrl+A selects all text of the focused widget and Ctrl+C copies the
//     selection to the clipboard (including the OSC 52 fallback path).
//   - Clicking directly on the first character of a wrapped line in the
//     Paragraph selects that character.
//   - Styled spans keep their formatting after word wrapping.
//
// Press 'q' to quit.

#include "cpptui.hpp"

using namespace cpptui;

namespace {

StyledText make_static_text() {
  StyledText text;
  text.add("Static: plain ASCII first half, unicode styled second half: ");
  text.bold("Ünïcödé ✓ ");
  text.colored("héllo wörld ", Theme::current().warning);
  text.italic("日本語 🎉");
  return text;
}

StyledText make_label_text() {
  StyledText text;
  text.add("Label: plain ASCII first half, unicode styled second half: ");
  text.underline("Ærøskøbing café naïve ");
  text.colored("中文测试 ", Theme::current().success);
  text.bold_italic("🚀✨ tail");
  return text;
}

StyledText make_paragraph_text() {
  StyledText text;
  text.add("Paragraph: long enough to word wrap at the border. ");
  text.bold("Ünïcödé tail: ");
  text.colored("héllo wörld ✓ ", Theme::current().warning);
  text.italic("日本語のテキスト ");
  text.colored_bold("🎉🚀 émoji tail", Theme::current().secondary);
  return text;
}

ListItem make_list_item(const std::string &prefix, const std::string &tail,
                        int level = 0) {
  StyledText text;
  text.add(prefix);
  text.bold("Ünïcödé ✓ ");
  text.colored(tail, Theme::current().primary);
  return ListItem(text, level);
}

std::vector<ListItem> make_list_items() {
  return {
      make_list_item("Item one: ASCII padding so wrapping happens later: ",
                     "héllo wörld 日本語 🎉"),
      make_list_item("Item two: ASCII padding so wrapping happens later: ",
                     "中文测试 🚀✨", 1),
      make_list_item("Item three: ASCII padding so wrapping happens later: ",
                     "naïve café 🎉"),
  };
}

}  // namespace

class SelectionTestRoot : public Border {
 public:
  SelectionTestRoot() : Border(BorderStyle::Rounded) {
    set_title(" Selection Test - mouse select, Ctrl+A/Ctrl+C copy, q quits ");
  }

  bool on_event(const Event &event) override {
    if (Border::on_event(event)) return true;
    if (event.type == EventType::Key && event.key == 'q') {
      App::quit();
      return true;
    }
    return false;
  }
};

int main() {
  Theme::set_theme(Theme::Nord());

  App app;
  auto root = std::make_shared<SelectionTestRoot>();
  auto layout = std::make_shared<Vertical>();
  root->add(layout);

  auto help = std::make_shared<Label>(
      "Mouse drag selects, Ctrl+A select all, Ctrl+C copy, q quits.");
  help->fg_color = Theme::current().foreground;
  help->fixed_height = 1;
  layout->add(help);

  auto add_section = [&](const std::string &title, Color accent,
                         std::shared_ptr<Widget> widget) {
    widget->fg_color = Theme::current().foreground;
    auto border = std::make_shared<Border>(BorderStyle::Single, accent);
    border->set_title(" " + title + " ");
    border->add(widget);
    layout->add(border);
  };

  add_section("Static", Theme::current().primary,
              std::make_shared<Static>(make_static_text()));
  add_section("Label", Theme::current().secondary,
              std::make_shared<Label>(make_label_text()));
  add_section("Paragraph", Theme::current().success,
              std::make_shared<Paragraph>(make_paragraph_text()));
  add_section("TextList", Theme::current().error,
              std::make_shared<TextList>(make_list_items()));

  app.run(root);
  return 0;
}
