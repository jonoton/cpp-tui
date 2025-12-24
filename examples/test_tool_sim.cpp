#include "cpptui.hpp"
#include <vector>
#include <string>
#include <deque>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <map>

using namespace cpptui;

enum class TestStatus {
    Pending,
    Running,
    Passed,
    Failed,
    Skipped
};

struct TestItem {
    int id;
    std::string name;
    std::string suite;
    TestStatus status = TestStatus::Pending;
    std::string output_log;
    float duration_ms = 0;
};

class TestRunner : public Vertical {
public:
    std::vector<TestItem> tests;
    int current_running_index = -1;
    bool is_running = false;
    
    // Simulation settings
    int simulation_speed_ms = 100; // Delay between test steps
    bool stop_on_fail = false;
    std::string filter_status = "ALL"; 

    // UI Refs
    std::shared_ptr<TableScrollable> test_table;
    std::shared_ptr<ScrollableVertical> output_view;
    std::shared_ptr<ProgressBar> total_progress;
    std::shared_ptr<Label> status_label;
    
    // Stats
    int count_pass = 0;
    int count_fail = 0;
    int count_skip = 0;

    TestRunner() {
        // Initialize Dummy Tests
        for (int i=1; i<=30; ++i) {
            TestItem t;
            t.id = i;
            t.name = "Test_Case_" + std::to_string(i);
            t.suite = (i%3 == 0) ? "Integration" : (i%2==0 ? "Unit" : "E2E");
            tests.push_back(t);
        }
    }

    void start_all() {
        reset_all();
        is_running = true;
        current_running_index = 0;
    }

    void reset_all() {
        is_running = false;
        current_running_index = -1;
        count_pass = 0; count_fail = 0; count_skip = 0;
        for(auto& t : tests) {
            t.status = TestStatus::Pending;
            t.output_log = "";
            t.duration_ms = 0;
        }
        refresh_table();
        update_stats();
    }

    void tick() {
        if (!is_running) return;
        
        if (current_running_index >= 0 && current_running_index < tests.size()) {
            auto& t = tests[current_running_index];
            
            if (t.status == TestStatus::Pending) {
                t.status = TestStatus::Running;
                t.output_log += "[RUN] Starting " + t.name + "...\n";
                refresh_table();
                // Update log data
            } else if (t.status == TestStatus::Running) {
                // Simulate work
                t.duration_ms += (rand() % 50 + 10);
                
                // Finish test randomly
                if (rand() % 10 > 2) { // 70% chance to finish this tick
                    bool pass = (rand() % 100) > 15; // 15% fail rate
                    if (pass) {
                        t.status = TestStatus::Passed;
                        t.output_log += "[PASS] Test passed successfully.\n";
                        count_pass++;
                    } else {
                        t.status = TestStatus::Failed;
                        t.output_log += "[FAIL] Assertion error at line " + std::to_string(rand()%100) + "\n";
                        t.output_log += "       Expected: true, Actual: false\n";
                        count_fail++;
                    }
                    
                    current_running_index++; // Move next
                    refresh_table();
                    update_stats();

                    if (stop_on_fail && !pass) {
                        is_running = false;
                    }
                }
            }
        } else {
            is_running = false;
            update_stats();
        }
    }

    Color get_status_color(TestStatus s) {
        switch(s) {
            case TestStatus::Passed: return Theme::current().success;
            case TestStatus::Failed: return Theme::current().error;
            case TestStatus::Running: return Theme::current().warning; // Yellow for running
            case TestStatus::Skipped: return {100, 100, 100};
            default: return Theme::current().foreground;
        }
    }
    
    std::string get_status_str(TestStatus s) {
        switch(s) {
            case TestStatus::Passed: return "PASS";
            case TestStatus::Failed: return "FAIL";
            case TestStatus::Running: return "RUN...";
            case TestStatus::Pending: return "-";
            case TestStatus::Skipped: return "SKIP";
            default: return "?";
        }
    }

    void refresh_table() {
        if (!test_table) return;
        test_table->rows.clear();
        
        for (const auto& t : tests) {
             // Filter Logic
             std::string s_str = get_status_str(t.status);
             if (filter_status != "ALL") {
                 if (filter_status == "PASS" && t.status != TestStatus::Passed) continue;
                 if (filter_status == "FAIL" && t.status != TestStatus::Failed) continue;
                 // simplified filter
             }
            
             std::vector<std::string> row;
             row.push_back(std::to_string(t.id));
             row.push_back(t.suite);
             row.push_back(t.name);
             row.push_back(s_str);
             
             std::stringstream ss; ss << std::fixed << std::setprecision(0) << t.duration_ms << "ms";
             row.push_back(ss.str());
             
             test_table->rows.push_back(row);
        }
    }

