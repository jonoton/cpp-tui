#include "cpptui.hpp"
#include <vector>
#include <string>
#include <deque>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace cpptui;

// Simulated Log Entry
struct LogEntry {
    std::string level;
    std::string message;
    Color color;
};

class SystemSimulator : public Border {
public:
    // Simulation Parameters managed by UI controls
    float target_cpu_load = 0.3f; // 0.0 to 1.0
    float target_mem_usage = 0.4f; // 0.0 to 1.0
    bool stress_mode = false;
    std::string log_level_filter = "ALL"; // ALL, INFO, WARN, ERROR

    // Current State
    float current_cpu = 0.0f;
    float current_mem = 0.0f;
    float current_disk = 0.65f;
    float uptime = 0.0f;

    // Logs
    std::deque<LogEntry> logs;
    const size_t max_logs = 100;

    // UI References
    std::shared_ptr<ProgressBar> cpu_bar;
    std::shared_ptr<Label> cpu_text;
    std::shared_ptr<ProgressBar> mem_bar;
    std::shared_ptr<Label> mem_text;
    std::shared_ptr<ProgressBar> disk_bar;
    std::shared_ptr<Label> disk_text;
    std::shared_ptr<Label> uptime_val;
    std::shared_ptr<ScrollableVertical> log_view;

    SystemSimulator() : Border(BorderStyle::Rounded) {
        set_title(" System Monitor & Debugger ", Alignment::Center);
        // Initial logs
        add_log("INFO", "System initialized.");
        add_log("INFO", "Monitoring daemon started.");
    }

    void add_log(std::string level, std::string msg) {
        Color c = Theme::current().foreground;
        if (level == "INFO") c = Theme::current().primary;
        else if (level == "WARN") c = Theme::current().warning;
        else if (level == "ERROR") c = Theme::current().error;

        logs.push_back({level, msg, c});
        if (logs.size() > max_logs) logs.pop_front();
        refresh_logs();
    }

    void refresh_logs() {
        if (!log_view) return;
        log_view->clear_children();
        
        for (const auto& log : logs) {
            if (log_level_filter != "ALL" && log.level != log_level_filter) continue;
            
            auto row = std::make_shared<Horizontal>();
            row->fixed_height = 1;
            
            auto lvl = std::make_shared<Label>("[" + log.level + "] ");
            lvl->fixed_width = 8;
            lvl->fg_color = log.color;
            
            auto txt = std::make_shared<Label>(log.message);
            
            row->add(lvl);
            row->add(txt);
            log_view->add(row);
        }
        // Logs append at bottom; no explicit scroll-to-bottom needed
    }

    // Determine simulation state for next tick
    void tick() {
        uptime += 1.0f; // 1 second per tick roughly (simulated)

        // Update Uptime Display
        if (uptime_val) {
            int h = (int)uptime / 3600;
            int m = ((int)uptime % 3600) / 60;
            int s = (int)uptime % 60;
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(2) << h << ":" 
               << std::setw(2) << m << ":" << std::setw(2) << s;
            uptime_val->set_text(ss.str());
        }

        // Target adjustment
        float eff_cpu_target = stress_mode ? 0.95f : target_cpu_load;
        float eff_mem_target = stress_mode ? 0.90f : target_mem_usage;

        // Smooth transition to target with some noise
        float noise = ((rand() % 100) - 50) / 500.0f; // +/- 0.1
        current_cpu += (eff_cpu_target - current_cpu) * 0.2f + noise;
        if (current_cpu < 0) current_cpu = 0; if (current_cpu > 1) current_cpu = 1;

        current_mem += (eff_mem_target - current_mem) * 0.1f + noise * 0.5f;
        if (current_mem < 0) current_mem = 0; if (current_mem > 1) current_mem = 1;

        // Update Bars
        if (cpu_bar) {
            cpu_bar->value = current_cpu;
            // Color based on load
            if (current_cpu > 0.8f) cpu_bar->color = Theme::current().error;
            else if (current_cpu > 0.5f) cpu_bar->color = Theme::current().warning;
            else cpu_bar->color = Theme::current().success;
            
            std::stringstream ss; ss << (int)(current_cpu * 100) << "%";
            if (cpu_text) cpu_text->set_text(ss.str());
        }

        if (mem_bar) {
            mem_bar->value = current_mem;
            if (current_mem > 0.85f) mem_bar->color = Theme::current().error;
            else mem_bar->color = Theme::current().primary;

            std::stringstream ss; ss << (int)(current_mem * 100) << "%";
            if (mem_text) mem_text->set_text(ss.str());
        }

        if (disk_bar) {
             disk_bar->value = current_disk; 
             if (disk_text) disk_text->set_text("65%");
        }

        // Randomly generate logs
        if (rand() % 20 == 0) {
            if (current_cpu > 0.8f) add_log("WARN", "High CPU load detected!");
            else if (current_mem > 0.9f) add_log("ERROR", "Out of memory warning!");
            else add_log("INFO", "Routine system check passed.");
        }
    }
};

