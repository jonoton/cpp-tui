#include "cpptui.hpp"
#include <iostream>

using namespace cpptui;

// A simple container that does not enforce layout, allowing manual positioning
class SimpleContainer : public Container
{
public:
    void layout() override
    {
        // Do nothing, let manual x/y override
    }
};

class Background : public Widget
{
public:
    void render(Buffer &buffer) override
    {
        int w = buffer.width();
        int h = buffer.height();
        int half_w = w / 2;
        int half_h = h / 2;

        auto fill = [&](int start_x, int start_y, int end_x, int end_y, Color c)
        {
            for (int y = start_y; y < end_y; ++y)
            {
                if (y >= h)
                    break;
                for (int x = start_x; x < end_x; ++x)
                {
                    if (x >= w)
                        break;
                    Cell cell;
                    cell.content = " ";
                    cell.bg_color = c;
                    buffer.set(x, y, cell);
                }
            }
        };

        // Quadrants:
        // TL: Black (Low contrast for shadow)
        fill(0, 0, half_w, half_h, {0, 0, 0});
        // TR: White (High contrast)
        fill(half_w, 0, w, half_h, {200, 200, 200});
        // BL: Navy Blue
        fill(0, half_h, half_w, h, {0, 0, 100});
        // BR: Dark Red
        fill(half_w, half_h, w, h, {100, 0, 0});

        std::string label = "Shadow Test (Press 'q' to quit)";
        for (int i = 0; i < (int)label.length(); ++i)
        {
            Cell text;
            text.content = std::string(1, label[i]);
            text.fg_color = {128, 128, 128};
            buffer.set(1 + i, 1, text);
        }
    }
};

int main()
{
    App app;
    std::shared_ptr<Container> root = std::make_shared<SimpleContainer>();

    // Background should fill screen.
    class LayeredContainer : public Container
    {
    public:
        void layout() override
        {
            for (auto &child : children_)
            {
                child->x = x;
                child->y = y;
                child->width = width;
                child->height = height;
                // Recursive
                if (auto cont = std::dynamic_pointer_cast<Container>(child))
                {
                    cont->layout();
                }
            }
        }
    };

    root = std::make_shared<SimpleContainer>();

    class TestLayout : public Container
    {
    public:
        void layout() override
        {
            // First child is BG, fill screen
            if (children_.empty())
                return;

            children_[0]->x = x;
            children_[0]->y = y;
            children_[0]->width = width;
            children_[0]->height = height;
            // Menus are now overlays, not children
        }
    };

    root = std::make_shared<TestLayout>();

    auto bg = std::make_shared<Background>();
    root->add(bg);

    // Helper to add menu
    auto add_menu = [&](std::string title)
    {
        auto menu = std::make_shared<MenuDialog>(&app);
        menu->set_title(title);
        menu->items.push_back({"Colors", []() {}});
        menu->items.push_back({"Columns", []() {}});
        menu->items.push_back({"Quit", []()
                               { App::quit(); }});

        // Get terminal size to match background quadrant positions
        auto [term_w, term_h] = Terminal::getSize();
        int half_w = term_w / 2;
        int half_h = term_h / 2;

        // Menu dimensions (approximate)
        int menu_w = 14; // ~width of menu
        int menu_h = 4;  // ~height of menu

        int mx = 0, my = 0;
        static int m_count = 0;

        // Center each menu within its quadrant
        if (m_count == 0)
        {
            // Top-left quadrant (Dark)
            mx = (half_w - menu_w) / 2;
            my = (half_h - menu_h) / 2;
        }
        else if (m_count == 1)
        {
            // Top-right quadrant (Light)
            mx = half_w + (half_w - menu_w) / 2;
            my = (half_h - menu_h) / 2;
        }
        else if (m_count == 2)
        {
            // Bottom-left quadrant (Blue)
            mx = (half_w - menu_w) / 2;
            my = half_h + (half_h - menu_h) / 2;
        }
        else
        {
            // Bottom-right quadrant (Red)
            mx = half_w + (half_w - menu_w) / 2;
            my = half_h + (half_h - menu_h) / 2;
        }
        m_count++;

        menu->open(mx, my);
    };

    add_menu("Dark BG");
    add_menu("Light BG");
    add_menu("Blue BG");
    add_menu("Red BG");

    app.register_exit_key('q');
    app.run(root);
    return 0;
}
