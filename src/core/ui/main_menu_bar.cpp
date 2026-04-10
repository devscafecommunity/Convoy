#include "main_menu_bar.h"
#include "theme_manager.h"
#include <imgui.h>
#include <cstdio>

namespace convoy {

void MainMenuBar::render(bool can_undo, bool can_redo,
                         const std::string& project_name,
                         float memory_mb, float memory_max_mb) {
    if (ImGui::BeginMainMenuBar()) {
        render_file_menu();
        render_edit_menu(can_undo, can_redo);
        render_modules_menu();
        render_view_menu();
        render_status_right(project_name, memory_mb, memory_max_mb);
        ImGui::EndMainMenuBar();
    }
}

void MainMenuBar::render_file_menu() {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Project",  "Ctrl+N") && cb_.on_new_project)  cb_.on_new_project();
        if (ImGui::MenuItem("Open...",      "Ctrl+O") && cb_.on_open_project) cb_.on_open_project();
        ImGui::Separator();
        if (ImGui::MenuItem("Save",         "Ctrl+S") && cb_.on_save_project) cb_.on_save_project();
        ImGui::Separator();
        if (ImGui::MenuItem("Deploy (.caf)","Ctrl+D") && cb_.on_export)       cb_.on_export();
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_edit_menu(bool can_undo, bool can_redo) {
    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, can_undo) && cb_.on_undo) cb_.on_undo();
        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, can_redo) && cb_.on_redo) cb_.on_redo();
        ImGui::Separator();
        if (ImGui::MenuItem("Preferences") && cb_.on_preferences) cb_.on_preferences();
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_modules_menu() {
    if (ImGui::BeginMenu("Modules")) {
        if (ImGui::MenuItem("Architect (Pixel)",    "F1") && cb_.on_module_switch) cb_.on_module_switch("architect");
        if (ImGui::MenuItem("Forge (Atlas)",        "F2") && cb_.on_module_switch) cb_.on_module_switch("forge");
        if (ImGui::MenuItem("Sequencer (Anim)",     "F3") && cb_.on_module_switch) cb_.on_module_switch("sequencer");
        if (ImGui::MenuItem("Walker (World)",       "F4") && cb_.on_module_switch) cb_.on_module_switch("walker");
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_view_menu() {
    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Toggle Grid",       "G")    && cb_.on_toggle_grid)      cb_.on_toggle_grid();
        if (ImGui::MenuItem("Collision Overlay", "C")    && cb_.on_toggle_collision) cb_.on_toggle_collision();
        if (ImGui::MenuItem("DOD Visualizer")            && cb_.on_toggle_dod)       cb_.on_toggle_dod();
        if (ImGui::MenuItem("Snap Zones",    "Tab") && cb_.on_toggle_snap_zones) cb_.on_toggle_snap_zones();
        ImGui::Separator();
        if (ImGui::MenuItem("Layout: Artisan",      "F5") && cb_.on_layout_change) cb_.on_layout_change(0);
        if (ImGui::MenuItem("Layout: Level Design","F6") && cb_.on_layout_change) cb_.on_layout_change(1);
        if (ImGui::MenuItem("Layout: Debugging",   "F7") && cb_.on_layout_change) cb_.on_layout_change(2);
        ImGui::Separator();
        if (ImGui::MenuItem("Reset Layout") && cb_.on_layout_change) cb_.on_layout_change(-1);
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_status_right(const std::string& project_name,
                                      float mem_mb, float mem_max) {
    char buf[128];
    snprintf(buf, sizeof(buf), "[ %s ]   Mem: %.0f / %.0f MB",
             project_name.c_str(), mem_mb, mem_max);

    float text_w = ImGui::CalcTextSize(buf).x;
    float bar_w  = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + bar_w - text_w - 8.0f);

    if (mem_mb / mem_max > 0.8f)
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::WARNING);
    else
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::TEXT_DIM);

    ImGui::Text("%s", buf);
    ImGui::PopStyleColor();
}

}
