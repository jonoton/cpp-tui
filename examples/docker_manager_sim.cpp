#include "cpptui.hpp"
#include <vector>
#include <string>
#include <deque>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace cpptui;

enum class ContainerState {
    Running,
    Stopped,
    Paused,
    Restarting
};

struct ContainerInfo {
    std::string id;
    std::string name;
    std::string image;
    ContainerState state;
    float cpu_usage; // 0-100%
    float mem_usage; // 0-100%
    std::string ports;
    std::string status_msg;
};

class DockerManager : public Border {
public:
    std::vector<ContainerInfo> containers;
    std::string filter_state = "ALL";
    bool auto_refresh = true;
    
    // Live log simulation line
    std::string live_log_line = "Waiting for logs...";
    
    // UI Refs
    std::shared_ptr<TableScrollable> container_table;
    std::shared_ptr<Label> stats_label;
    std::shared_ptr<Label> log_label;
    std::shared_ptr<Notification> notification;

    DockerManager() : Border(BorderStyle::Double) {
        set_title(" Docker Container Manager ", Alignment::Center);
        
        // Init Dummy Data
        containers = {
            {"1a2b3c", "web-frontend", "nginx:latest", ContainerState::Running, 0.5f, 2.5f, "0.0.0.0:80->80/tcp", "Up 2 hours"},
            {"4d5e6f", "api-gateway", "traefik:v2", ContainerState::Running, 1.2f, 4.0f, "0.0.0.0:443->443/tcp", "Up 2 hours"},
            {"7g8h9i", "db-primary", "postgres:14", ContainerState::Running, 4.5f, 15.2f, "5432/tcp", "Up 5 days"},
            {"0j1k2l", "cache-redis", "redis:alpine", ContainerState::Running, 0.1f, 0.8f, "6379/tcp", "Up 5 days"},
            {"3m4n5o", "worker-job", "python:3.9-slim", ContainerState::Stopped, 0.0f, 0.0f, "", "Exited (0) 10 min ago"},
            {"6p7q8r", "dev-sandbox", "ubuntu:22.04", ContainerState::Paused, 0.0f, 4.2f, "", "Paused"},
            {"9s0t1u", "metrics-prom", "prom/prometheus", ContainerState::Running, 2.1f, 6.5f, "9090/tcp", "Up 1 day"},
            {"2v3w4x", "grafana-ui", "grafana/grafana", ContainerState::Running, 1.5f, 5.8f, "3000/tcp", "Up 1 day"}
        };
    }

    void tick() {
        if (!auto_refresh) return;

        static int tick_count = 0;
        tick_count++;

        for (auto& c : containers) {
            if (c.state == ContainerState::Running) {
                // Fluctuate resources
                float noise_cpu = ((rand()%100) - 50) / 100.0f; // +/- 0.5
                c.cpu_usage += noise_cpu;
                if (c.cpu_usage < 0) c.cpu_usage = 0;

                float noise_mem = ((rand()%100) - 50) / 200.0f; // +/- 0.25
                c.mem_usage += noise_mem;
                if (c.mem_usage < 0) c.mem_usage = 0;
            } else {
                c.cpu_usage = 0;
            }
        }
        
        if (tick_count % 5 == 0) { // Every 5 ticks update table explicitly
             refresh_table();
        }

        // Simulate random log from a running container
        if (tick_count % 10 == 0) {
            std::vector<ContainerInfo*> running;
            for(auto& c : containers) if(c.state == ContainerState::Running) running.push_back(&c);
            
            if (!running.empty()) {
                auto* lucky = running[rand() % running.size()];
                std::stringstream ss;
                ss << "[" << lucky->name << "] " << "GET /api/v1/resource " << (rand()%400 + 200) << " " << (rand()%100) << "ms";
                live_log_line = ss.str();
                if (log_label) log_label->set_text(live_log_line);
            }
        }
    }

    void action_start() { 
        update_selected_state(ContainerState::Running); 
        if(notification) notification->show("Container " + target_id + " started", Notification::Type::Success);
    }
    void action_stop() { 
        update_selected_state(ContainerState::Stopped); 
        if(notification) notification->show("Container " + target_id + " stopped", Notification::Type::Warning);
    }
    void action_restart() { 
         // Simulate restart logic
         update_selected_state(ContainerState::Restarting);
         update_selected_state(ContainerState::Running);
         if(notification) notification->show("Container " + target_id + " restarted", Notification::Type::Info);
    }
    
    // Container selection via ID input field (target_id below)
    
    std::string target_id;

    void update_selected_state(ContainerState new_state) {
        for(auto& c : containers) {
            if (c.id.find(target_id) != std::string::npos && !target_id.empty()) {
                c.state = new_state;
                if (new_state == ContainerState::Running) c.status_msg = "Up 1 second";
                if (new_state == ContainerState::Stopped) c.status_msg = "Exited (0) just now";
            }
        }
        refresh_table();
    }

    std::string state_to_str(ContainerState s) {
        switch(s) {
            case ContainerState::Running: return "Running";
            case ContainerState::Stopped: return "Stopped";
            case ContainerState::Paused: return "Paused";
            case ContainerState::Restarting: return "Restarting";
            default: return "Unknown";
        }
    }

