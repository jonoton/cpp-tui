#include <cmath>
#include <string>
#include <vector>

#include "../cpptui.hpp"

using namespace cpptui;

class ChartRoot : public Vertical {
 public:
  bool on_event(const Event &event) override {
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
  auto header = std::make_shared<Label>(
      "Comprehensive Chart Demo (Press q to quit)", Color::White());
  header->fixed_height = 1;
  root->add(header);

  // Theme Switcher
  auto theme_layout = std::make_shared<Horizontal>();
  theme_layout->fixed_height = 1;

  auto btn_toggle = std::make_shared<Button>("Switch Theme", []() {
    static bool is_dark = true;
    is_dark = !is_dark;
    if (is_dark)
      Theme::set_theme(Theme::Dark());
    else
      Theme::set_theme(Theme::Light());
  });

  theme_layout->add(std::make_shared<Label>("Theme: "));
  theme_layout->add(btn_toggle);
  root->add(theme_layout);

  // Main content in scrollable container
  auto scroll = std::make_shared<ScrollableVertical>();

  // 1. Line Chart
  auto line_title =
      std::make_shared<Label>("Line Chart (Sine/Cosine)", Color::Cyan());
  line_title->fixed_height = 1;
  scroll->add(line_title);

  auto line_chart = std::make_shared<LineChart>();
  line_chart->show_legend = true;
  line_chart->show_y_axis = true;
  line_chart->show_x_axis = true;
  line_chart->show_y_tick_labels = true;
  line_chart->show_y_ticks = true;
  line_chart->show_x_ticks = true;
  line_chart->show_grid_lines = true;
  line_chart->auto_scale = true;
  line_chart->y_tick_count = 3;
  line_chart->x_tick_count = 5;
  line_chart->y_tick_precision = 2;
  line_chart->fixed_height = 10;
  line_chart->show_tooltip = true;  // Enable Tooltips

  // Custom Tooltip Formatter
  line_chart->tooltip_formatter = [](const LineChart::Series &s, int idx,
                                     double val) -> std::string {
    std::stringstream ss;
    ss << s.label << " [" << idx << "]: " << std::fixed << std::setprecision(3)
       << val;
    return ss.str();
  };

  scroll->add(line_chart);

  // 2. Scatter Chart
  auto scatter_title =
      std::make_shared<Label>("Scatter Chart (Orbits)", Color::Yellow());
  scatter_title->fixed_height = 1;
  scroll->add(scatter_title);

  auto scatter_chart = std::make_shared<ScatterChart>();
  scatter_chart->x_min = -2.0;
  scatter_chart->x_max = 2.0;
  scatter_chart->y_min = -2.0;
  scatter_chart->y_max = 2.0;
  scatter_chart->show_legend = true;
  scatter_chart->show_x_axis = true;
  scatter_chart->show_y_axis = true;
  scatter_chart->show_x_tick_labels = true;
  scatter_chart->show_y_tick_labels = true;
  scatter_chart->show_x_ticks = true;
  scatter_chart->show_y_ticks = true;
  scatter_chart->show_grid_lines = true;
  scatter_chart->auto_scale = false;
  scatter_chart->x_tick_count = 5;
  scatter_chart->y_tick_count = 5;
  scatter_chart->x_tick_precision = 1;
  scatter_chart->y_tick_precision = 1;
  scatter_chart->fixed_height = 10;
  scatter_chart->show_tooltip = true;  // Enable Tooltips
  scroll->add(scatter_chart);

  // 3. Bar Chart (Multi-Series)
  auto bar_title =
      std::make_shared<Label>("Bar Chart (Quarterly Revenue)", Color::Green());
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
  bar_chart->show_grid_lines = true;
  bar_chart->y_tick_count = 4;
  bar_chart->y_tick_formatter = [](double val) -> std::string {
    return "$" + std::to_string((int)val);
  };
  bar_chart->fixed_height = 8;
  bar_chart->show_tooltip = true;

  // Custom Tooltip for Bar Chart
  bar_chart->tooltip_formatter = [](const std::string &series,
                                    const std::string &cat,
                                    double val) -> std::string {
    std::stringstream ss;
    ss << series << " (" << cat << "): "
       << "$" << (int)val;
    return ss.str();
  };

  scroll->add(bar_chart);

  // 4. Gauge (NEW)
  scroll->add(std::make_shared<VerticalSpacer>(1));
  auto gauge_title =
      std::make_shared<Label>("Gauge (CPU Usage - Animated)", Color::Magenta());
  gauge_title->fixed_height = 1;
  scroll->add(gauge_title);

  auto gauge = std::make_shared<Gauge>(0.0);
  gauge->min_label = "0%";
  gauge->max_label = "100%";
  gauge->fixed_height = 4;
  scroll->add(gauge);

  // 5. ProportionalBar (NEW)
  scroll->add(std::make_shared<VerticalSpacer>(1));
  auto pie_title = std::make_shared<Label>(
      "Proportional Bar (Market Share - Animated)", Color::Cyan());
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

  // 5b. Circular Pie / Donut Chart (NEW)
  scroll->add(std::make_shared<VerticalSpacer>(1));
  auto circular_pie_title = std::make_shared<Label>(
      "Circular Donut Chart (Animated - Size Oscillates to Demo Scaling)",
      Color::Yellow());
  circular_pie_title->fixed_height = 1;
  scroll->add(circular_pie_title);

  auto circular_pie = std::make_shared<PieChart>();
  circular_pie->donut = true;
  circular_pie->fixed_height = 10;
  scroll->add(circular_pie);

  // 6. Heatmap (NEW)
  scroll->add(std::make_shared<VerticalSpacer>(1));
  auto heatmap_title = std::make_shared<Label>(
      "Heatmap (Activity Grid - Animated)", Color::Yellow());
  heatmap_title->fixed_height = 1;
  scroll->add(heatmap_title);

  auto heatmap = std::make_shared<Heatmap>();
  heatmap->row_labels = {"Mon", "Tue", "Wed", "Thu", "Fri"};
  heatmap->col_labels = {"0-4", "4-8", "8-12", "12-16", "16-20", "20-24"};
  heatmap->data = {{0.1, 0.2, 0.8, 0.9, 0.7, 0.3},
                   {0.2, 0.3, 0.7, 0.8, 0.6, 0.2},
                   {0.1, 0.4, 0.9, 1.0, 0.8, 0.4},
                   {0.3, 0.5, 0.8, 0.7, 0.5, 0.2},
                   {0.1, 0.2, 0.6, 0.5, 0.3, 0.1}};
  heatmap->fixed_height = 5;
  heatmap->show_tooltip = true;
  scroll->add(heatmap);

  // 7. Enhanced Sparkline (NEW)
  scroll->add(std::make_shared<VerticalSpacer>(1));
  auto spark_title = std::make_shared<Label>(
      "Enhanced Sparkline (Multi-Line, Auto-Scale, Thresholds - Animated)",
      Color::Cyan());
  spark_title->fixed_height = 1;
  scroll->add(spark_title);

  auto sparkline = std::make_shared<Sparkline>();
  sparkline->height = 3;
  sparkline->fixed_height = 3;
  sparkline->auto_scale = true;
  sparkline->show_label = true;
  sparkline->label_format = " %.2f";
  sparkline->color_thresholds = {{0.5f, Color::Yellow()}, {0.8f, Color::Red()}};
  scroll->add(sparkline);

  // 8. Box and Whisker Plot (NEW)
  scroll->add(std::make_shared<VerticalSpacer>(1));
  auto box_title = std::make_shared<Label>(
      "Box & Whisker Plot (Statistical Distributions - Animated)",
      Color::Yellow());
  box_title->fixed_height = 1;
  scroll->add(box_title);

  auto box_plot = std::make_shared<BoxWhiskerPlot>();
  box_plot->label = "Latency";
  box_plot->box_color = Theme::current().primary;
  box_plot->whisker_color = Theme::current().foreground;
  box_plot->median_color = Theme::current().secondary;
  scroll->add(box_plot);

  root->add(scroll);

  // Animation Logic
  double time = 0;
  app.add_timer(50, [line_chart, scatter_chart, gauge, pie_chart, circular_pie,
                     heatmap, sparkline, box_plot, &time]() {
    time += 0.1;

    Color bg = Theme::current().background;
    Color pri = Theme::current().primary;
    Color sec = Theme::current().secondary;
    Color succ = Theme::current().success;
    Color warn = Theme::current().warning;

    // Blend: 70% background, 30% primary/secondary
    Color fill_pri((uint8_t)(bg.r * 0.7 + pri.r * 0.3),
                   (uint8_t)(bg.g * 0.7 + pri.g * 0.3),
                   (uint8_t)(bg.b * 0.7 + pri.b * 0.3));
    Color fill_sec((uint8_t)(bg.r * 0.7 + sec.r * 0.3),
                   (uint8_t)(bg.g * 0.7 + sec.g * 0.3),
                   (uint8_t)(bg.b * 0.7 + sec.b * 0.3));

    // Update Line Chart
    std::vector<double> sin_data, cos_data;
    for (int i = 0; i < 50; ++i) {
      sin_data.push_back(std::sin(time + i * 0.2));
      cos_data.push_back(std::cos(time + i * 0.2));
    }
    line_chart->series.clear();
    line_chart->add_series(sin_data, "Sin (Braille)", pri,
                           LineChart::LineStyle::Braille);
    line_chart->add_series(cos_data, "Cos (Lines)", sec,
                           LineChart::LineStyle::Lines, "#");
    line_chart->series[0].fill = true;
    line_chart->series[0].fill_color = fill_pri;
    line_chart->series[1].fill = true;
    line_chart->series[1].fill_char = "░";
    line_chart->series[1].fill_color = fill_sec;

    // Update Scatter Chart
    std::vector<std::pair<double, double>> planets, comets;
    for (int i = 0; i < 8; ++i) {
      double angle = time * 0.5 + i * (6.28 / 8);
      planets.push_back({std::cos(angle) * 1.5, std::sin(angle) * 1.5});
    }
    comets.push_back({std::cos(time * 2.0) * 1.8, std::sin(time * 2.0) * 0.5});

    scatter_chart->series.clear();
    scatter_chart->add_series(planets, "Planets (Braille)", pri, "*", true);
    scatter_chart->add_series(comets, "Comet (Char)", sec, "+", false);

    // Update Gauge (oscillating CPU usage)
    gauge->value = 0.5 + 0.4 * std::sin(time * 0.5);

    // Update ProportionalBar (shifting market share)
    double shift = std::sin(time * 0.3) * 10;
    pie_chart->segments.clear();
    pie_chart->add_segment(45 + shift, "Chrome", pri);
    pie_chart->add_segment(30 - shift * 0.5, "Firefox", sec);
    pie_chart->add_segment(15 + shift * 0.3, "Safari", succ);
    pie_chart->add_segment(10 - shift * 0.2, "Other", warn);

    // Update Circular Pie / Donut
    circular_pie->segments.clear();
    circular_pie->add_segment(45 + shift, "Chrome", pri);
    circular_pie->add_segment(30 - shift * 0.5, "Firefox", sec);
    circular_pie->add_segment(15 + shift * 0.3, "Safari", succ);
    circular_pie->add_segment(10 - shift * 0.2, "Other", warn);
    circular_pie->radius_scale = 0.8 + 0.2 * std::sin(time * 0.5);

    // Update Heatmap (wave pattern)
    for (size_t r = 0; r < heatmap->data.size(); ++r) {
      for (size_t c = 0; c < heatmap->data[r].size(); ++c) {
        double wave = std::sin(time + r * 0.5 + c * 0.3);
        heatmap->data[r][c] = 0.5 + 0.5 * wave;
      }
    }

    // Update Sparkline
    static std::vector<float> spark_data;
    spark_data.push_back(0.5f + 0.45f * std::sin(time * 0.8f));
    if (spark_data.size() > 80) spark_data.erase(spark_data.begin());
    sparkline->data = spark_data;

    // Update Box Plot (oscillating median and quartiles)
    double med = 50.0 + 10.0 * std::sin(time * 0.5);
    box_plot->set_data(10.0, 35.0, med, 70.0, 95.0);
  });

  app.run(root);
  return 0;
}
