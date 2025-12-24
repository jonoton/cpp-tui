#include "cpptui.hpp"
#include <string>
#include <vector>

using namespace cpptui;
#include <iomanip> // for std::fixed and std::setprecision

// =============================================================================
// Helper functions to reduce boilerplate
// =============================================================================

/// @brief Create a styled section label with a vertical spacer before it
inline void add_section(std::shared_ptr<Container> container, const std::string& title) {
    container->add(std::make_shared<VerticalSpacer>(1));
    container->add(std::make_shared<Label>(title, Theme::current().primary));
}

/// @brief Create a theme button with fixed width
inline std::shared_ptr<Button> make_theme_button(const std::string& label, std::function<void()> on_click, int width = 8) {
    auto btn = std::make_shared<Button>(label, on_click);
    btn->fixed_width = width;
    return btn;
}

/// @brief Create a notification demo button
inline std::shared_ptr<Button> make_notif_button(const std::string& label, 
                                                  std::shared_ptr<Notification> notif,
                                                  Notification::Type type,
                                                  const std::string& message,
                                                  Color bg_color = Color()) {
    auto btn = std::make_shared<Button>(label, [notif, message, type](){
        notif->show(message, type, 3000);
    });
    btn->fixed_width = 10;
    if (!bg_color.is_default) btn->bg_color = bg_color;
    return btn;
}

/// @brief Create a labeled spinner with separator for spinner demos
inline void add_spinner_with_label(std::shared_ptr<Horizontal> row, 
                                   const std::string& label_text, int label_width,
                                   std::shared_ptr<Spinner> spinner,
                                   bool add_separator = true) {
    auto lbl = std::make_shared<Label>(label_text);
    lbl->fixed_width = label_width;
    row->add(lbl);
    row->add(spinner);
    if (add_separator) {
        auto sep = std::make_shared<Label>("  ");
        sep->fixed_width = 2;
        row->add(sep);
    }
}

