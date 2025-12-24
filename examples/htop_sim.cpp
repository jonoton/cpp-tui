#include "cpptui.hpp"
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace cpptui;

// Helper to format memory
std::string format_mem(float usage_mb) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << (usage_mb / 1024.0) << "G";
    return ss.str();
}

// Helper for creating labels - use default colors to respect theme changes
std::shared_ptr<Label> make_label(std::string text, int w) {
    auto l = std::make_shared<Label>(text);
    l->width = w;
    l->fixed_width = w; // Use integer width
    return l;
}

    // --- Simulator ---
    class Simulator : public Border {
    public:
        using Border::Border; 
        
        std::vector<std::shared_ptr<ProgressBar>> cpus;
        std::vector<std::shared_ptr<Label>> percents;
        std::shared_ptr<ProgressBar> mem;
        std::shared_ptr<Sparkline> spark;
        std::shared_ptr<TableScrollable> list_view;
        std::shared_ptr<StatusBar> top_status;
        
        void tick() {
            // Update simulation
            static float time = 0;
            time += 0.1f;
            
            for (size_t i=0; i<cpus.size(); ++i) {
                // Skewed random usage
                float val = 0.3f + 0.3f * std::sin(time + i * 1.5f) + 0.2f * ((std::rand()%100)/100.0f);
                if (val > 1.0f) val = 1.0f;
                if (val < 0.0f) val = 0.0f;
                
                cpus[i]->value = val;
                
                // Dynamic Color
                if (val < 0.5f) cpus[i]->color = Theme::current().success; // Green
                else if (val < 0.8f) cpus[i]->color = Theme::current().warning; // Yellow
                else cpus[i]->color = Theme::current().error; // Red
                
                // Update Text
                std::stringstream ss; 
                ss << std::fixed << std::setprecision(1) << " " << (val * 100.0f) << "%";
                if (i < percents.size()) percents[i]->set_text(ss.str());
            }
            
            if (mem) {
                mem->value = 0.4f + 0.05f * std::cos(time * 0.3f);
            }
            
            if (spark) {
                spark->data.push_back(cpus[0]->value);
                if (spark->data.size() > 200) spark->data.erase(spark->data.begin());
            }

            // Update Top Status
            if (top_status) {
                top_status->sections.clear();
                
                int uptime_secs = (int)(time * 10);
                int h = uptime_secs / 3600;
                int m = (uptime_secs % 3600) / 60;
                int s = uptime_secs % 60;
                
                std::stringstream ss_up;
                ss_up << " Uptime: " << std::setfill('0') << std::setw(2) << h << ":" 
                      << std::setw(2) << m << ":" << std::setw(2) << s;
                top_status->add_section(ss_up.str());
                
                float l1 = 0.5f + 0.2f * std::sin(time * 0.1f) + 0.1f * ((std::rand()%10)/10.0f);
                float l5 = l1 * 0.9f + 0.05f * std::cos(time * 0.05f);
                float l15 = l5 * 0.8f + 0.1f;
                
                std::stringstream ss_load;
                ss_load << " Load average: " << std::fixed << std::setprecision(2) << l1 << " " << l5 << " " << l15;
                top_status->add_section(ss_load.str());
            }

            // Update List View (Dynamic values)
            if (list_view) {
                for (auto& row : list_view->rows) {
                    if (row.size() > 6) {
                        // Randomly fluctuate CPU% and MEM%
                        float cpu_val = std::stof(row[5]);
                        cpu_val += ((std::rand() % 10) - 5) / 10.0f;
                        if (cpu_val < 0) cpu_val = 0;
                        if (cpu_val > 99.9) cpu_val = 99.9;
                        
                        std::stringstream ss_c; ss_c << std::fixed << std::setprecision(1) << cpu_val;
                        row[5] = ss_c.str();

                        float mem_val = std::stof(row[6]);
                        mem_val += ((std::rand() % 4) - 2) / 10.0f;
                        if (mem_val < 0) mem_val = 0;
                        
                        std::stringstream ss_m; ss_m << std::fixed << std::setprecision(1) << mem_val;
                        row[6] = ss_m.str();
                    }
                }
            }
        }

        bool on_event(const Event& e) override {
            return Border::on_event(e);
        }
    };

    class HtopRoot : public Container {
    public:
        App* app = nullptr;
        std::shared_ptr<Simulator> sim;
        std::shared_ptr<MenuDialog> menu;

        void layout() override {
            if (sim) {
                sim->x = x;
                sim->y = y;
                sim->width = width;
                sim->height = height;
                sim->layout();
            }
        }

        bool on_event(const Event& e) override {
            // Menu handled by App now


             if (e.type == EventType::Key) {
                  // 's' or 'm' to open Settings/Menu
                  if (e.key == 's' || e.key == 'm') {
                      if (menu && !menu->is_open) {
                          // Center menu
                          int mw = menu->width;
                          int mh = menu->height;
                          if (mw == 0) mw = 20; // fallback
                          if (mh == 0) mh = 10;
                          
                          int mx = x + (width - mw) / 2;
                          int my = y + (height - mh) / 2;
                          if (mx < 0) mx = 0;
                          if (my < 0) my = 0;
                          
                          if (app) menu->open(mx, my);
                          return true;
                      }
                  }
                  // Handle 'q' to quit (handled by App)
             }

            if (sim) {
                return sim->on_event(e);
            }
            return false;
        }
    };

