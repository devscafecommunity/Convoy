#include "preferences_window.h"

#include <imgui.h>

#include <cstring>

#include "../keybind_manager.h"
#include "../plugins/plugin_registry.h"
#include "../settings_config.h"

namespace convoy
{

void PreferencesWindow::render()
{
    if (!is_open_)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        {io.DisplaySize.x * 0.5f - 350.0f, io.DisplaySize.y * 0.5f - 250.0f},
        ImGuiCond_Once);
    ImGui::SetNextWindowSize({700.0f, 500.0f}, ImGuiCond_Once);
    if (!ImGui::Begin("Preferences", &is_open_))
    {
        ImGui::End();
        return;
    }

    ImGui::PushItemWidth(200);
    ImGui::InputText("##search", search_buffer_, sizeof(search_buffer_));
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::TextDisabled("(Search)");

    ImGui::Separator();

    if (ImGui::BeginChild("categories", {150, 0}))
    {
        if (ImGui::Selectable("General", selected_category_ == 0))
            selected_category_ = 0;
        if (ImGui::Selectable("Input", selected_category_ == 1))
            selected_category_ = 1;
        if (ImGui::Selectable("Graphics", selected_category_ == 2))
            selected_category_ = 2;
        if (ImGui::Selectable("Engine", selected_category_ == 3))
            selected_category_ = 3;
        if (ImGui::Selectable("Plugins", selected_category_ == 4))
            selected_category_ = 4;
        if (ImGui::Selectable("Keybinds", selected_category_ == 5))
            selected_category_ = 5;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if (ImGui::BeginChild("settings", {0, 0}))
    {
        switch (selected_category_)
        {
            case 0:
                render_general_tab();
                break;
            case 1:
                render_input_tab();
                break;
            case 2:
                render_graphics_tab();
                break;
            case 3:
                render_engine_tab();
                break;
            case 4:
                render_plugins_tab();
                break;
            case 5:
                render_keybinds_tab();
                break;
        }
    }
    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("Reset Defaults"))
    {
        if (on_reset_)
            on_reset_();
    }

    ImGui::SameLine();

    if (ImGui::Button("Apply"))
    {
        if (on_apply_)
            on_apply_();
    }

    ImGui::SameLine();

    if (ImGui::Button("Close"))
    {
        close();
    }

    ImGui::End();
}

