#include "../cpptui.hpp"
#include <cmath>
#include <string>
#include <vector>

using namespace cpptui;

class ChartRoot : public Vertical {
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
    auto root = std::make_shared<ChartRoot>();
    
    // Header
    auto header = std::make_shared<Label>("Comprehensive Chart Demo (Press q to quit)", Color::White());
    header->fixed_height = 1;
    root->add(header);

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

    // Main content in scrollable container
    auto scroll = std::make_shared<ScrollableVertical>();
    
    // 1. Line Chart
    auto line_title = std::make_shared<Label>("Line Chart (Sine/Cosine)", Color::Cyan());
    line_title->fixed_height = 1;
    scroll->add(line_title);
    
    auto line_chart = std::make_shared<LineChart>();
    line_chart->min_val = -1.5; 
    line_chart->max_val = 1.5;
    line_chart->show_legend = true;
    line_chart->show_y_axis = true;
    line_chart->show_x_axis = true;
    line_chart->show_y_tick_labels = true;
    line_chart->show_y_ticks = true;
    line_chart->show_x_ticks = true;
    line_chart->y_tick_count = 3;
    line_chart->x_tick_count = 5;
    line_chart->y_tick_precision = 2;
    line_chart->fixed_height = 10;
    line_chart->show_tooltip = true; // Enable Tooltips
    
    // Custom Tooltip Formatter
    line_chart->tooltip_formatter = [](const LineChart::Series& s, int idx, double val) -> std::string {
        std::stringstream ss;
        ss << s.label << " [" << idx << "]: " << std::fixed << std::setprecision(3) << val;
        return ss.str();
    };
    
    scroll->add(line_chart);

    // 2. Scatter Chart
    auto scatter_title = std::make_shared<Label>("Scatter Chart (Orbits)", Color::Yellow());
    scatter_title->fixed_height = 1;
    scroll->add(scatter_title);
    
    auto scatter_chart = std::make_shared<ScatterChart>();
    scatter_chart->x_min = -2.0; scatter_chart->x_max = 2.0;
    scatter_chart->y_min = -2.0; scatter_chart->y_max = 2.0;
    scatter_chart->show_legend = true;
    scatter_chart->show_x_axis = true;
    scatter_chart->show_y_axis = true;
    scatter_chart->show_x_tick_labels = true;
    scatter_chart->show_y_tick_labels = true;
    scatter_chart->show_x_ticks = true;
    scatter_chart->show_y_ticks = true;
    scatter_chart->x_tick_count = 5;
    scatter_chart->y_tick_count = 5;
    scatter_chart->x_tick_precision = 1;
    scatter_chart->y_tick_precision = 1;
    scatter_chart->fixed_height = 10;
    scatter_chart->show_tooltip = true; // Enable Tooltips
    scroll->add(scatter_chart);
    
    // 3. Bar Chart (Multi-Series)
    auto bar_title = std::make_shared<Label>("Bar Chart (Quarterly Revenue)", Color::Green());
    bar_title->fixed_height = 1;
    scroll->add(bar_title);
    
    auto bar_chart = std::make_shared<BarChart>();
    bar_chart->categories = {"Q1", "Q2", "Q3", "Q4"};
    bar_chart->add_series({100, 150, 120, 200}, "2023", Color{100, 100, 255});
    bar_chart->add_series({110, 130, 180, 220}, "2024", Color{255, 100, 100});
    bar_chart->show_legend = true;
    bar_chart->show_y_axis = true;
    bar_chart->show_x_axis = true;
    bar_chart->show_y_tick_labels = true;
    bar_chart->show_y_ticks = true;
    bar_chart->show_x_ticks = true;
    bar_chart->y_tick_count = 4;
    bar_chart->y_tick_formatter = [](double val) -> std::string {
        return "$" + std::to_string((int)val);
    };
    bar_chart->fixed_height = 8;
    bar_chart->show_tooltip = true; 
    
    // Custom Tooltip for Bar Chart
    bar_chart->tooltip_formatter = [](const std::string& series, const std::string& cat, double val) -> std::string {
        std::stringstream ss;
        ss << series << " (" << cat << "): " << "$" << (int)val;
        return ss.str();
    };
    
    scroll->add(bar_chart);

    // 4. Gauge (NEW)
    scroll->add(std::make_shared<VerticalSpacer>(1));
    auto gauge_title = std::make_shared<Label>("Gauge (CPU Usage - Animated)", Color::Magenta());
    gauge_title->fixed_height = 1;
    scroll->add(gauge_title);
    
