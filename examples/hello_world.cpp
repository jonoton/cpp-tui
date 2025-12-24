#include "cpptui.hpp"

using namespace cpptui;

class HelloWorldWidget : public Widget {
public:
    void render(Buffer& buffer) override {
        std::string msg = "Hello, World! Press 'q' to quit.";
        int startX = 10;
        int y = 5;
        for (size_t i = 0; i < msg.length(); ++i) {
            std::string s(1, msg[i]);
            buffer.set_char(startX + i, y, s);
        }
    }
};

class DemoRoot : public Vertical {
public:
    bool on_event(const Event& event) override {
        if (Vertical::on_event(event)) return true;
        if (event.type == EventType::Key && event.key == 'q') {
            App::quit();
            return true;
        }
        return false;
    }
};

int main() {
    App app;
    auto root = std::make_shared<DemoRoot>();

    // Theme Switcher
    auto theme_layout = std::make_shared<Horizontal>();
    theme_layout->fixed_height = 1;

    auto btn_toggle = std::make_shared<Button>("Switch Theme", [](){
        static bool is_dark = true;
        is_dark = !is_dark;
        if (is_dark) Theme::set_theme(Theme::Dark());
        else Theme::set_theme(Theme::Light());
    });

    theme_layout->add(std::make_shared<Label>("Theme: "));
    theme_layout->add(btn_toggle);
    root->add(theme_layout);

    auto widget = std::make_shared<HelloWorldWidget>();
    root->add(widget);

    app.run(root);
    return 0;
}