void PreferencesWindow::render_general_tab()
{
    auto& settings = SettingsManager::instance();
    auto& env = settings.env();

    ImGui::Text("Environment");
    ImGui::Separator();

    ImGui::Text("Theme");
    const char* themes[] = {"Dark Terminal", "High Contrast", "Custom"};
    int theme_idx = static_cast<int>(env.theme);
    ImGui::Combo("##theme", &theme_idx, themes, 3);
    env.theme = static_cast<Theme>(theme_idx);

    static char lang_buffer[32] = {};
    std::strncpy(lang_buffer, env.language.c_str(), sizeof(lang_buffer) - 1);
    ImGui::InputText("##lang", lang_buffer, sizeof(lang_buffer));
    env.language = lang_buffer;

    ImGui::Text("Auto-Save Interval (seconds)");
    int auto_save = env.auto_save_interval_seconds;
    ImGui::InputInt("##autosave", &auto_save, 30, 60);
    env.auto_save_interval_seconds = auto_save;

    ImGui::Text("Max Backup Versions");
    int max_backups = env.max_backup_versions;
    ImGui::InputInt("##backups", &max_backups, 1, 5);
    env.max_backup_versions = max_backups;

    ImGui::Separator();
    ImGui::Text("Multi-Window");

    ImGui::Checkbox("Independent Viewports", &env.independent_viewports);

    ImGui::Text("Docking Sensitivity");
    ImGui::SliderFloat("##docking", &env.docking_sensitivity, 0.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Artisan Profile");

    const char* profiles[] = {"Junior", "Technical Artist", "Custom"};
    int profile_idx = static_cast<int>(settings.get_current_profile());
    ImGui::Combo("##profile", &profile_idx, profiles, 3);
    settings.apply_profile(static_cast<ArtisanProfile>(profile_idx));
}

void PreferencesWindow::render_input_tab()
{
    auto& settings = SettingsManager::instance();
    auto& hw = settings.hw();

    ImGui::Text("Tablet Settings");
    ImGui::Separator();

    const char* tablet_apis[] = {"Windows Ink", "Wintab", "Auto"};
    int api_idx = static_cast<int>(hw.tablet_api);
    ImGui::Combo("API", &api_idx, tablet_apis, 3);
    hw.tablet_api = static_cast<TabletAPI>(api_idx);

    ImGui::Text("Pressure Curve");
    if (ImGui::Button("Edit Spline"))
    {
    }

    ImGui::Text("Min Pressure Threshold");
    ImGui::SliderFloat("##minpressure", &hw.min_pressure_threshold, 0.0f, 0.5f);

    ImGui::Separator();
    ImGui::Text("Canvas Behavior");

    auto& render = settings.render();
    ImGui::Checkbox("Snap to Pixel Grid", &render.snap_to_pixel_grid);
    ImGui::Checkbox("Show Rulers", &render.show_rulers);

    ImGui::Separator();
    ImGui::Text("Input Smoothing");
    ImGui::SliderFloat("##smoothing", &hw.input_smoothing, 0.0f, 1.0f);
}

void PreferencesWindow::render_graphics_tab()
{
    auto& settings = SettingsManager::instance();
    auto& render = settings.render();

    ImGui::Text("Display");
    ImGui::Separator();

    ImGui::Checkbox("VSync", &render.vsync);

    ImGui::Text("Canvas Sampling");
    const char* sampling_modes[] = {"Nearest Neighbor", "Bilinear", "Trilinear"};
    int samp_idx = static_cast<int>(render.sampling);
    ImGui::Combo("##sampling", &samp_idx, sampling_modes, 3);
    render.sampling = static_cast<SamplingMode>(samp_idx);

    ImGui::Separator();
    ImGui::Text("Post-Processing");

    ImGui::Checkbox("Scanlines", &render.enable_scanlines);
    ImGui::Checkbox("CRT Effect", &render.enable_crt_effect);

    ImGui::Separator();
    ImGui::Text("DOD Debug Overlay");

    ImGui::SliderFloat("Opacity", &render.dod_overlay_opacity, 0.0f, 1.0f);
}

void PreferencesWindow::render_engine_tab()
{
    auto& settings = SettingsManager::instance();
    auto& engine = settings.engine();

    ImGui::Text("Caffeine Engine Sync");
    ImGui::Separator();

    static char path_buffer[256] = {};
    std::strncpy(path_buffer, engine.caf_export_path.c_str(), sizeof(path_buffer) - 1);
    ImGui::InputText("##exportpath", path_buffer, sizeof(path_buffer));
    engine.caf_export_path = path_buffer;

    ImGui::Text("Memory Alignment (bytes)");
    const char* aligns[] = {"32", "64", "128"};
    static int align_idx = 0;
    if (engine.memory_alignment == 64)
        align_idx = 1;
    else if (engine.memory_alignment == 128)
        align_idx = 2;
    ImGui::Combo("##alignment", &align_idx, aligns, 3);
    engine.memory_alignment = (align_idx == 0) ? 32 : (align_idx == 1) ? 64 : 128;

    ImGui::Checkbox("Auto-export on Save", &engine.auto_export_on_save);

    ImGui::Separator();
    ImGui::Text("Hot-Reload Socket");

    static char ip_buffer[64] = {};
    std::strncpy(ip_buffer, engine.hot_reload_ip.c_str(), sizeof(ip_buffer) - 1);
    ImGui::InputText("##ip", ip_buffer, sizeof(ip_buffer));
    engine.hot_reload_ip = ip_buffer;

    ImGui::Text("Port");
    int port = engine.hot_reload_port;
    ImGui::InputInt("##port", &port);
    engine.hot_reload_port = port;
}

void PreferencesWindow::render_plugins_tab()
{
    ImGui::Text("Plugin Settings");
    ImGui::Separator();

    if (plugin_sections_.empty())
    {
        ImGui::TextDisabled("No plugins with settings");
    }
    else
    {
        for (auto* section : plugin_sections_)
        {
            if (ImGui::CollapsingHeader(section->get_name().c_str()))
            {
                section->render();
            }
        }
    }

    ImGui::Separator();
    ImGui::Text("Installed Plugins");
    ImGui::Separator();

    static PluginRegistry* registry = nullptr;
    if (!registry)
    {
    }

    ImGui::TextDisabled("(Plugin registry not connected)");
    ImGui::Separator();

    if (ImGui::Button("Refresh Plugins"))
    {
    }
}

void PreferencesWindow::render_keybinds_tab()
{
    auto& km = KeybindManager::instance();
    auto& keybinds = km.get_all_keybinds();

    static int selected_category = 0;
    const char* categories[] = {"File", "Edit", "View", "Tools", "Modules", "Zoom"};

    ImGui::Combo("Category", &selected_category, categories, 6);

    ImGui::Separator();

    if (ImGui::BeginChild("keybinds_list", {0, 0}))
    {
        for (const auto& kb : keybinds)
        {
            if (kb.category != categories[selected_category])
                continue;

            std::string key_display = kb.current_modifiers + (kb.current_modifiers.empty() ? "" : "+") + kb.current_key;
            ImGui::Text("%s", kb.display_name.c_str());
            ImGui::SameLine(150);
            ImGui::TextDisabled("%s", key_display.c_str());
            ImGui::SameLine();
            if (ImGui::Button(("Reset##" + kb.id).c_str()))
            {
                km.reset_keybind(kb.id);
            }
        }
    }
    ImGui::EndChild();

    ImGui::Separator();

    if (ImGui::Button("Reset All to Defaults"))
    {
        km.reset_all_keybinds();
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(Click key then press new key to rebind)");
}

void PreferencesWindow::register_plugin_section(ISettingsSection* section)
{
    plugin_sections_.push_back(section);
}

}  // namespace convoy