    void update_stats() {
        if (status_label) {
            std::stringstream ss;
            ss << "Total: " << tests.size() 
               << " | Passed: " << count_pass 
               << " | Failed: " << count_fail 
               << " | Pending: " << (tests.size() - count_pass - count_fail - count_skip);
            status_label->set_text(ss.str());
        }
        if (total_progress) {
            float progress = (float)(count_pass + count_fail + count_skip) / (float)tests.size();
            total_progress->value = progress;
            if (count_fail > 0) total_progress->color = Theme::current().error;
            else if (progress >= 1.0f) total_progress->color = Theme::current().success;
            else total_progress->color = Theme::current().primary;
        }
    }

    void show_output_for(int row_index) {
        if (!output_view) return;
        output_view->clear_children();
        
        // Map row index to test (assuming simplistic mapping where row index matches test index if no filter)
        // For accurate mapping with filters, we'd need more logic. Assuming ALL filter for Demo simplicity.
        if (row_index >= 0 && row_index < tests.size()) {
            const auto& t = tests[row_index];
            output_view->add(std::make_shared<Label>("Output for: " + t.name, Theme::current().primary));
            output_view->add(std::make_shared<VerticalSpacer>(1));
            
            // split log by newline
            std::stringstream ss(t.output_log);
            std::string line;
            while(std::getline(ss, line)) {
                output_view->add(std::make_shared<Label>(line));
            }
            if (t.output_log.empty()) {
                output_view->add(std::make_shared<Label>("(No output available)"));
            }
        }
    }
};

int main() {
    App app;
    auto root = std::make_shared<Vertical>();
    auto runner = std::make_shared<TestRunner>();

    // Header
    auto header = std::make_shared<Label>(" TEST RUNNER SIMULATION ", Theme::current().primary);
    header->fixed_height = 1;
    root->add(header);

    // Toolbar
    auto toolbar = std::make_shared<Horizontal>();
    toolbar->fixed_height = 3;
    
    auto box_controls = std::make_shared<Border>(BorderStyle::Single);
    box_controls->set_title(" Controls ", Alignment::Left);
    auto h_ctrl = std::make_shared<Horizontal>();
    
    auto btn_run = std::make_shared<Button>("Run All Tests", [runner](){
        runner->start_all();
    });
    btn_run->bg_color = Theme::current().success;
    h_ctrl->add(btn_run);
    
    h_ctrl->add(std::make_shared<Label>("  "));

    auto btn_reset = std::make_shared<Button>("Reset", [runner](){
        runner->reset_all();
    });
    h_ctrl->add(btn_reset);

    h_ctrl->add(std::make_shared<Label>("  Speed: "));
    auto speed_in = std::make_shared<NumberInput>(100);
    speed_in->on_change = [runner](int val){
        runner->simulation_speed_ms = val;
    };
    speed_in->fixed_width = 6;
    h_ctrl->add(speed_in);

    box_controls->add(h_ctrl);
    toolbar->add(box_controls);
    root->add(toolbar);

    // Main Content: Split Table (Left) and Output (Right)
    auto split = std::make_shared<Horizontal>();
    
    // Left: Test List
    auto list_box = std::make_shared<Border>(BorderStyle::Rounded);
    list_box->set_title(" Test Suite ", Alignment::Center);
    
    auto table = std::make_shared<TableScrollable>();
    table->columns = {"ID", "Suite", "Test Name", "Status", "Time"};
    table->col_widths = {4, 12, 20, 8, 8};
    runner->test_table = table;
    table->on_change = [runner](int idx){
        // Show output on selection
        runner->show_output_for(idx);
    };
    runner->refresh_table();
    
    // Live output update
    
    list_box->add(table);
    split->add(list_box);

    // Right: Output
    auto out_box = std::make_shared<Border>(BorderStyle::Rounded);
    out_box->set_title(" Console Output ", Alignment::Left);
    auto sv_out = std::make_shared<ScrollableVertical>();
    runner->output_view = sv_out;
    
    out_box->add(sv_out);
    split->add(out_box);
    
    root->add(split);

    // Bottom: Status Bar
    auto status_bar = std::make_shared<Horizontal>();
    status_bar->fixed_height = 1;

    auto lbl_stat = std::make_shared<Label>("Ready.");
    runner->status_label = lbl_stat;
    status_bar->add(lbl_stat);
    
    auto pb = std::make_shared<ProgressBar>();
    runner->total_progress = pb;
    status_bar->add(pb);

    root->add(status_bar);

    // Timer
    app.add_timer(100, [runner](){
        runner->tick();
        // Update output only if the currently running test IS the selected test
        // OR if the user hasn't selected anything specific (but default is 0).
        // Better: Only refresh output if the running test is the one currently being viewed.
        if (runner->is_running && runner->current_running_index >=0) {
             if (runner->test_table && runner->test_table->selected_index == runner->current_running_index) {
                 runner->show_output_for(runner->current_running_index);
             }
        }
    });

    app.register_exit_key('q');
    app.run(root);

    return 0;
}
