#include "cpptui.hpp"
#include <cmath>

using namespace cpptui;

class ColorGrid : public Widget
{
public:
    void render(Buffer &buffer) override
    {
        // Render a spectrum
        for (int dy = 0; dy < height; ++dy)
        {
            for (int dx = 0; dx < width; ++dx)
            {
                float hue = (float)dx / (float)width;
                float val = (float)dy / (float)height;

                // Use library's HSV to RGB conversion
                Color c = Color::hsv_to_rgb(hue, 1.0f, val);

                Cell cell;
                cell.content = " "; // Solid block using background color
                cell.bg_color = c;

                // Overlay text for contrast test
                if ((dx + dy) % 10 == 0)
                {
                    cell.content = ".";
                    cell.fg_color = Color::White();
                }

                buffer.set(x + dx, y + dy, cell);
            }
        }
    }
};

int main()
{
    App app;
    auto grid = std::make_shared<ColorGrid>();
    app.run(grid);
    return 0;
}