    void refresh_table() {
        if (!container_table) return;
        container_table->rows.clear();

        int running_cnt = 0;

        for (const auto& c : containers) {
            if (c.state == ContainerState::Running) running_cnt++;

            if (filter_state != "ALL") {
                if (filter_state == "RUNNING" && c.state != ContainerState::Running) continue;
                if (filter_state == "STOPPED" && c.state != ContainerState::Stopped) continue;
            }

            std::vector<std::string> row;
            row.push_back(c.id);
            row.push_back(c.name);
            row.push_back(c.image);
            row.push_back(state_to_str(c.state));
            
            std::stringstream cpu; cpu << std::fixed << std::setprecision(2) << c.cpu_usage << "%";
            row.push_back(cpu.str());
            
            std::stringstream mem; mem << std::fixed << std::setprecision(1) << c.mem_usage << "%";
            row.push_back(mem.str());
            
            row.push_back(c.ports);
            row.push_back(c.status_msg);
            
            container_table->rows.push_back(row);
        }
        
        if (stats_label) {
            stats_label->set_text("Running: " + std::to_string(running_cnt) + " / " + std::to_string(containers.size()));
        }
    }
};

int main() {
    App app;
    auto root = std::make_shared<Vertical>();
    auto manager = std::make_shared<DockerManager>();

    // Notification System
    auto notif = std::make_shared<Notification>();
    notif->position = Notification::Position::BottomCenter;
    manager->notification = notif;

    // Header
    root->add(manager); // The Manager IS the Border

    // Inside the manager border, we want layout.
    // Since we inherited from Border, we can add children to 'this' (manager)
    // But we are in main, so we add to manager via pointer.
    
    auto main_layout = std::make_shared<Vertical>();
    
    // Toolbar
    auto toolbar = std::make_shared<Horizontal>();
    toolbar->fixed_height = 3;
    
    auto tb_box = std::make_shared<Border>(BorderStyle::Single);
    tb_box->set_title(" Management ", Alignment::Left);
    auto tb_in = std::make_shared<Horizontal>();
    
    tb_in->add(std::make_shared<Label>("Target ID: "));
    auto id_in = std::make_shared<SearchInput>(&app);
    id_in->fixed_width = 12;
    id_in->placeholder = "Search ID...";
    
    // Populate suggestions
    for(const auto& c : manager->containers) {
        id_in->suggestions.push_back(c.id);
        id_in->suggestions.push_back(c.name);
    }

    id_in->on_search = [manager](const std::string& val){
        manager->target_id = val;
    };
    // Also allow on_change for immediate feedback if desired, but search usually implies selection
    id_in->on_change = [manager](const std::string& val) {
         manager->target_id = val;
    };
    
    tb_in->add(id_in);
    
    tb_in->add(std::make_shared<Label>("  "));
    
    auto btn_start = std::make_shared<Button>(" Start ", [manager](){ manager->action_start(); });
    btn_start->bg_color = Theme::current().success;
    tb_in->add(btn_start);
    
    auto btn_stop = std::make_shared<Button>(" Stop ", [manager](){ manager->action_stop(); });
    btn_stop->bg_color = Theme::current().error;
    tb_in->add(btn_stop);
    
    auto btn_restart = std::make_shared<Button>(" Restart ", [manager](){ manager->action_restart(); });
    btn_restart->bg_color = Theme::current().warning;
    tb_in->add(btn_restart);

    tb_in->add(std::make_shared<VerticalSpacer>(1));
    tb_in->add(std::make_shared<Label>("  Filter: "));
    auto filter_drp = std::make_shared<Dropdown>(&app);
    filter_drp->set_options({"ALL", "RUNNING", "STOPPED"});
    filter_drp->selected_index = 0;
    filter_drp->fixed_width = 10;
    filter_drp->on_change = [manager](int idx, std::string val){
        manager->filter_state = val;
        manager->refresh_table();
    };
    tb_in->add(filter_drp);

    tb_box->add(tb_in);
    toolbar->add(tb_box);
    main_layout->add(toolbar);

    // Table
    auto table_panel = std::make_shared<Vertical>();
    auto table = std::make_shared<TableScrollable>();
    table->columns = {"ID", "Name", "Image", "State", "CPU", "MEM", "Ports", "Status"};
    table->col_widths = {8, 15, 15, 10, 8, 8, 18, 20};
    manager->container_table = table;
    manager->refresh_table();
    
    table_panel->add(table);
    main_layout->add(table_panel); // Allow it to expand

    // Footer / Stats
    auto footer = std::make_shared<Horizontal>();
    footer->fixed_height = 1;
    
    auto stats = std::make_shared<Label>("Running: 0/0");
    manager->stats_label = stats;
    footer->add(stats);
    
    footer->add(std::make_shared<Label>(" | Last Log: "));
    auto log_l = std::make_shared<Label>("...");
    manager->log_label = log_l;
    footer->add(log_l);

    main_layout->add(footer);
    
    manager->add(main_layout);
    root->add(std::make_shared<Label>("Press 'q' to quit. Enter Target ID to control containers.", Color{100,100,100}));
    
    // Add Notification widget to tree so it gets render calls
    root->add(notif);

    app.add_timer(500, [manager](){
        manager->tick();
    });

    app.register_exit_key('q');
    app.run(root);

    return 0;
}