int main() {
    App app;
    
    // Shared pointer for animation access

    std::shared_ptr<Spinner> sp_dots_det, sp_line_det, sp_pie_det, sp_bounce_bar_det, sp_bounce_circle_det;
    std::shared_ptr<Label> lbl_dots, lbl_line, lbl_pie, lbl_bar, lbl_circle;
    
    auto app_container = std::make_shared<Vertical>();
    auto root = app_container; 

    
    // Header
    auto header = std::make_shared<Label>("cpp-tui Widget Showcase", Color{255, 200, 100});
    header->fixed_height = 1;
    root->add(header);
    
    // Theme Selector
    auto theme_layout = std::make_shared<Horizontal>();
    theme_layout->fixed_height = 1;
    
    theme_layout->add(std::make_shared<Label>("Theme: "));
    theme_layout->add(make_theme_button("Dark", [](){ Theme::set_theme(Theme::Dark()); }));
    theme_layout->add(make_theme_button("Light", [](){ Theme::set_theme(Theme::Light()); }));
    theme_layout->add(make_theme_button("Nord", [](){ Theme::set_theme(Theme::Nord()); }));
    theme_layout->add(make_theme_button("Tokyo", [](){ Theme::set_theme(Theme::TokyoNight()); }));
    theme_layout->add(make_theme_button("Solar", [](){ Theme::set_theme(Theme::SolarizedLight()); }));
    
    root->add(theme_layout);


    // --- Tabbed Interface as Main Organizer ---
    auto tabs = std::make_shared<Tabs>();
    tabs->fixed_height = 0; // Fill rest


    // =========================================================================
    // TAB 1: INPUTS - All Input Widgets
    // =========================================================================
    auto inputs_layout = std::make_shared<Vertical>();
    {
        auto box = std::make_shared<Border>(BorderStyle::Rounded, Color{100, 180, 255});
        box->set_title(" Input Controls ", Alignment::Center);
        auto sv = std::make_shared<ScrollableVertical>();
        box->add(sv);
        
        // --- Text Inputs ---
        sv->add(std::make_shared<Label>("[ Text Input ]", Theme::current().primary));
        
        auto name_row = std::make_shared<Horizontal>();
        name_row->fixed_height = 1;
        auto name_label = std::make_shared<Label>("Name: ");
        name_label->fixed_width = 8;
        name_row->add(name_label);
        auto name_input = std::make_shared<Input>();
        name_input->placeholder = "Enter your name...";
        name_row->add(name_input);
        sv->add(name_row);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        auto email_row = std::make_shared<Horizontal>();
        email_row->fixed_height = 1;
        auto email_label = std::make_shared<Label>("Email: ");
        email_label->fixed_width = 8;
        email_row->add(email_label);
        auto mail = std::make_shared<Input>();
        mail->placeholder = "user@example.com";
        mail->regex_pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
        email_row->add(mail);
        sv->add(email_row);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Slider ---
        sv->add(std::make_shared<Label>("[ Slider ]", Theme::current().primary));
        auto slider_h = std::make_shared<Horizontal>();
        auto slider_lbl = std::make_shared<Label>("Volume: 50");
        slider_lbl->fixed_width = 15;
        // Test automatic tooltip on label
        slider_lbl->set_tooltip("Current volume level");
        
        auto slider1 = std::make_shared<Slider>(50.0, 0.0, 100.0);
        slider1->on_change = [slider_lbl](double v) {
            slider_lbl->set_text("Volume: " + std::to_string((int)v));
        };
        // Test automatic tooltip on slider
        slider1->set_tooltip("Drag to adjust volume");
        
        slider_h->add(slider_lbl);
        slider_h->add(slider1);
        slider_h->fixed_height = 1;
        sv->add(slider_h);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Toggle Switch ---
        sv->add(std::make_shared<Label>("[ Toggle Switch ]", Theme::current().primary));
        auto toggle = std::make_shared<ToggleSwitch>("Enable Hyper-Speed");
        sv->add(toggle);

        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Checkboxes with Progress ---
        sv->add(std::make_shared<Label>("[ Checkboxes with Progress ]", Theme::current().primary));
        auto pbar = std::make_shared<ProgressBar>();
        pbar->value = 0.0f;
        
        auto cb1 = std::make_shared<Checkbox>("Design");
        auto cb2 = std::make_shared<Checkbox>("Implementation");
        auto cb3 = std::make_shared<Checkbox>("Testing");
        
        std::weak_ptr<Checkbox> w_cb1 = cb1;
        std::weak_ptr<Checkbox> w_cb2 = cb2;
        std::weak_ptr<Checkbox> w_cb3 = cb3;
        std::weak_ptr<ProgressBar> w_pbar = pbar;
        
        auto update_progress = [w_cb1, w_cb2, w_cb3, w_pbar](bool) {
            auto cb1 = w_cb1.lock();
            auto cb2 = w_cb2.lock();
            auto cb3 = w_cb3.lock();
            auto pbar = w_pbar.lock();
            if (!cb1 || !cb2 || !cb3 || !pbar) return;

            float total = 3.0f;
            float count = 0.0f;
            if (cb1->is_checked()) count++;
            if (cb2->is_checked()) count++;
            if (cb3->is_checked()) count++;
            pbar->value = count / total;
        };
        
        cb1->on_change = update_progress;
        cb2->on_change = update_progress;
        cb3->on_change = update_progress;
        
        sv->add(cb1);
        sv->add(cb2);
        sv->add(cb3);
        sv->add(std::make_shared<Label>("Progress:"));
        sv->add(pbar);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Radio Buttons ---
        sv->add(std::make_shared<Label>("[ Radio Buttons ]", Theme::current().primary));
        auto radio = std::make_shared<RadioSet>();
        radio->set_options({"Easy", "Medium", "Hard", "Nightmare"});
        sv->add(radio);

        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Dropdown ---
        sv->add(std::make_shared<Label>("[ Dropdown ]", Theme::current().primary));
        auto drop = std::make_shared<Dropdown>(&app);
        drop->set_options({"Select Option...", "Apple", "Banana", "Cherry", "Date", "Elderberry"});
        sv->add(drop);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Number Input ---
        sv->add(std::make_shared<Label>("[ Number Input ]", Theme::current().primary));
        
        sv->add(std::make_shared<Label>("Buttons Right:"));
        auto num = std::make_shared<NumberInput>(10);
        num->step = 5;
        num->set_position(NumberInput::ButtonPos::Right);
        sv->add(num);
        
        sv->add(std::make_shared<Label>("Buttons Left:"));
        auto num2 = std::make_shared<NumberInput>(5);
        num2->set_position(NumberInput::ButtonPos::Left);
        sv->add(num2);

        sv->add(std::make_shared<Label>("Buttons Split:"));
        auto num3 = std::make_shared<NumberInput>(0);
        num3->set_position(NumberInput::ButtonPos::Split);
        sv->add(num3);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Checkbox List ---
        sv->add(std::make_shared<Label>("[ Checkbox List ]", Theme::current().primary));
        auto checklist = std::make_shared<CheckboxList>();
        checklist->set_options({"Engine", "Transmission", "Wheels", "Nitro", "Spoiler"});
        sv->add(checklist);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Button ---
        sv->add(std::make_shared<Label>("[ Button ]", Theme::current().primary));
        auto result_label = std::make_shared<Label>("");
        
        auto btn = std::make_shared<Button>("Submit Form", [name_input, mail, result_label](){
            result_label->set_text("Submitted: " + name_input->get_value() + " | " + mail->get_value());
        });
        
        // Test custom tooltip (widget-owned)
        auto btn_tip = std::make_shared<Tooltip>("Submits the form data");
        btn_tip->position = Tooltip::Position::Bottom;
        btn->set_tooltip(btn_tip);
        
        sv->add(btn);
        sv->add(result_label);
        
        inputs_layout->add(box);
    }
    tabs->add_tab("Inputs", inputs_layout);


    // =========================================================================
    // TAB 3: TABLES - Paginated & Scrollable
    // =========================================================================
    auto tables_layout = std::make_shared<Horizontal>();
    {
        // Left: Paginated Table
        {
            auto table_box = std::make_shared<Border>(BorderStyle::Single);
            table_box->set_title(" Paginated Table ", Alignment::Center);
            
            auto table = std::make_shared<TablePaginated>();
            table->columns = {"ID", "User", "Role", "Status"};
            table->rows = {
                {"01", "Alice", "Admin", "Online"},
                {"02", "Bob", "User", "Offline"},
                {"03", "Charlie", "User", "Busy"},
                {"04", "Dave", "Guest", "Online"},
                {"05", "Eve", "Admin", "Online"},
                {"06", "Frank", "User", "Away"},
                {"07", "Grace", "Guest", "Offline"},
                {"08", "Heidi", "Admin", "Busy"},
                {"09", "Ivan", "User", "Online"},
                {"10", "Judy", "User", "Offline"},
                {"11", "Karl", "Guest", "Away"},
                {"12", "Leo", "Admin", "Online"},
                {"13", "Mallory", "User", "Online"},
                {"14", "Niaj", "User", "Busy"},
                {"15", "Olivia", "Guest", "Offline"},
                {"16", "Peggy", "Admin", "Online"},
                {"17", "Quentin", "User", "Away"},
                {"18", "Rupert", "User", "Online"},
                {"19", "Sybil", "Guest", "Offline"},
                {"20", "Ted", "Admin", "Busy"},
            };
            table->auto_page_size = true; 
            table->fixed_height = 0;
            
            table_box->add(table);
            tables_layout->add(table_box);
        }

        // Right: Scrollable List
        {
            auto scroll_box = std::make_shared<Border>(BorderStyle::Single);
            scroll_box->set_title(" Scrollable Table ", Alignment::Center);
            
            auto list = std::make_shared<TableScrollable>();
            list->columns = {"PID", "USER", "PRI", "VIRT", "Command"};
            list->col_widths = {6, 8, 4, 8, 15};
            
            for(int i=1; i<=50; ++i) {
                 std::string pid = std::to_string(1000 + i);
                 std::string user = (i % 3 == 0) ? "root" : "vscode";
                 std::string cmd = (i % 2 == 0) ? "/usr/bin/code" : "bash";
                 
                 list->rows.push_back({
                     pid, user, "20", "1024M", cmd
                 });
            }

            
            scroll_box->add(list);
            tables_layout->add(scroll_box);
        }
    }
    tabs->add_tab("Tables", tables_layout);


    // =========================================================================
    // TAB 4: TREE - TreeView Widget
    // =========================================================================
    auto tree_layout = std::make_shared<Vertical>();
    {
        auto box = std::make_shared<Border>(BorderStyle::Rounded, Color{50, 200, 120});
        box->set_title(" File System Tree ", Alignment::Left);
        
        auto tree = std::make_shared<TreeView>();
        
        // Custom Styles
        tree->indent_unit = "  ";
        tree->icon_expanded = "[-]";
        tree->icon_collapsed = "[+]";
        tree->icon_leaf = " o ";
        
        tree->root_nodes.push_back({"/"});
        auto& root_n = tree->root_nodes.back();
        root_n.expanded = true;
        root_n.color = {200, 100, 200}; // Purple root
        
        auto& home = root_n.add("home");
        home.expanded = true;
        auto& user = home.add("user");
        user.expanded = true;
        user.add("documents").color = {100, 200, 255};
        user.add("downloads").color = {100, 200, 255};
        user.add("music").color = {100, 200, 255};
        user.add("pictures").color = {100, 200, 255};
        
        auto& var = root_n.add("var");
        var.add("log");
        var.add("tmp");
        var.add("cache");
        
        auto& etc = root_n.add("etc");
        etc.add("passwd").color = {255, 100, 100}; // Red critical file
        etc.add("shadow").color = {255, 100, 100};
        etc.add("hosts");
        
        auto& usr = root_n.add("usr");
        auto& usrbin = usr.add("bin");
        usrbin.add("git");
        usrbin.add("vim");
        auto& usrlib = usr.add("lib");
        usrlib.add("python3");
        
        auto status_lbl = std::make_shared<Label>("Selected: None");
        

        tree->on_select = [status_lbl, tree](TreeNode* n) {
            status_lbl->set_text("Selected: " + tree->get_full_path(n));
        };
        
        tree->on_submit = [status_lbl, tree](TreeNode* n) {
             status_lbl->set_text("SUBMITTED: " + tree->get_full_path(n));
        };

        tree->refresh();
        
        box->add(tree);
        tree_layout->add(box);
        tree_layout->add(status_lbl);
        
        auto help = std::make_shared<Label>("Controls: Arrows to nav, Space/Enter to toggle/submit", Color{150, 150, 150});
        help->fixed_height = 1;
        tree_layout->add(help);
    }
    tabs->add_tab("Tree", tree_layout);

    // =========================================================================
    // TAB 5: LAYOUTS - Grid & Alignments
    // =========================================================================
    auto layouts_layout = std::make_shared<Vertical>();
    {
        // Alignments Section
        auto b_align = std::make_shared<Border>(BorderStyle::Single, Color{100, 200, 255});
        b_align->set_title(" Alignment Demo ", Alignment::Left);
        
        auto align_box = std::make_shared<Vertical>();
        align_box->fixed_height = 6;
        
        auto a1 = std::make_shared<Align>(Align::H::Left, Align::V::Center);
        auto b1 = std::make_shared<Button>("Left");
        b1->fixed_width = 10; b1->fixed_height = 1;
        a1->add(b1);
        
        auto a2 = std::make_shared<Align>(Align::H::Center, Align::V::Center);
        auto b2 = std::make_shared<Button>("Center");
        b2->fixed_width = 10; b2->fixed_height = 1;
        a2->add(b2);
        
        auto a3 = std::make_shared<Align>(Align::H::Right, Align::V::Center);
        auto b3 = std::make_shared<Button>("Right");
        b3->fixed_width = 10; b3->fixed_height = 1;
        a3->add(b3);
        
        align_box->add(a1);
        align_box->add(a2);
        align_box->add(a3);
        
        b_align->add(align_box);
        layouts_layout->add(b_align);
        
        // Grid Section
        auto b_grid = std::make_shared<Border>(BorderStyle::Double, Color{255, 180, 100});
        b_grid->set_title(" Grid Layout (3x4) ", Alignment::Center);
        
        auto grid = std::make_shared<Grid>();
        
        for(int r=0; r<3; ++r) {
            for(int c=0; c<4; ++c) {
                std::string txt = "(" + std::to_string(r) + "," + std::to_string(c) + ")";
                auto btn = std::make_shared<Button>(txt);
                btn->fixed_width = 8;
                grid->add_item(btn, r, c);
            }
        }
        
        b_grid->add(grid);
        layouts_layout->add(b_grid);
    }
    tabs->add_tab("Layouts", layouts_layout);

    // =========================================================================
    // TAB 6: SCROLLING - All Scroll Types
    // =========================================================================
    auto scroll_layout = std::make_shared<Vertical>();
    {
        // Vertical Scroll
        auto v_box = std::make_shared<Border>(BorderStyle::Rounded, Color{0, 200, 200});
        v_box->set_title(" Vertical Scroll ", Alignment::Left);
        v_box->fixed_height = 8;
        
        auto sv = std::make_shared<ScrollableVertical>();
        sv->add(std::make_shared<Label>("--- Start ---", Theme::current().primary));
        for(int i=1; i<=20; ++i) {
            std::string text = "Scrollable Item #" + std::to_string(i);
            auto lbl = std::make_shared<Label>(text);
            if (i % 2 == 0) lbl->fg_color = {200, 200, 255};
            sv->add(lbl);
        }
        sv->add(std::make_shared<Label>("--- End ---", Theme::current().primary));
        v_box->add(sv);
        scroll_layout->add(v_box);
        
        // Horizontal Scroll
        auto h_box = std::make_shared<Border>(BorderStyle::Rounded, Color{255, 100, 100});
        h_box->set_title(" Horizontal Scroll ", Alignment::Center);
        h_box->fixed_height = 4;

        auto sh = std::make_shared<ScrollableHorizontal>();
        for (int i=1; i<=15; ++i) {
            auto btn = std::make_shared<Button>("Item" + std::to_string(i));
            btn->fixed_width = 10;
            sh->add(btn);
        }
        h_box->add(sh);
        scroll_layout->add(h_box);
        
        // 2D Scroll
        auto box2d = std::make_shared<Border>(BorderStyle::Double, Color{150, 100, 255});
        box2d->set_title(" 2D Scrollable Container ", Alignment::Right);
        
        auto sc = std::make_shared<ScrollableContainer>();
        auto grid = std::make_shared<Grid>();
        grid->fixed_width = 80;
        grid->fixed_height = 30; 
        
        for(int r=0; r<12; ++r) {
            for(int c=0; c<8; ++c) {
                std::string txt = "R" + std::to_string(r) + "C" + std::to_string(c);
                auto btn = std::make_shared<Button>(txt);
                grid->add_item(btn, r, c);
            }
        }
        
        sc->add(grid);
        box2d->add(sc);
        scroll_layout->add(box2d);
    }
    tabs->add_tab("Scroll", scroll_layout);

    // =========================================================================
    // TAB 7: CAROUSEL - Wizard Demo
    // =========================================================================
    auto carousel_layout = std::make_shared<Vertical>();
    {
        auto car_box = std::make_shared<Border>(BorderStyle::Double, Color{255, 180, 50});
        car_box->set_title(" Carousel Wizard ", Alignment::Center);
        
        auto carousel = std::make_shared<Carousel>();
        
        // Page 1: Welcome
        auto p1 = std::make_shared<Vertical>();
        p1->add(std::make_shared<Label>("Welcome!", Color{100, 255, 100}));
        p1->add(std::make_shared<VerticalSpacer>(1));
        p1->add(std::make_shared<Label>("This is a multi-step wizard demo."));
        p1->add(std::make_shared<Label>("Use the arrows or [ and ] to navigate."));
        carousel->add_page(p1);
        
        // Page 2: Configuration
        auto p2 = std::make_shared<Vertical>();
        p2->add(std::make_shared<Label>("Configuration", Color{255, 255, 100}));
        p2->add(std::make_shared<VerticalSpacer>(1));
        auto p2_toggle = std::make_shared<ToggleSwitch>("Enable Feature");
        p2->add(p2_toggle);
        auto p2_radio = std::make_shared<RadioSet>();
        p2_radio->set_options({"Option A", "Option B", "Option C"});
        p2->add(p2_radio);
        carousel->add_page(p2);
        
        // Page 3: Summary
        auto p3 = std::make_shared<Vertical>();
        p3->add(std::make_shared<Label>("Complete!", Color{100, 200, 255}));
        p3->add(std::make_shared<VerticalSpacer>(1));
        p3->add(std::make_shared<Label>("Your wizard setup is complete."));
        p3->add(std::make_shared<Label>("Click Finish to apply settings."));
        auto finish_btn = std::make_shared<Button>("Finish", [](){});
        finish_btn->fixed_width = 14;
        finish_btn->fixed_height = 1;
        finish_btn->bg_color = Theme::current().success;
        finish_btn->text_color = Color(0, 0, 0); // Black text for contrast
        
        auto a_finish = std::make_shared<Align>(Align::H::Center, Align::V::Top);
        a_finish->add(finish_btn);
        
        p3->add(a_finish);
        carousel->add_page(p3);
        
        carousel->show_arrows = true;
        
        car_box->add(carousel);
        carousel_layout->add(car_box);
    }
    tabs->add_tab("Carousel", carousel_layout);

    // =========================================================================
    // TAB 8: DIALOGS - MenuBar & Dialogs
    // =========================================================================
    auto dialog_layout = std::make_shared<Vertical>();
    {
        auto box = std::make_shared<Border>(BorderStyle::Double, Color{255, 100, 150});
        box->set_title(" MenuBar & Dialogs ", Alignment::Center);
        
        auto v = std::make_shared<ScrollableVertical>();
        box->add(v);
        
        // MenuBar
        v->add(std::make_shared<Label>("[ Menu Bar ]", Theme::current().primary));
        auto menubar = std::make_shared<MenuBar>(&app);
        menubar->items.push_back({"File", [](){}, {
            {"New", [](){}},
            {"Open", [](){}},
            {"Save", [](){}},
            {"Exit", [](){ App::quit(); }}
        }});
        menubar->items.push_back({"Edit", [](){}, {
            {"Undo", [](){}},
            {"Redo", [](){}},
            {"Cut", [](){}},
            {"Copy", [](){}},
            {"Paste", [](){}}
        }});
        menubar->items.push_back({"View", [](){}, {
            {"Zoom In", [](){}},
            {"Zoom Out", [](){}},
            {"Reset", [](){}}
        }});
        menubar->items.push_back({"Help", [](){}, {
            {"Documentation", [](){}},
            {"About", [](){}}
        }});
        v->add(menubar);
        
        v->add(std::make_shared<VerticalSpacer>(1));
        
        // Dialog demos
        v->add(std::make_shared<Label>("[ Dialog Demos ]", Theme::current().primary));
        
        // Info Dialog
        auto dlg1 = std::make_shared<Dialog>(&app, BorderStyle::Double);
        dlg1->set_title(" Info ");
        dlg1->width = 30; dlg1->height = 8;
        dlg1->bg_color = {30, 30, 50};
        
        auto d_v = std::make_shared<Vertical>();
        d_v->add(std::make_shared<VerticalSpacer>(1));
        d_v->add(std::make_shared<Label>("Hello from Dialog!"));
        d_v->add(std::make_shared<VerticalSpacer>(1));
        
        auto ok_btn = std::make_shared<Button>("OK", [dlg1](){
            dlg1->close();
        });
        ok_btn->fixed_width = 8;
        auto a_btn = std::make_shared<Align>(Align::H::Center, Align::V::Center);
        a_btn->add(ok_btn);
        d_v->add(a_btn);
        dlg1->add(d_v);
        
        auto btn_open1 = std::make_shared<Button>("Open Info Dialog", [dlg1](){
            dlg1->open(10, 5); 
        });
        v->add(btn_open1);
        
        v->add(std::make_shared<VerticalSpacer>(1));
        
        // Modal Input Dialog
        auto dlg2 = std::make_shared<Dialog>(&app, BorderStyle::Rounded);
        dlg2->set_title(" Enter Name ");
        dlg2->width = 40; dlg2->height = 10;
        dlg2->modal = true;
        dlg2->shadow = true;
        dlg2->bg_color = {30, 30, 50};
        
        auto d2_v = std::make_shared<Vertical>();
        auto inp = std::make_shared<Input>();
        inp->placeholder = "Your Name...";
        inp->fixed_height = 1;
        inp->bg_color = {20, 20, 70};

        d2_v->add(std::make_shared<Label>("Please enter your name:"));
        d2_v->add(inp);
        d2_v->add(std::make_shared<VerticalSpacer>());
        
        auto h_btns = std::make_shared<Horizontal>();
        h_btns->fixed_height = 1;

        auto btn_submit = std::make_shared<Button>("Submit", [dlg2](){
            dlg2->close();
        });
        btn_submit->bg_color = Theme::current().success;
        h_btns->add(btn_submit);
        
        h_btns->add(std::make_shared<Label>("  "));
        
        auto btn_cancel = std::make_shared<Button>("Cancel", [dlg2](){
             dlg2->close();
        });
        btn_cancel->bg_color = Theme::current().error;
        h_btns->add(btn_cancel);
        
        d2_v->add(h_btns);
        dlg2->add(d2_v);
      
        auto btn_open2 = std::make_shared<Button>("Open Modal Dialog", [dlg2](){
             dlg2->open(15, 8);
        });
        v->add(btn_open2);
        
        v->add(std::make_shared<VerticalSpacer>(1));
        
        // Menu Dialog
        auto menu = std::make_shared<MenuDialog>(&app);
        menu->set_title(" Context Menu ");
        menu->items = {
             {"Open", [](){}},
             {"Edit", [](){}},
             {"Delete", [](){}},
             {"Close", [menu](){ menu->close(); }}
        };
        
        auto btn_menu = std::make_shared<Button>("Open Context Menu", [menu](){
            menu->open(40, 10);
        });
        v->add(btn_menu);
        
        v->add(std::make_shared<VerticalSpacer>(1));
        
        // Notifications Section
        v->add(std::make_shared<Label>("--- Notifications ---", Theme::current().secondary));
        
        // Notification widget (uses screen coordinates for top-right positioning)
        auto notif = std::make_shared<Notification>();
        notif->max_visible = 3;
        
        auto h_notif = std::make_shared<Horizontal>();
        
        h_notif->add(make_notif_button("Info", notif, Notification::Type::Info, "Information message displayed!"));
        h_notif->add(std::make_shared<HorizontalSpacer>(1));
        h_notif->add(make_notif_button("Success", notif, Notification::Type::Success, "Operation completed successfully!", Theme::current().success));
        h_notif->add(std::make_shared<HorizontalSpacer>(1));
        h_notif->add(make_notif_button("Warning", notif, Notification::Type::Warning, "This is a warning notification", Theme::current().warning));
        h_notif->add(std::make_shared<HorizontalSpacer>(1));
        h_notif->add(make_notif_button("Error", notif, Notification::Type::Error, "An error has occurred!", Theme::current().error));
        
        v->add(h_notif);
        v->add(std::make_shared<Label>("Click buttons to show toast notifications", Theme::current().foreground));
        
        v->add(std::make_shared<VerticalSpacer>(1));
        
        // Tooltip Section  
        v->add(std::make_shared<Label>("--- Tooltip ---", Theme::current().secondary));
        
        auto tooltip_btn = std::make_shared<Button>("Hover over me");
        tooltip_btn->fixed_width = 16;
        
        // Use set_tooltip for automatic handling
        auto t = std::make_shared<Tooltip>("This is a helpful tooltip!");
        t->position = Tooltip::Position::Bottom;
        tooltip_btn->set_tooltip(t);
        
        v->add(tooltip_btn);
        v->add(std::make_shared<Label>("Hover over button to show tooltip", Theme::current().foreground));
        
        // Add notification (uses screen coordinates)
        box->add(notif);
        
        dialog_layout->add(box);
    }
    tabs->add_tab("Dialogs", dialog_layout);

    // =========================================================================
    // TAB 9: STYLES - Custom Widget Styling
    // =========================================================================
    auto styled_layout = std::make_shared<Vertical>();
    {
        auto box = std::make_shared<Border>(BorderStyle::Double, Color{255, 200, 50});
        box->set_title(" Custom Widget Styles ", Alignment::Center);
        auto sv = std::make_shared<ScrollableVertical>();
        box->add(sv);
        
        // Custom Toggle
        sv->add(std::make_shared<Label>("[ Custom Toggle ]", Theme::current().primary));
        auto pwr = std::make_shared<ToggleSwitch>("System Power");
        pwr->on_label = "[HIGH]";
        pwr->off_label = "[LOW ]";
        pwr->active_color = {255, 50, 50};   // Red for HIGH
        pwr->inactive_color = {0, 255, 0};    // Green for LOW
        sv->add(pwr);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Custom Radio
        sv->add(std::make_shared<Label>("[ Custom Radio ]", Theme::current().primary));
        auto radio = std::make_shared<RadioSet>();
        radio->set_options({"Retro A", "Retro B", "Retro C"});
        radio->set_style("> ", "  ");
        radio->selected_color = {255, 0, 255};
        sv->add(radio);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Custom Checkbox List
        sv->add(std::make_shared<Label>("[ Custom Checkbox List ]", Theme::current().primary));
        auto check = std::make_shared<CheckboxList>();
        check->set_options({"Read", "Write", "Execute"});
        check->checked_prefix = "[V] ";
        check->unchecked_prefix = "[ ] ";
        check->checked_color = {0, 255, 0};
        check->cursor_color = {255, 255, 0};
        sv->add(check);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Custom Progress Bars
        sv->add(std::make_shared<Label>("[ Custom Progress Bars ]", Theme::current().primary));
        
        sv->add(std::make_shared<Label>("ASCII Style:"));
        auto pbar1 = std::make_shared<ProgressBar>(0.75f);
        pbar1->max_height = 1;
        pbar1->char_filled = "#";
        pbar1->char_empty = "-"; 
        pbar1->color = {255, 100, 0};
        pbar1->empty_color = {80, 80, 80};
        sv->add(pbar1);
        
        sv->add(std::make_shared<Label>("Block Style:"));
        auto pbar2 = std::make_shared<ProgressBar>(0.5f);
        pbar2->max_height = 1;
        pbar2->char_filled = "█";
        pbar2->char_empty = "░"; 
        pbar2->color = {100, 200, 255};
        pbar2->empty_color = {50, 50, 80};
        sv->add(pbar2);
        
        sv->add(std::make_shared<Label>("Text Overlay (Percentage):"));
        auto pbar3 = std::make_shared<ProgressBar>(0.65f);
        pbar3->show_text = true;
        pbar3->color = {100, 255, 100};
        pbar3->empty_color = {60, 60, 60};
        sv->add(pbar3);

        sv->add(std::make_shared<Label>("Text Overlay (Custom & Centered):"));
        auto pbar4 = std::make_shared<ProgressBar>(0.4f);
        pbar4->show_text = true;
        pbar4->text_formatter = [](float v) { return "Loading Resources... " + std::to_string(int(v*100)) + "%"; };
        pbar4->color = {255, 100, 255};
        pbar4->empty_color = {60, 60, 60};
        pbar4->text_color = {255, 255, 255};
        sv->add(pbar4);

        sv->add(std::make_shared<VerticalSpacer>(1));

        sv->add(std::make_shared<VerticalSpacer>(1));

        // Row 1: Indeterminate
        sv->add(std::make_shared<Label>("Indeterminate Spinners:", Theme::current().primary));
        auto row_ind = std::make_shared<Horizontal>();
        row_ind->height = 1;
        
        auto sp_dots = std::make_shared<Spinner>(&app, Spinner::StyleBrailleSpin());
        add_spinner_with_label(row_ind, "Braille: ", 9, sp_dots);

        auto sp_line = std::make_shared<Spinner>(&app, Spinner::StyleLineSpin());
        add_spinner_with_label(row_ind, "Line: ", 6, sp_line);

        auto sp_pie = std::make_shared<Spinner>(&app, Spinner::StylePieSpin()); 
        sp_pie->color = Color::Magenta();
        add_spinner_with_label(row_ind, "Pie: ", 5, sp_pie);

        auto sp_bar = std::make_shared<Spinner>(&app, Spinner::StyleBounceBar()); 
        sp_bar->color = Color::Cyan();
        add_spinner_with_label(row_ind, "Bar: ", 5, sp_bar);

        auto sp_circle = std::make_shared<Spinner>(&app, Spinner::StylePulseAscii()); 
        sp_circle->color = Color::Yellow();
        add_spinner_with_label(row_ind, "Pulse: ", 7, sp_circle, false);

        sv->add(row_ind);
        sv->add(std::make_shared<VerticalSpacer>(1));

        // Row 2: Determinate
        sv->add(std::make_shared<Label>("Determinate Spinners:", Theme::current().primary));
        auto row_det = std::make_shared<Horizontal>();
        row_det->height = 1;

        // Braille (uses value to show progress, but still spins)
        sp_dots_det = std::make_shared<Spinner>(&app, Spinner::StyleBrailleSpin());
        lbl_dots = std::make_shared<Label>("Braille 0%"); lbl_dots->fixed_width = 12;
        row_det->add(lbl_dots); row_det->add(sp_dots_det); 
        
        auto sep_det1 = std::make_shared<Label>("  "); sep_det1->fixed_width = 2; row_det->add(sep_det1);

        // Line
        sp_line_det = std::make_shared<Spinner>(&app, Spinner::StyleLineSpin());
        lbl_line = std::make_shared<Label>("Line 0%"); lbl_line->fixed_width = 9;
        row_det->add(lbl_line); row_det->add(sp_line_det); 
        
        auto sep_det2 = std::make_shared<Label>("  "); sep_det2->fixed_width = 2; row_det->add(sep_det2);

        // Pie (Progress - maps value to frame)
        sp_pie_det = std::make_shared<Spinner>(&app, Spinner::StylePieProgress());
        lbl_pie = std::make_shared<Label>("Pie 0%"); lbl_pie->fixed_width = 8;
        row_det->add(lbl_pie); row_det->add(sp_pie_det); 
        
        auto sep_det3 = std::make_shared<Label>("  "); sep_det3->fixed_width = 2; row_det->add(sep_det3);

        // Vertical Bar (Progress - maps value to frame for filling effect)
        sp_bounce_bar_det = std::make_shared<Spinner>(&app, Spinner::StyleBarProgress()); 
        sp_bounce_bar_det->color = Color::Cyan();
        lbl_bar = std::make_shared<Label>("Bar 0%"); lbl_bar->fixed_width = 8;
        row_det->add(lbl_bar); row_det->add(sp_bounce_bar_det); 
        
        auto sep_det4 = std::make_shared<Label>("  "); sep_det4->fixed_width = 2; row_det->add(sep_det4);

        // Pulse ASCII (with value tracking, but still spins)
        sp_bounce_circle_det = std::make_shared<Spinner>(&app, Spinner::StylePulseAscii());
        sp_bounce_circle_det->color = Color::Yellow();
        lbl_circle = std::make_shared<Label>("Pulse 0%"); lbl_circle->fixed_width = 10;
        row_det->add(lbl_circle); row_det->add(sp_bounce_circle_det);

        sv->add(row_det);


        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Responsive Visibility
        sv->add(std::make_shared<Label>("[ Responsive Visibility ]", Theme::current().primary));
        
        auto resp_large = std::make_shared<Label>("Visible on LARGE WIDTH (>120 cols)", Color::Red());
        resp_large->set_responsive(false, false, true);
        sv->add(resp_large);
        
        auto resp_sm = std::make_shared<Label>("Visible on SMALL/MEDIUM WIDTH", Color::Green());
        resp_sm->set_responsive(true, true, false);
        sv->add(resp_sm);

        sv->add(std::make_shared<VerticalSpacer>(1));
        
        auto resp_h = std::make_shared<Label>("Visible on SMALL/MEDIUM HEIGHT (<50 lines)", Color::Red());
        resp_h->set_responsive_height(true, true, false); 
        sv->add(resp_h);
        
        auto resp_h_lg = std::make_shared<Label>("Visible on LARGE HEIGHT (>50 lines)", Color::Green());
        resp_h_lg->set_responsive_height(false, false, true);
        sv->add(resp_h_lg);

        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Badge
        sv->add(std::make_shared<Label>("[ Badge ]", Theme::current().primary));
        auto badge_row = std::make_shared<Horizontal>();
        badge_row->fixed_height = 1;
        auto badge1 = std::make_shared<Badge>("Running", Theme::current().success);
        badge1->fixed_width = 11;
        auto badge2 = std::make_shared<Badge>("Warning", Theme::current().warning);
        badge2->fixed_width = 11;
        auto badge3 = std::make_shared<Badge>("Error", Theme::current().error);
        badge3->fixed_width = 9;
        auto badge4 = std::make_shared<Badge>("v2.1", Theme::current().primary);
        badge4->style = Badge::Style::Square;
        badge4->fixed_width = 7;
        badge_row->add(badge1);
        badge_row->add(std::make_shared<HorizontalSpacer>(1));
        badge_row->add(badge2);
        badge_row->add(std::make_shared<HorizontalSpacer>(1));
        badge_row->add(badge3);
        badge_row->add(std::make_shared<HorizontalSpacer>(1));
        badge_row->add(badge4);
        sv->add(badge_row);
        
        styled_layout->add(box);
    }
    tabs->add_tab("Styles", styled_layout);

    // =========================================================================
    // TAB 10: TEXT - Text Styling & Paragraphs
    // =========================================================================
    auto text_layout = std::make_shared<Vertical>();
    {
        auto box = std::make_shared<Border>(BorderStyle::Rounded, Color{100, 200, 100});
        box->set_title(" Text Styling & Paragraphs ", Alignment::Center);
        auto sv = std::make_shared<ScrollableVertical>();
        box->add(sv);
        
        // Section: Basic Text Styles
        sv->add(std::make_shared<Label>("[ Basic Text Styles ]", Theme::current().primary));
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Bold paragraph
        auto p_bold = std::make_shared<Paragraph>("This text is bold. Bold text stands out.");
        p_bold->bold = true;
        p_bold->fixed_height = 1;
        sv->add(p_bold);
        
        // Italic paragraph
        auto p_italic = std::make_shared<Paragraph>("This text is italic. Italics add emphasis.");
        p_italic->italic = true;
        p_italic->fixed_height = 1;
        sv->add(p_italic);
        
        // Underline paragraph (default: text-only)
        auto p_underline = std::make_shared<Paragraph>("Underline: This only underlines the characters in the text.");
        p_underline->underline = true;
        p_underline->fixed_height = 1;
        sv->add(p_underline);
        
        // Label with underline
        auto lbl_underline = std::make_shared<Label>("Underlined Label");
        lbl_underline->underline = true;
        sv->add(lbl_underline);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Section: Mixed Styled Text
        sv->add(std::make_shared<Label>("[ Mixed Styled Text ]", Theme::current().primary));
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Create styled text with mixed formatting
        StyledText mixed;
        mixed.add("This sentence has ")
             .bold("bold")
             .add(", ")
             .italic("italic")
             .add(", ")
             .underline("underlined")
             .add(", and ")
             .bold_italic("bold+italic")
             .add(" text.");
        
        auto p_mixed = std::make_shared<Paragraph>(mixed);
        p_mixed->fixed_height = 1;
        sv->add(p_mixed);
        
        // Colored styled text
        StyledText colored_mixed;
        colored_mixed.add("Colors: ")
                     .colored("red", Color::Red())
                     .add(", ")
                     .colored("green", Color::Green())
                     .add(", ")
                     .colored("blue", Color::Blue())
                     .add(", ")
                     .colored_bold("cyan bold", Color::Cyan())
                     .add(".");
        
        auto p_colored = std::make_shared<Paragraph>(colored_mixed);
        p_colored->fixed_height = 1;
        sv->add(p_colored);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Section: Paragraph Indentation
        sv->add(std::make_shared<Label>("[ Paragraph Indentation ]", Theme::current().primary));
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // First-line indent (like a book paragraph)
        sv->add(std::make_shared<Label>("First-line indent (4 spaces):", Color{180, 180, 180}));
        auto p_first = std::make_shared<Paragraph>(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor "
            "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam.");
        p_first->first_line_indent = 4;
        p_first->fixed_height = 3;
        sv->add(p_first);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Hanging indent (like a bibliography)
        sv->add(std::make_shared<Label>("Hanging indent (4 spaces on subsequent lines):", Color{180, 180, 180}));
        auto p_hang = std::make_shared<Paragraph>(
            "Smith, J. (2024). The Complete Guide to Terminal User Interfaces. "
            "New York: Tech Publishing. ISBN 978-0-123456-78-9.");
        p_hang->first_line_indent = 0;
        p_hang->hanging_indent = 4;
        p_hang->fixed_height = 3;
        sv->add(p_hang);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Section: Word Wrapping Demo
        sv->add(std::make_shared<Label>("[ Word Wrapping Demo ]", Theme::current().primary));
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        auto p_wrap = std::make_shared<Paragraph>(
            "This paragraph demonstrates automatic word wrapping. As you resize the terminal, "
            "the text will reflow to fit the available width while respecting word boundaries. "
            "No words are split in the middle unless they exceed the line width.");
        p_wrap->fixed_height = 4;
        p_wrap->fg_color = Color{200, 220, 255};
        sv->add(p_wrap);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Section: Text Lists
        sv->add(std::make_shared<Label>("[ Text Lists ]", Theme::current().primary));
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Bullet List
        sv->add(std::make_shared<Label>("Bullet List (default marker):", Color{180, 180, 180}));
        auto bullet_list = std::make_shared<TextList>();
        bullet_list->items = {
            ListItem{"First item in the list"},
            ListItem{"Second item"},
            ListItem{"Third item with more text"}
        };
        bullet_list->style = ListStyle::Bullet;
        bullet_list->fixed_height = 3;
        sv->add(bullet_list);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Nested Bullet List
        sv->add(std::make_shared<Label>("Nested List (with levels):", Color{180, 180, 180}));
        auto nested_list = std::make_shared<TextList>();
        nested_list->items = {
            ListItem{"Fruits", 0},
            ListItem{"Apples", 1},
            ListItem{"Granny Smith", 2},
            ListItem{"Fuji", 2},
            ListItem{"Oranges", 1},
            ListItem{"Vegetables", 0},
            ListItem{"Carrots", 1},
            ListItem{"Broccoli", 1}
        };
        nested_list->style = ListStyle::Bullet;
        nested_list->fixed_height = 8;
        nested_list->fg_color = Color{150, 255, 200};
        sv->add(nested_list);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Numbered List
        sv->add(std::make_shared<Label>("Numbered List:", Color{180, 180, 180}));
        auto num_list = std::make_shared<TextList>();
        num_list->items = {
            ListItem{"Install dependencies"},
            ListItem{"Configure the project"},
            ListItem{"Build and run"}
        };
        num_list->style = ListStyle::Numbered;
        num_list->fixed_height = 3;
        sv->add(num_list);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // Nested Numbered List
        sv->add(std::make_shared<Label>("Nested Numbered List:", Color{180, 180, 180}));
        auto nested_num = std::make_shared<TextList>();
        nested_num->items = {
            ListItem{"Getting Started", 0},
            ListItem{"Download the source", 1},
            ListItem{"Install prerequisites", 1},
            ListItem{"Building", 0},
            ListItem{"Run cmake", 1},
            ListItem{"Run make", 1},
            ListItem{"Testing", 0}
        };
        nested_num->style = ListStyle::Numbered;
        nested_num->fixed_height = 7;
        nested_num->fg_color = Color{255, 200, 100};
        sv->add(nested_num);
        
        sv->add(std::make_shared<VerticalSpacer>(1));
        
        // List with word wrapping
        sv->add(std::make_shared<Label>("List with Word Wrapping:", Color{180, 180, 180}));
        auto wrap_list = std::make_shared<TextList>();
        wrap_list->items = {
            ListItem{"This is a longer item that will wrap to the next line if the widget is narrow enough"},
            ListItem{"Another item with extended text to demonstrate the wrapping behavior"},
            ListItem{"Short item"}
        };
        wrap_list->style = ListStyle::Bullet;
        wrap_list->bullet_markers = {"> "};
        wrap_list->word_wrap = true;
        wrap_list->fixed_height = 6;
        wrap_list->fg_color = Color{200, 200, 255};
        sv->add(wrap_list);
        
        text_layout->add(box);
    }
    tabs->add_tab("Text", text_layout);

    // === NEW WIDGETS TAB ===
    auto new_widgets_layout = std::make_shared<ScrollableVertical>();
    {
        // Title
        new_widgets_layout->add(std::make_shared<Label>("=== Layout & Navigation Widgets ===", Theme::current().primary));
        new_widgets_layout->add(std::make_shared<VerticalSpacer>(1));
        
        // --- SearchInput Section ---
        new_widgets_layout->add(std::make_shared<Label>("[ SearchInput - Type to filter ]", Theme::current().secondary));
        auto search = std::make_shared<SearchInput>(&app);
        search->placeholder = "Search fruits...";
        search->suggestions = {
            "Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig", "Grape", "Honeydew", "Kiwi", "Lemon", "Mango"};
        new_widgets_layout->add(search);

        auto search_result = std::make_shared<Label>("Select a fruit and press Enter", Theme::current().border);
        search_result->fixed_height = 1;
        new_widgets_layout->add(search_result);
        
        search->on_search = [search_result](const std::string& value) {
            if (!value.empty()) {
                search_result->set_text("You selected: " + value);
                search_result->fg_color = Theme::current().success;
            } else {
                search_result->set_text("No fruit selected");
                search_result->fg_color = Theme::current().border;
            }
        };
        new_widgets_layout->add(std::make_shared<VerticalSpacer>(1));
        
        // --- Breadcrumb Section ---
        new_widgets_layout->add(std::make_shared<Label>("[ Breadcrumb ]", Theme::current().secondary));
        auto bc = std::make_shared<Breadcrumb>();
        auto bc_result = std::make_shared<Label>("Click a breadcrumb to navigate", Theme::current().border);
        bc_result->fixed_height = 1;
        
        bc->add("Home", [bc_result](){
            bc_result->set_text("Navigated to: Home");
            bc_result->fg_color = Theme::current().primary;
        });
        bc->add("Products", [bc_result](){
            bc_result->set_text("Navigated to: Home > Products");
            bc_result->fg_color = Theme::current().primary;
        });
        bc->add("Electronics", [bc_result](){
            bc_result->set_text("Navigated to: Home > Products > Electronics");
            bc_result->fg_color = Theme::current().primary;
        });
        bc->add("Laptops", [bc_result](){
            bc_result->set_text("Current page: Laptops");
            bc_result->fg_color = Theme::current().success;
        });
        bc->fixed_height = 1;
        new_widgets_layout->add(bc);
        new_widgets_layout->add(bc_result);
        new_widgets_layout->add(std::make_shared<VerticalSpacer>(1));
        
        // --- ShortcutBar Section ---
        new_widgets_layout->add(std::make_shared<Label>("[ ShortcutBar ]", Theme::current().secondary));
        auto shortcuts = std::make_shared<ShortcutBar>();
        shortcuts->add("F1", "Help");
        shortcuts->add("F5", "Refresh");
        shortcuts->add("Ctrl+S", "Save");
        shortcuts->add("Q", "Quit");
        shortcuts->fixed_height = 1;
        new_widgets_layout->add(shortcuts);
        new_widgets_layout->add(std::make_shared<VerticalSpacer>(1));
        
        // --- StatusBar Section ---
        new_widgets_layout->add(std::make_shared<Label>("[ StatusBar ]", Theme::current().secondary));
        auto status = std::make_shared<StatusBar>();
        status->add_section("Ready");
        status->add_section("Ln 42, Col 15");
        status->add_section("UTF-8");
        status->add_section("LF");
        status->fixed_height = 1;
        new_widgets_layout->add(status);
    }
    tabs->add_tab("Utility", new_widgets_layout);

    // === ACCORDION TAB ===
    auto accordion_layout = std::make_shared<Vertical>();
    {
        accordion_layout->add(std::make_shared<Label>("=== Accordion Widget ===", Theme::current().primary));
        accordion_layout->add(std::make_shared<VerticalSpacer>(1));
        
        accordion_layout->add(std::make_shared<Label>("Use Up/Down arrows to navigate sections", Theme::current().secondary));
        accordion_layout->add(std::make_shared<Label>("Press Enter or Space to expand/collapse", Theme::current().secondary));
        accordion_layout->add(std::make_shared<VerticalSpacer>(1));
        
        auto accordion = std::make_shared<Accordion>();
        accordion->add_section("Section 1: Introduction", 
            std::make_shared<Label>("  Welcome! This is the introduction section with helpful content."));
        accordion->add_section("Section 2: Features", 
            std::make_shared<Label>("  Key features: Collapsible sections, keyboard navigation, theming."));
        accordion->add_section("Section 3: Usage", 
            std::make_shared<Label>("  Use add_section() to add content. Set allow_multiple for multi-expand."));
        accordion->add_section("Section 4: API Reference", 
            std::make_shared<Label>("  Properties: sections, selected_index, header_bg, header_fg, icons."));
        accordion->sections[0].expanded = true;
        accordion->fixed_height = 12;
        accordion_layout->add(accordion);
    }
    tabs->add_tab("Accordion", accordion_layout);

    // === CALENDAR TAB ===
    auto calendar_layout = std::make_shared<Vertical>();
    auto selected_date_label = std::make_shared<Label>("Selected Date: December 19, 2024", Theme::current().primary);
    {
        calendar_layout->add(std::make_shared<Label>("=== Calendar Widget ===", Theme::current().primary));
        calendar_layout->add(std::make_shared<VerticalSpacer>(1));
        
        calendar_layout->add(std::make_shared<Label>("Tab to < > buttons, or click them to change months", Theme::current().secondary));
        calendar_layout->add(std::make_shared<Label>("Click on a day to select, then use arrows to navigate", Theme::current().secondary));
        calendar_layout->add(std::make_shared<Label>("Press Enter to confirm selection", Theme::current().secondary));
        calendar_layout->add(std::make_shared<VerticalSpacer>(1));
        
        selected_date_label->fixed_height = 1;
        calendar_layout->add(selected_date_label);
        calendar_layout->add(std::make_shared<VerticalSpacer>(1));
        
        auto cal = std::make_shared<Calendar>();
        cal->show_border = true;  // Enable border
        cal->on_select = [selected_date_label](int year, int month, int day) {
            const char* months[] = {"January", "February", "March", "April", "May", "June",
                                    "July", "August", "September", "October", "November", "December"};
            selected_date_label->set_text("Selected Date: " + std::string(months[month-1]) + " " + 
                                          std::to_string(day) + ", " + std::to_string(year));
        };
        calendar_layout->add(cal);
    }
    tabs->add_tab("Calendar", calendar_layout);

    // === SPLITPANE TAB ===
    auto splitpane_layout = std::make_shared<ScrollableVertical>();
    {
        splitpane_layout->add(std::make_shared<Label>("=== Resizable Split Pane ===", Theme::current().primary));
        splitpane_layout->add(std::make_shared<VerticalSpacer>(1));
        splitpane_layout->add(std::make_shared<Label>("Drag the divider line to resize panes", Theme::current().secondary));
        splitpane_layout->add(std::make_shared<VerticalSpacer>(1));
        
        // Horizontal split (left | right)
        auto split_h = std::make_shared<SplitPane>();
        split_h->vertical = false;
        split_h->ratio = 0.4;
        split_h->fixed_height = 10;
        
        auto left_pane = std::make_shared<Border>(BorderStyle::Single, Theme::current().primary);
        left_pane->set_title(" Left Pane ", Alignment::Center);
        auto left_content = std::make_shared<Vertical>();
        left_content->add(std::make_shared<Label>("Resizable left panel"));
        left_content->add(std::make_shared<VerticalSpacer>(1));
        left_content->add(std::make_shared<Label>("Drag the | divider"));
        left_pane->add(left_content);
        
        auto right_pane = std::make_shared<Border>(BorderStyle::Single, Theme::current().secondary);
        right_pane->set_title(" Right Pane ", Alignment::Center);
        auto right_content = std::make_shared<Vertical>();
        right_content->add(std::make_shared<Label>("Resizable right panel"));
        right_content->add(std::make_shared<VerticalSpacer>(1));
        auto btn_right = std::make_shared<Button>("Click me!");
        btn_right->fixed_width = 12;
        right_content->add(btn_right);
        right_pane->add(right_content);
        
        split_h->set_panes(left_pane, right_pane);
        splitpane_layout->add(split_h);
        
        splitpane_layout->add(std::make_shared<VerticalSpacer>(1));
        
        // Vertical split (top / bottom)
        splitpane_layout->add(std::make_shared<Label>("Vertical Split (set vertical = true):", Theme::current().foreground));
        
        auto split_v = std::make_shared<SplitPane>();
        split_v->vertical = true;
        split_v->ratio = 0.5;
        split_v->fixed_height = 40;
        split_v->min_size1 = 4;      // Min 4 to fit border (2) + content (2)
        split_v->min_size2 = 4;
        
        auto top_pane = std::make_shared<Border>(BorderStyle::Rounded, Theme::current().success);
        top_pane->set_title(" Top ", Alignment::Left);
        top_pane->add(std::make_shared<Label>("Top panel - drag the line to resize"));
        
        auto bottom_pane = std::make_shared<Border>(BorderStyle::Rounded, Theme::current().warning);
        bottom_pane->set_title(" Bottom ", Alignment::Left);
        bottom_pane->add(std::make_shared<Label>("Bottom panel"));
        
        split_v->set_panes(top_pane, bottom_pane);
        splitpane_layout->add(split_v);
    }
    tabs->add_tab("SplitPane", splitpane_layout);

    // === COLORS TAB ===
    auto color_layout = std::make_shared<Vertical>();
    {
        color_layout->add(std::make_shared<Label>("=== Color Picker ===", Theme::current().primary));
        color_layout->add(std::make_shared<VerticalSpacer>(1));
        
        auto h_layout = std::make_shared<Horizontal>();
        h_layout->fixed_height = 16;
        
        auto picker = std::make_shared<ColorPicker>();
        picker->border_color = Theme::current().border;
        h_layout->add(picker);
        
        h_layout->add(std::make_shared<HorizontalSpacer>(2));
        
        auto info_v = std::make_shared<Vertical>();
        info_v->add(std::make_shared<Label>("Controls:", Theme::current().secondary));
        info_v->add(std::make_shared<Label>(" - Click on gradient to pick Hue/Sat"));
        info_v->add(std::make_shared<Label>(" - Click on slider to pick Value"));
        info_v->add(std::make_shared<Label>(" - Arrow Keys: Adjust Hue/Sat"));
        info_v->add(std::make_shared<Label>(" - +/- Keys: Adjust Value"));
        
        info_v->add(std::make_shared<VerticalSpacer>(1));
        
        auto selected_lbl = std::make_shared<Label>("Selected Color");
        selected_lbl->fixed_height = 3;
        selected_lbl->fixed_width = 20;
        selected_lbl->bg_color = Color::Red(); 
        selected_lbl->fg_color = Color::White();

        info_v->add(selected_lbl);
        
        // Sync picker with label
        picker->on_change = [selected_lbl](Color c) {
            selected_lbl->bg_color = c;
            selected_lbl->fg_color = Color::contrast_color(c);
            selected_lbl->set_text("RGB: " + std::to_string(c.r) + "," + std::to_string(c.g) + "," + std::to_string(c.b));
        };
        
        // Use on_select for final confirmation if needed

        
        // Set initial color
        picker->set_color(Color::Red());
        
        h_layout->add(info_v);
        color_layout->add(h_layout);
    }
    tabs->add_tab("Colors", color_layout);

    root->add(tabs);



    // Footer
    auto footer = std::make_shared<Label>("[ / ] Switch tabs  |  Arrow keys navigate  |  Enter/Space select  |  q quit", Color{150, 150, 150});
    footer->fixed_height = 1;
    root->add(footer);

    // Register Exit Key
    app.register_exit_key('q');

    // Enable tick for animations
    // Animation Timer for "Determinate" spinners only
    app.add_timer(80, [&](){
        if (!sp_dots_det) return;

        // Simulate progress for determinate spinners
        bool reset = false;
        float inc = 0.02f;

        auto update = [&](std::shared_ptr<Spinner> s, std::shared_ptr<Label> l, std::string prefix) {
            if (s->value < 0) s->value = 0; // ensure started
            s->value += inc;
            if (s->value > 1.2f) { // Pause at 100% (1.0) for a bit
                 reset = true;
            }
            int pct = (int)(std::min(s->value, 1.0f) * 100);
            l->set_text(prefix + " " + std::to_string(pct) + "%");
        };

        update(sp_dots_det, lbl_dots, "Braille");
        update(sp_line_det, lbl_line, "Line");
        update(sp_pie_det, lbl_pie, "Pie");
        update(sp_bounce_bar_det, lbl_bar, "Bar");
        update(sp_bounce_circle_det, lbl_circle, "Pulse");

        if (reset) {
            sp_dots_det->value = 0;
            sp_line_det->value = 0;
            sp_pie_det->value = 0;
            sp_bounce_bar_det->value = 0;
            sp_bounce_circle_det->value = 0;
        }
    });

    app.run(root);
    return 0;
}
