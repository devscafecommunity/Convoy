#pragma once
#include <functional>
#include <string>

namespace convoy {

struct MenuBarCallbacks {
    std::function<void()> on_new_project;
    std::function<void()> on_open_project;
    std::function<void()> on_save_project;
    std::function<void()> on_export;
    std::function<void()> on_undo;
    std::function<void()> on_redo;
    std::function<void(const std::string& module)> on_module_switch;
    std::function<void()> on_preferences;
    std::function<void(int)> on_layout_change;
    std::function<void()> on_toggle_snap_zones;
    std::function<void()> on_toggle_grid;
    std::function<void()> on_toggle_collision;
    std::function<void()> on_toggle_dod;
};

class MainMenuBar {
public:
    void set_callbacks(const MenuBarCallbacks& cb) { cb_ = cb; }
    void render(bool can_undo, bool can_redo,
                const std::string& project_name,
                float memory_mb, float memory_max_mb);

private:
    MenuBarCallbacks cb_;

    void render_file_menu();
    void render_edit_menu(bool can_undo, bool can_redo);
    void render_modules_menu();
    void render_view_menu();
    void render_status_right(const std::string& project_name,
                             float mem_mb, float mem_max);
};

}