int main() {
    App app;
    
    // Root Layout
    auto root = std::make_shared<Vertical>();
    
    auto system_sim = std::make_shared<SystemSimulator>();
    
    // --- Top: Header ---
    auto header = std::make_shared<Label>(" SYSTEM DEBUG TOOL v1.0 ", Theme::current().primary);
    header->fixed_height = 1;
    root->add(header);

    // --- Middle Loop: Controls + Meters + Info (Horizontal Split) ---
    auto dashboard = std::make_shared<Horizontal>();
    dashboard->fixed_height = 12;

    // 1. Left controls panel
    auto controls_box = std::make_shared<Border>(BorderStyle::Single);
    controls_box->set_title(" Simulation Controls ", Alignment::Left);
    controls_box->fixed_width = 30;
    
    auto controls = std::make_shared<Vertical>();
    
    // CPU Load Control (using NumberInput with step buttons)
    controls->add(std::make_shared<Label>("Target CPU Load:"));
    auto cpu_input = std::make_shared<NumberInput>(30); // 30%
    cpu_input->step = 10;
    cpu_input->on_change = [system_sim, cpu_input](int val){
        system_sim->target_cpu_load = std::clamp(val, 0, 100) / 100.0f;
    };
    controls->add(cpu_input);

    controls->add(std::make_shared<VerticalSpacer>(1));

    controls->add(std::make_shared<Label>("Target Mem Usage:"));
    auto mem_input = std::make_shared<NumberInput>(40);
    mem_input->step = 10;
    mem_input->on_change = [system_sim, mem_input](int val){
        system_sim->target_mem_usage = std::clamp(val, 0, 100) / 100.0f;
    };
    controls->add(mem_input);

    controls->add(std::make_shared<VerticalSpacer>(1));

    auto stress_check = std::make_shared<Checkbox>("Stress Mode");
    stress_check->on_change = [system_sim](bool checked) {
        system_sim->stress_mode = checked;
    };
    controls->add(stress_check);

    controls_box->add(controls);
    dashboard->add(controls_box);

    // 2. Middle Meters
    auto meters_box = std::make_shared<Border>(BorderStyle::Single);
    meters_box->set_title(" Live Metrics ", Alignment::Center);
    
    auto meters = std::make_shared<Vertical>();
    
    // CPU
    auto r1 = std::make_shared<Horizontal>(); r1->fixed_height = 1;
    r1->add(std::make_shared<Label>("CPU: "));
    auto cpu_txt = std::make_shared<Label>("0%"); cpu_txt->fixed_width=5;
    system_sim->cpu_text = cpu_txt;
    r1->add(cpu_txt);
    meters->add(r1);
    
    auto pb_cpu = std::make_shared<ProgressBar>();
    system_sim->cpu_bar = pb_cpu;
    meters->add(pb_cpu);

    meters->add(std::make_shared<VerticalSpacer>(1));

    // MEM
    auto r2 = std::make_shared<Horizontal>(); r2->fixed_height = 1;
    r2->add(std::make_shared<Label>("RAM: "));
    auto mem_txt = std::make_shared<Label>("0%"); mem_txt->fixed_width=5;
    system_sim->mem_text = mem_txt;
    r2->add(mem_txt);
    meters->add(r2);

    auto pb_mem = std::make_shared<ProgressBar>();
    system_sim->mem_bar = pb_mem;
    meters->add(pb_mem);

    meters->add(std::make_shared<VerticalSpacer>(1));

    // DISK
    auto r3 = std::make_shared<Horizontal>(); r3->fixed_height = 1;
    r3->add(std::make_shared<Label>("DSK: "));
    auto dsk_txt = std::make_shared<Label>("65%"); dsk_txt->fixed_width=5;
    system_sim->disk_text = dsk_txt;
    r3->add(dsk_txt);
    meters->add(r3);
    
    auto pb_disk = std::make_shared<ProgressBar>();
    pb_disk->value = 0.65f;
    system_sim->disk_bar = pb_disk;
    meters->add(pb_disk);

    meters_box->add(meters);
    dashboard->add(meters_box);

    // 3. Right Info
    auto info_box = std::make_shared<Border>(BorderStyle::Single);
    info_box->set_title(" System Info ", Alignment::Right);
    info_box->fixed_width = 25;
    
    auto info = std::make_shared<Vertical>();
    info->add(std::make_shared<Label>("Host: cpp-tui-Sim"));
    info->add(std::make_shared<Label>("OS: Linux 6.5"));
    info->add(std::make_shared<Label>("Arch: x86_64"));
    info->add(std::make_shared<Label>("Cores: 8"));
    info->add(std::make_shared<VerticalSpacer>(1));
    info->add(std::make_shared<Label>("Uptime:"));
    auto up_lbl = std::make_shared<Label>("00:00:00");
    system_sim->uptime_val = up_lbl;
    info->add(up_lbl);

    info_box->add(info);
    dashboard->add(info_box);

    root->add(dashboard);

    // --- Bottom: Logs ---
    auto log_box = std::make_shared<Border>(BorderStyle::Rounded);
    log_box->set_title(" System Logs ", Alignment::Left);
    
    auto log_layout = std::make_shared<Vertical>();
    
    // Log Toolbar
    auto toolbar = std::make_shared<Horizontal>();
    toolbar->fixed_height = 1;
    toolbar->add(std::make_shared<Label>("Filter: "));
    
    auto filter_dd = std::make_shared<Dropdown>(&app);
    filter_dd->set_options({"ALL", "INFO", "WARN", "ERROR"});
    filter_dd->selected_index = 0;
    filter_dd->fixed_width = 10;
    filter_dd->on_change = [system_sim, filter_dd](int idx, std::string val) {
        system_sim->log_level_filter = val;
        system_sim->refresh_logs();
    };
    toolbar->add(filter_dd);

    toolbar->add(std::make_shared<Label>("   ")); // spacer
    auto btn_gen = std::make_shared<Button>("Gen Log", [system_sim](){
         system_sim->add_log("INFO", "Manual log entry generated by user.");
    });
    toolbar->add(btn_gen);
    
    log_layout->add(toolbar);
    auto sep_row = std::make_shared<Horizontal>();
    sep_row->add(std::make_shared<Label>("---"));
    log_layout->add(sep_row);
    
    // Scrollable Log View
    auto sv_logs = std::make_shared<ScrollableVertical>();
    system_sim->log_view = sv_logs;
    log_layout->add(sv_logs);

    log_box->add(log_layout);
    // Add logic to system sim to hold this box or add updates
    

    // Add log box to root layout
    root->add(log_box);
    
    // --- Footer ---
    auto footer = std::make_shared<Label>(" [q] Quit  |  [Tab] Next Control  |  [Space] Activate ", Color{100, 100, 100});
    footer->fixed_height = 1;
    root->add(footer);

    // Initial log population
    system_sim->refresh_logs();

    // Timer
    app.add_timer(500, [system_sim](){
        system_sim->tick();
    });

    app.register_exit_key('q');
    app.run(root);

    return 0;
}
