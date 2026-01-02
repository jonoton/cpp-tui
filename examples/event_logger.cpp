#include "cpptui.hpp"
#include <deque>
#include <iomanip>

using namespace cpptui;

class EventLog : public Widget
{
public:
    void log(const Event &e)
    {
        std::stringstream ss;
        ss << "Type: ";
        switch (e.type)
        {
        case EventType::None:
            ss << "None";
            break;
        case EventType::Resize:
            ss << "Resize(" << e.x << "x" << e.y << ")";
            break;
        case EventType::Key:
            if (e.key < 32)
                ss << "Key(" << (int)e.key << " ^" << (char)(e.key + 64) << ")";
            else if (e.key < 127)
                ss << "Key(" << (int)e.key << " '" << (char)e.key << "')";
            else
                ss << "Key(" << (int)e.key << ")";
            if (e.ctrl)
                ss << "+Ctrl";
            if (e.shift)
                ss << "+Shift";
            if (e.alt)
                ss << "+Alt";
            break;
        case EventType::Mouse:
            ss << "Mouse(" << e.x << ", " << e.y << ") ";
            if (e.mouse_drag())
                ss << "Drag ";
            else if (e.mouse_move())
                ss << "Move ";
            else if (e.mouse_left())
                ss << "Click(Left) ";
            else if (e.mouse_right())
                ss << "Click(Right) ";
            else if (e.mouse_middle())
                ss << "Click(Middle) ";
            else if (e.mouse_release())
                ss << "Release ";
            else if (e.mouse_wheel())
            {
                if (e.button == 64)
                    ss << "Wheel(Up) ";
                else if (e.button == 65)
                    ss << "Wheel(Down) ";
                else
                    ss << "Wheel ";
            }
            else
                ss << "Unknown(" << e.button << ") ";

            if (e.ctrl)
                ss << "+Ctrl";
            if (e.shift)
                ss << "+Shift";
            if (e.alt)
                ss << "+Alt";
            break;

        case EventType::Quit:
            ss << "Quit";
            break;
        case EventType::Paste:
            ss << "Paste(text: '" << e.paste_text << "')";
            break;
        default:
            ss << "Unknown(" << (int)e.type << ")";
            break;
        }

        logs_.push_front(ss.str());
        if (logs_.size() > 50)
            logs_.pop_back();
    }

    void render(Buffer &buffer) override
    {
        int current_y = y;

        // Title
        std::string title = "Event Logger (Press keys or click)";
        for (int i = 0; i < (int)title.length() && i < width; ++i)
        {
            buffer.set_char(x + i, current_y, std::string(1, title[i]));
        }
        current_y++;

        // Separator
        for (int i = 0; i < width; ++i)
            buffer.set_char(x + i, current_y, "-");
        current_y++;

        // Logs
        for (const auto &line : logs_)
        {
            if (current_y >= y + height)
                break;
            for (int dx = 0; dx < width && dx < (int)line.length(); ++dx)
            {
                buffer.set_char(x + dx, current_y, std::string(1, line[dx]));
            }
            current_y++;
        }
    }

    // Transparently consume all events to log them
    bool on_event(const Event &event) override
    {
        log(event);
        if (event.type == EventType::Mouse)
        {
            // Only consume if inside our bounds
            if (event.x >= x && event.x < x + width &&
                event.y >= y && event.y < y + height)
            {
                return true;
            }
            return false; // Pass through to other widgets (like Buttons)
        }
        return true;
    }

private:
    std::deque<std::string> logs_;
};

int main()
{
    App app;
    auto root = std::make_shared<Vertical>();

    // Theme Switcher
    auto theme_layout = std::make_shared<Horizontal>();
    theme_layout->fixed_height = 1;

    auto btn_toggle = std::make_shared<Button>("Switch Theme", []()
                                               {
        static bool is_dark = true;
        is_dark = !is_dark;
        if (is_dark) Theme::set_theme(Theme::Dark());
        else Theme::set_theme(Theme::Light()); });

    theme_layout->add(std::make_shared<Label>("Theme: "));
    theme_layout->add(btn_toggle);
    root->add(theme_layout);

    auto logger = std::make_shared<EventLog>();
    root->add(logger);

    app.run(root);
    return 0;
}
