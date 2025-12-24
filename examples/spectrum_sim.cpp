#include "cpptui.hpp"
#include <cmath>
#include <vector>
#include <random>
#include <iostream>

using namespace cpptui;

// Helper for gaussian peak
double gaussian(double x, double center, double width, double height) {
    return height * std::exp(-std::pow(x - center, 2) / (2 * std::pow(width, 2)));
}

int main() {
    App app;
    
    // Layout
    auto root = std::make_shared<Vertical>(); // Use standard container
    
    auto header = std::make_shared<Label>("Real-time Spectrum Analyzer Simulation", Color{0, 255, 255});

    header->fixed_height = 1;
    root->add(header);
    
    auto info_box = std::make_shared<Border>(BorderStyle::Single, Color{100, 100, 100});
    auto info_txt = std::make_shared<Label>("Simulating RF Spectrum: 2.4GHz Band | Noise Floor: -90dBm | Peaks: Bluetooth/WiFi");
    info_box->set_title(" Status ", Alignment::Center);
    info_box->add(info_txt);
    info_box->fixed_height = 3;
    root->add(info_box);
    
    // Chart
    auto chart = std::make_shared<LineChart>();
    chart->min_val = 0.0;
    chart->max_val = 1.2; // Normalized power
    chart->show_legend = true;
    chart->show_x_axis = true;
    chart->show_y_axis = true;
    chart->show_y_tick_labels = true;
    chart->show_x_tick_labels = true;
    chart->show_y_ticks = true;
    chart->show_x_ticks = true;
    
    // Axis customization for spectrum display
    chart->y_tick_count = 5;
    chart->x_tick_count = 6;
    
    // Y-axis: Convert normalized power (0-1.2) to dBm scale (-90 to -20 dBm)
    chart->y_tick_formatter = [](double val) -> std::string {
        int dbm = (int)(-90 + val * 58);  // 0.0 -> -90dBm, 1.2 -> ~-20dBm
        return std::to_string(dbm) + "dB";
    };
    
    // X-axis: Frequency in MHz (2.4GHz band: 2400-2500 MHz mapped across 200 bins)
    chart->x_tick_formatter = [](double val) -> std::string {
        int mhz = 2400 + (int)(val * 100 / 200);  // Map 0-200 -> 2400-2500 MHz
        return std::to_string(mhz);
    };
    
    // Enable intermediate tick labels
    chart->label_all_x_ticks = true;
    chart->label_all_y_ticks = true;
    
    // Enable Tooltip
    chart->show_tooltip = true;
    chart->tooltip_formatter = [&](const LineChart::Series& s, int index, double val) -> std::string {
         // Reconstruct frequency from index (0-199 mapped to 2400-2500 MHz)
         int mhz = 2400 + (int)((double)index * 100.0 / 200.0);
         // Reconstruct dBm from val
         int dbm = (int)(-90 + val * 58);
         return s.label + ": " + std::to_string(mhz) + "MHz " + std::to_string(dbm) + "dBm";
    };
    
    root->add(chart);
    
    // Controls Footer
    auto footer = std::make_shared<Horizontal>();
    footer->fixed_height = 1;
    footer->add(std::make_shared<Label>("Controls: [q] Quit"));
    root->add(footer);

    // Simulation State
    double time_t = 0.0;
    std::default_random_engine gen;
    std::uniform_real_distribution<double> noise_dist(0.0, 0.15);
    
    // Carrier params
    double c1_freq = 30.0;
    double c1_amp = 0.8;
    
    double c2_freq = 70.0;
    double c2_amp = 0.5;
    
    // Simulation Persistence
    int bins = 200; // Higher resolution for Braille
    std::vector<double> max_hold(bins, 0.0);
    std::vector<double> avg_line(bins, 0.0);
    bool first_run = true;

    // Timer
    app.add_timer(30, [&]() { // Faster refresh 30ms ~30fps
        time_t += 0.04;
        
        // Evolve signals
        // Carrier 1: Sweeps slowly
        c1_freq = bins * (0.3 + 0.2 * std::sin(time_t * 0.5));
        c1_amp = 0.8 + 0.15 * std::sin(time_t * 2.3);
        
        // Carrier 2: Hopping
        if (std::fmod(time_t, 2.5) < 0.15) {
             c2_freq = bins * (0.6 + (rand() % 100) / 500.0); // Random hop
        }
        
        // Generate Current Power Spectrum
        std::vector<double> power(bins);
        
        for(int i=0; i<bins; ++i) {
            double p = 0.02; // Base floor -90dBm roughly
            
            // Thermal Noise (simulated)
            p += noise_dist(gen) * 0.6;
            
            // Signals
            p += gaussian((double)i, c1_freq, 3.0, c1_amp);
            p += gaussian((double)i, c2_freq, 2.0, c2_amp);
            
            // WiFi Burst Interference
            if (std::fmod(time_t, 3.0) > 2.2) {
                 double wifi_center = bins * 0.8;
                 // Wideband noise
                 p += gaussian((double)i, wifi_center + 10.0*std::sin(time_t*50), 10.0, 0.3 * (rand()%100/100.0));
            }
            
            if (p > 1.2) p = 1.2;
            power[i] = p;
        }

        // Update Persistence
        for(int i=0; i<bins; ++i) {
            // Max Hold
            if (power[i] > max_hold[i]) {
                max_hold[i] = power[i];
            } else {
                max_hold[i] *= 0.995; // Slow decay
            }

            // Exponential Moving Average
            if (first_run) avg_line[i] = power[i];
            else avg_line[i] = avg_line[i] * 0.9 + power[i] * 0.1;
        }
        first_run = false;
        
        // Update Chart
        chart->series.clear();
        
        // Max Hold (Red, Braille)
        chart->add_series(max_hold, "Max Hold", {255, 100, 100}, LineChart::LineStyle::Braille);
        
        // Average (Yellow, Braille)
        chart->add_series(avg_line, "Avg", {255, 255, 0}, LineChart::LineStyle::Braille);

        // Current (Green, Braille, Filled)
        chart->add_series(power, "Live", {0, 255, 0}, LineChart::LineStyle::Braille); 
        
    });
    
    app.register_exit_key('q');
    app.run(root);
    return 0;
}