    auto gauge = std::make_shared<Gauge>(0.0);
    gauge->min_label = "0%";
    gauge->max_label = "100%";
    gauge->fixed_height = 4;
    scroll->add(gauge);

    // 5. ProportionalBar (NEW)
    scroll->add(std::make_shared<VerticalSpacer>(1));
    auto pie_title = std::make_shared<Label>("Proportional Bar (Market Share - Animated)", Color::Cyan());
    pie_title->fixed_height = 1;
    scroll->add(pie_title);
    
    auto pie_chart = std::make_shared<ProportionalBar>();
    pie_chart->add_segment(45, "Chrome", Color::Blue());
    pie_chart->add_segment(30, "Firefox", Color::Red());
    pie_chart->add_segment(15, "Safari", Color::Cyan());
    pie_chart->add_segment(10, "Other", Color::Green());
    pie_chart->show_percentages = true;
    pie_chart->fixed_height = 5;
    scroll->add(pie_chart);

    // 6. Heatmap (NEW)
    scroll->add(std::make_shared<VerticalSpacer>(1));
    auto heatmap_title = std::make_shared<Label>("Heatmap (Activity Grid - Animated)", Color::Yellow());
    heatmap_title->fixed_height = 1;
    scroll->add(heatmap_title);
    
    auto heatmap = std::make_shared<Heatmap>();
    heatmap->row_labels = {"Mon", "Tue", "Wed", "Thu", "Fri"};
    heatmap->col_labels = {"0-4", "4-8", "8-12", "12-16", "16-20", "20-24"};
    heatmap->data = {
        {0.1, 0.2, 0.8, 0.9, 0.7, 0.3},
        {0.2, 0.3, 0.7, 0.8, 0.6, 0.2},
        {0.1, 0.4, 0.9, 1.0, 0.8, 0.4},
        {0.3, 0.5, 0.8, 0.7, 0.5, 0.2},
        {0.1, 0.2, 0.6, 0.5, 0.3, 0.1}
    };
    heatmap->fixed_height = 5;
    scroll->add(heatmap);

    root->add(scroll);

    // Animation Logic
    double time = 0;
    app.add_timer(50, [line_chart, scatter_chart, gauge, pie_chart, heatmap, &time]() {
        time += 0.1;
        
        // Update Line Chart
        std::vector<double> sin_data, cos_data;
        for(int i=0; i<50; ++i) {
            sin_data.push_back(std::sin(time + i * 0.2));
            cos_data.push_back(std::cos(time + i * 0.2));
        }
        line_chart->series.clear();
        line_chart->add_series(sin_data, "Sin (Braille)", {0, 255, 255}, LineChart::LineStyle::Braille);
        line_chart->add_series(cos_data, "Cos (Lines)", {255, 0, 255}, LineChart::LineStyle::Lines, "#");
        
        // Update Scatter Chart
        std::vector<std::pair<double,double>> planets, comets;
        for(int i=0; i<8; ++i) {
             double angle = time * 0.5 + i * (6.28 / 8);
             planets.push_back({std::cos(angle)*1.5, std::sin(angle)*1.5});
        }
        comets.push_back({std::cos(time*2.0)*1.8, std::sin(time*2.0)*0.5});
        
        scatter_chart->series.clear();
        scatter_chart->add_series(planets, "Planets (Braille)", {255, 255, 0}, "*", true); 
        scatter_chart->add_series(comets, "Comet (Char)", {0, 255, 0}, "+", false);

        // Update Gauge (oscillating CPU usage)
        gauge->value = 0.5 + 0.4 * std::sin(time * 0.5);
        
        // Update ProportionalBar (shifting market share)
        double shift = std::sin(time * 0.3) * 10;
        pie_chart->segments.clear();
        pie_chart->add_segment(45 + shift, "Chrome", Color::Blue());
        pie_chart->add_segment(30 - shift * 0.5, "Firefox", Color::Red());
        pie_chart->add_segment(15 + shift * 0.3, "Safari", Color::Cyan());
        pie_chart->add_segment(10 - shift * 0.2, "Other", Color::Green());
        
        // Update Heatmap (wave pattern)
        for (size_t r = 0; r < heatmap->data.size(); ++r) {
            for (size_t c = 0; c < heatmap->data[r].size(); ++c) {
                double wave = std::sin(time + r * 0.5 + c * 0.3);
                heatmap->data[r][c] = 0.5 + 0.5 * wave;
            }
        }
    });

    app.run(root);
    return 0;
}