int main() {
    App app;
    
    auto main_layout = std::make_shared<Vertical>();

    // Top System Status
    auto top_status = std::make_shared<StatusBar>();
    main_layout->add(top_status);

    // Theme Switcher & System Title
    auto theme_layout = std::make_shared<Horizontal>();
    theme_layout->fixed_height = 1;

    auto btn_toggle = std::make_shared<Button>("Switch Theme", [](){
        static bool is_dark = true;
        is_dark = !is_dark;
        if (is_dark) Theme::set_theme(Theme::Dark());
        else Theme::set_theme(Theme::Light());
    });
    btn_toggle->width = 16;
    btn_toggle->fixed_width = 16;

    theme_layout->add(make_label(" Theme: ", 8));
    theme_layout->add(btn_toggle);
    theme_layout->add(std::make_shared<Label>("")); // Spacer to push button to left if desired, or just let it be
    main_layout->add(theme_layout);

    // --- Header (CPU & Mem) ---
    auto header = std::make_shared<Horizontal>();
    header->height = 8; 
    header->fixed_height = 8; 
    
    // CPU Bars (organized in two vertical panels)
    auto cpu_panel_left = std::make_shared<Vertical>();
    auto cpu_panel_right = std::make_shared<Vertical>();
    
    std::vector<std::shared_ptr<ProgressBar>> cpu_bars;
    std::vector<std::shared_ptr<Label>> cpu_percents;
    
    for (int i=0; i<8; ++i) {
        auto row = std::make_shared<Horizontal>();
        row->height = 1;
        row->fixed_height = 1;
        
        row->add(make_label(std::to_string(i+1) + " ", 3));
        
        auto bar = std::make_shared<ProgressBar>();
        cpu_bars.push_back(bar);
        
        auto percent = make_label(" 0.0%", 7);
        cpu_percents.push_back(percent);
        
        row->add(bar);
        row->add(percent);
        if (i < 4) cpu_panel_left->add(row);
        else cpu_panel_right->add(row);
    }
    
    // Memory & Swap & History
    auto stats_panel = std::make_shared<Vertical>();
    
    auto mem_bar = std::make_shared<ProgressBar>();
    auto swap_bar = std::make_shared<ProgressBar>();
    
    auto mem_row = std::make_shared<Horizontal>(); mem_row->height = 1; mem_row->fixed_height = 1;
    mem_row->add(make_label("Mem ", 5));
    mem_row->add(mem_bar);
    mem_row->add(make_label(" 4.2G/16G", 10));
    
    auto swap_row = std::make_shared<Horizontal>(); swap_row->height = 1; swap_row->fixed_height = 1;
    swap_row->add(make_label("Swp ", 5));
    swap_row->add(swap_bar);
    swap_row->add(make_label(" 0.0G/8G ", 10));
    
    stats_panel->add(mem_row);
    stats_panel->add(swap_row);
    
    auto sp2 = make_label(" CPU History:", 15); sp2->height = 1; sp2->fixed_height = 1;
    stats_panel->add(sp2);
    
    auto spark = std::make_shared<Sparkline>();
    spark->height = 3; 
    spark->fixed_height = 3;
    stats_panel->add(spark);

    header->add(cpu_panel_left);
    header->add(make_label(" ", 2)); // Fixed spacer
    header->add(cpu_panel_right);
    header->add(make_label(" ", 2)); // Fixed spacer
    header->add(stats_panel); // Stats panel takes the rest of the space
    
    main_layout->add(header);
    
    // Process Rows
    auto list_body = std::make_shared<TableScrollable>();
    list_body->columns = {"PID", "USER", "PRI", "NI", "VIRT", "CPU%", "MEM%", "Command"};
    list_body->col_widths = {6, 10, 5, 5, 6, 6, 6, 80};
    
    struct Process { int pid; std::string user; float cpu; float mem; std::string cmd; };
    std::vector<Process> processes = {
        {101, "root", 0.0, 0.1, "/sbin/init"},
        {102, "root", 0.0, 0.0, "[kthreadd]"},
        {103, "root", 0.0, 0.0, "[kworker/u16:0]"},
        {104, "root", 0.0, 0.0, "[rcu_sched]"},
        {150, "root", 0.1, 0.4, "/lib/systemd/systemd-journald"},
        {180, "root", 0.0, 0.2, "/lib/systemd/systemd-udevd"},
        {234, "vscode", 12.5, 4.2, "/usr/bin/code --type=renderer"},
        {245, "vscode", 5.2, 1.1, "/usr/bin/code --type=gpu-process"},
        {250, "vscode", 1.2, 2.5, "/usr/bin/code --type=utility"},
        {290, "message", 0.0, 0.1, "/usr/bin/dbus-daemon --system"},
        {300, "root", 0.5, 0.2, "/usr/bin/dockerd"},
        {310, "root", 0.1, 0.5, "containerd"},
        {404, "user", 0.1, 0.1, "/bin/bash"},
        {500, "user", 24.1, 8.5, "./htop_sim"},
        {501, "user", 0.0, 0.0, "sleep 100"},
        {502, "www", 0.0, 0.5, "nginx: worker process"},
        {503, "www", 0.0, 0.5, "nginx: worker process"},
        {504, "www", 0.0, 0.5, "nginx: worker process"},
        {505, "www", 0.0, 0.5, "nginx: worker process"},
        {600, "mysql", 1.2, 15.4, "/usr/sbin/mysqld"},
        {620, "redis", 0.5, 2.1, "/usr/bin/redis-server"},
        {700, "user", 8.5, 12.4, "python3 training.py"},
        {750, "user", 0.0, 0.1, "top"},
        {751, "user", 0.0, 0.1, "vi configuration.conf"},
        {800, "root", 0.0, 0.1, "/usr/sbin/sshd -D"},
        {805, "user", 0.0, 0.2, "ssh user@remote"},
    };
    
    for (const auto& p : processes) {
        std::vector<std::string> row;
        row.push_back(std::to_string(p.pid));
        row.push_back(p.user);
        row.push_back("20");
        row.push_back("0");
        row.push_back("106M");
        
        std::stringstream cpu_ss; cpu_ss << std::fixed << std::setprecision(1) << p.cpu;
        row.push_back(cpu_ss.str());
        
        std::stringstream mem_ss; mem_ss << std::fixed << std::setprecision(1) << p.mem;
        row.push_back(mem_ss.str());
        
        row.push_back(p.cmd);
        
        list_body->rows.push_back(row);
    }
    main_layout->add(list_body);
    
    // --- Footer Shortcut Bar ---
    auto footer = std::make_shared<ShortcutBar>();
    footer->spacing = 1; // More compact spacing
    footer->add("F1", "Help");
    footer->add("F2", "Setup");
    footer->add("F3", "Search");
    footer->add("F4", "Filter");
    footer->add("F5", "Tree");
    footer->add("F6", "Sort");
    footer->add("F7", "Nice-");
    footer->add("F8", "Nice+");
    footer->add("F9", "Kill"); // Added F9
    footer->add("F10", "Quit");
    
    main_layout->add(footer);
    
    // --- Simulator Setup ---
    auto sim_root = std::make_shared<Simulator>(BorderStyle::Rounded); 
    sim_root->cpus = cpu_bars;
    sim_root->percents = cpu_percents;
    sim_root->mem = mem_bar;
    sim_root->spark = spark;
    sim_root->list_view = list_body; 
    sim_root->top_status = top_status;
    
    sim_root->add(main_layout);
    
    // --- Menu Setup ---
    auto menu = std::make_shared<MenuDialog>(&app);
    menu->set_style(BorderStyle::Double);
    menu->set_title(" Settings ");
    menu->shadow = true;
    
    menu->items.push_back({"Display Options", [](){}});
    menu->items.push_back({"Colors", [](){}});
    menu->items.push_back({"Columns", [](){}});
    menu->items.push_back({"Quit", [](){ App::quit(); }});

    // --- Root Setup ---
    auto root = std::make_shared<HtopRoot>();
    root->app = &app;
    root->sim = sim_root;
    root->menu = menu;
    
    root->add(sim_root);
    
    // --- Timer ---
    app.add_timer(200, [&](){
        sim_root->tick();
    });

    app.register_exit_key('q');
    app.run(root);
}
