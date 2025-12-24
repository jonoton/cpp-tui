#include "cpptui.hpp"

using namespace cpptui;

class ColorBox : public Widget {
public:
    ColorBox(Color c, std::string label) : c_(c), label_(label) {}
    
    void render(Buffer& buffer) override {
        for(int dy=0; dy<height; ++dy) {
            for(int dx=0; dx<width; ++dx) {
                Cell cell;
                // Fill with color
                cell.content = " ";
                cell.bg_color = c_;
                
                // Center text
                if (dy == height/2) {
                     int txtStart = (width - (int)label_.length())/2;
                     if (dx >= txtStart && dx < txtStart + (int)label_.length()) {
                         cell.content = std::string(1, label_[dx-txtStart]);
                         cell.fg_color = Color::White();
                     }
                }
                buffer.set(x+dx, y+dy, cell);
            }
        }
    }
private:
    Color c_;
    std::string label_;
};

class LayoutRoot : public Border {
public:
    using Border::Border;
    bool on_event(const Event& event) override {
        if (Border::on_event(event)) return true;
        if (event.type == EventType::Key && event.key == 'q') {
            if (quit_app) quit_app();
            return true;
        }
        return false;
    }
};

int main() {
    App app;
    
    // Root with Rounded Border (Cyan)
    auto root = std::make_shared<LayoutRoot>(BorderStyle::Rounded, Color{0, 255, 255});

    // Main vertical container
    auto main_col = std::make_shared<Vertical>();
    root->add(main_col);
    
    // Top Row
    auto top_row = std::make_shared<Horizontal>();
    // Wrap items in borders to show nesting styles
    auto b1 = std::make_shared<Border>(BorderStyle::Single, Color::Red());
    b1->add(std::make_shared<ColorBox>(Color{50,0,0}, "Top Left"));
    top_row->add(b1);
    
    auto b2 = std::make_shared<Border>(BorderStyle::Double, Color::Blue());
    b2->add(std::make_shared<ColorBox>(Color{0,0,50}, "Top Right"));
    top_row->add(b2);
    
    // Middle Area (Nested Mix)
    auto mid_row = std::make_shared<Horizontal>();
    
    auto left_panel = std::make_shared<Vertical>();
    left_panel->add(std::make_shared<ColorBox>(Color{50,50,50}, "Menu 1"));
    left_panel->add(std::make_shared<ColorBox>(Color{70,70,70}, "Menu 2"));
    left_panel->add(std::make_shared<ColorBox>(Color{90,90,90}, "Menu 3"));
    
    auto content_panel = std::make_shared<ColorBox>(Color{20,20,40}, "Main Content Area");
    
    mid_row->add(left_panel);
    mid_row->add(content_panel);
    
    // Footer
    auto footer = std::make_shared<ColorBox>(Color{0,100,0}, "Footer Status");
    
    main_col->add(top_row);
    main_col->add(mid_row);
    main_col->add(footer);
    
    app.run(root); // Run with border root
    return 0;
}
