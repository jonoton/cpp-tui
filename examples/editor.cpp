#include "cpptui.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace cpptui;

int main()
{
    App app;

    auto root = std::make_shared<Vertical>();

    // Menu Bar
    auto menu_bar = std::make_shared<MenuBar>(&app);

    // File Menu
    auto file_menu = std::make_shared<MenuDialog>(&app);
    file_menu->set_title("File");

    // Main Content
    auto main_hf = std::make_shared<Horizontal>();

    auto explorer = std::make_shared<FileExplorer>(".");
    explorer->fixed_width = 30;

    auto text_area = std::make_shared<TextArea>();
    text_area->accepts_tab = true;

    // Notifications
    auto notif = std::make_shared<Notification>();

    file_menu->items.push_back({"New", [&text_area, notif]()
                                {
                                    text_area->set_text("");
                                    notif->show("New file created.", Notification::Type::Info);
                                }});

    file_menu->items.push_back({"Save", [&text_area, notif]()
                                {
                                    notif->show("File Saved (Simulated).", Notification::Type::Success);
                                }});

    file_menu->items.push_back({"Exit", []()
                                {
                                    App::quit();
                                }});

    menu_bar->items.push_back({"File", [file_menu, &app]()
                               {
                                   file_menu->open(0, 1);
                               }});

    // View Menu
    auto view_menu = std::make_shared<MenuDialog>(&app);
    view_menu->set_title("View");

    // Initial label for word wrap
    std::string wrap_label = text_area->word_wrap ? "Turn Word Wrap OFF" : "Turn Word Wrap ON";

    view_menu->items.push_back({wrap_label, [text_area, view_menu]()
                                {
                                    text_area->word_wrap = !text_area->word_wrap;
                                    // Update the menu item label for next time it's opened
                                    view_menu->items[0].label = text_area->word_wrap ? "Turn Word Wrap OFF" : "Turn Word Wrap ON";
                                    text_area->ensure_cursor_visible();
                                }});

    // Initial label for line numbers
    std::string ln_label = text_area->show_line_numbers ? "Hide Line Numbers" : "Show Line Numbers";

    view_menu->items.push_back({ln_label, [text_area, view_menu]()
                                {
                                    text_area->show_line_numbers = !text_area->show_line_numbers;
                                    // Update the menu item label (index 1)
                                    view_menu->items[1].label = text_area->show_line_numbers ? "Hide Line Numbers" : "Show Line Numbers";
                                    // Force cursor visibility check as dimensions change
                                    text_area->ensure_cursor_visible();
                                }});

    // Tab handling toggle
    std::string tab_label = text_area->accepts_tab ? "Disable Tab Indent" : "Enable Tab Indent";
    view_menu->items.push_back({tab_label, [text_area, view_menu]()
                                {
                                    text_area->accepts_tab = !text_area->accepts_tab;
                                    view_menu->items[2].label = text_area->accepts_tab ? "Disable Tab Indent" : "Enable Tab Indent";
                                }});

    menu_bar->items.push_back({"View", [view_menu, &app, menu_bar]()
                               {
                                   // Position it after File
                                   int x_pos = 1 + 6; // [File ] -> [View]
                                   view_menu->open(x_pos, 1);
                               }});

    root->add(menu_bar);

    auto split = std::make_shared<SplitPane>();
    split->set_panes(explorer, text_area);
    split->ratio = 0.25;

    auto main_area = std::make_shared<Stack>();
    main_area->add(split);
    main_area->add(notif); // Add notification as overlay

    root->add(main_area);

    // Status Bar
    auto status_bar = std::make_shared<StatusBar>();
    status_bar->add_section("cpp-tui Editor", 20);
    status_bar->add_section("UTF-8", 10);
    status_bar->add_section("Ln 1, Col 1", 15);
    root->add(status_bar);

    text_area->on_cursor_move = [status_bar](int x, int y)
    {
        status_bar->sections[2].content = "Ln " + std::to_string(y + 1) + ", Col " + std::to_string(x + 1);
    };

    explorer->on_file_selected = [&](std::string path)
    {
        std::ifstream f(path);
        if (f)
        {
            text_area->load_from_stream(f);
            status_bar->sections[0].content = std::filesystem::path(path).filename().string();
        }
    };

    app.run(root);

    return 0;
}
