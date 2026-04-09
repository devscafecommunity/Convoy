#include "settings_config.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace convoy
{

SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

void SettingsManager::load_from_file(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return;

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    if (json.find("\"DarkTerminal\"") != std::string::npos)
        env_.theme = Theme::DarkTerminal;
    else if (json.find("\"HighContrast\"") != std::string::npos)
        env_.theme = Theme::HighContrast;
    else if (json.find("\"Custom\"") != std::string::npos)
        env_.theme = Theme::Custom;

    auto find_num = [&json](const char* key, int default_val) -> int
    {
        size_t pos = json.find(key);
        if (pos == std::string::npos)
            return default_val;
        size_t colon = json.find(":", pos);
        if (colon == std::string::npos)
            return default_val;
        size_t end = json.find_first_of(",}", colon);
        std::string val = json.substr(colon + 1, end - colon - 1);
        try
        {
            return std::stoi(val);
        }
        catch (...)
        {
            return default_val;
        }
    };

    env_.auto_save_interval_seconds = find_num("auto_save_interval", 300);
    env_.max_backup_versions = find_num("max_backups", 10);
    env_.independent_viewports = json.find("independent_viewports") != std::string::npos;

    render_.vsync = json.find("vsync") == std::string::npos || json.find("vsync\": true") != std::string::npos;
    render_.sampling = json.find("\"bilinear\"") != std::string::npos  ? SamplingMode::Bilinear
                     : json.find("\"trilinear\"") != std::string::npos ? SamplingMode::Trilinear
                                                                       : SamplingMode::Nearest;

    render_.enable_scanlines = json.find("scanlines") != std::string::npos;
    render_.enable_crt_effect = json.find("crt_effect") != std::string::npos;
    render_.snap_to_pixel_grid = json.find("snap_to_pixel_grid") == std::string::npos ||
                                 json.find("snap_to_pixel_grid\": true") != std::string::npos;

    engine_.caf_export_path = "./exports";
    engine_.memory_alignment = find_num("memory_alignment", 32);
    engine_.auto_export_on_save = json.find("auto_export_on_save") == std::string::npos ||
                                  json.find("auto_export_on_save\": true") != std::string::npos;

    hw_.input_smoothing = 0.0f;
    hw_.min_pressure_threshold = 0.05f;

    if (json.find("\"Junior\"") != std::string::npos)
        current_profile_ = ArtisanProfile::Junior;
    else if (json.find("\"TechnicalArtist\"") != std::string::npos)
        current_profile_ = ArtisanProfile::TechnicalArtist;
    else
        current_profile_ = ArtisanProfile::Custom;

    if (on_changed_)
        on_changed_();
}

void SettingsManager::save_to_file(const std::string& path) const
{
    std::ofstream file(path);
    if (!file.is_open())
        return;

    file << "{\n";
    file << "  \"profile\": \"";

    switch (current_profile_)
    {
        case ArtisanProfile::Junior:
            file << "Junior";
            break;
        case ArtisanProfile::TechnicalArtist:
            file << "TechnicalArtist";
            break;
        case ArtisanProfile::Custom:
            file << "Custom";
            break;
    }

    file << "\",\n";

    file << "  \"environment\": {\n";
    file << "    \"theme\": \"";

    switch (env_.theme)
    {
        case Theme::DarkTerminal:
            file << "DarkTerminal";
            break;
        case Theme::HighContrast:
            file << "HighContrast";
            break;
        case Theme::Custom:
            file << "Custom";
            break;
    }

    file << "\",\n";
    file << "    \"language\": \"" << env_.language << "\",\n";
    file << "    \"auto_save_interval\": " << env_.auto_save_interval_seconds << ",\n";
    file << "    \"max_backups\": " << env_.max_backup_versions << ",\n";
    file << "    \"independent_viewports\": " << (env_.independent_viewports ? "true" : "false") << ",\n";
    file << "    \"docking_sensitivity\": " << env_.docking_sensitivity << "\n";
    file << "  },\n";

    file << "  \"hardware\": {\n";
    file << "    \"tablet_api\": \"";

    switch (hw_.tablet_api)
    {
        case TabletAPI::WindowsInk:
            file << "WindowsInk";
            break;
        case TabletAPI::Wintab:
            file << "Wintab";
            break;
        case TabletAPI::Auto:
            file << "Auto";
            break;
    }

    file << "\",\n";
    file << "    \"stabilization\": " << hw_.input_smoothing << ",\n";
    file << "    \"min_pressure\": " << hw_.min_pressure_threshold << "\n";
    file << "  },\n";

    file << "  \"graphics\": {\n";
    file << "    \"vsync\": " << (render_.vsync ? "true" : "false") << ",\n";
    file << "    \"sampling\": \"";

    switch (render_.sampling)
    {
        case SamplingMode::Nearest:
            file << "nearest";
            break;
        case SamplingMode::Bilinear:
            file << "bilinear";
            break;
        case SamplingMode::Trilinear:
            file << "trilinear";
            break;
    }

    file << "\",\n";
    file << "    \"scanlines\": " << (render_.enable_scanlines ? "true" : "false") << ",\n";
    file << "    \"crt_effect\": " << (render_.enable_crt_effect ? "true" : "false") << ",\n";
    file << "    \"show_rulers\": " << (render_.show_rulers ? "true" : "false") << ",\n";
    file << "    \"snap_to_pixel_grid\": " << (render_.snap_to_pixel_grid ? "true" : "false") << "\n";
    file << "  },\n";

    file << "  \"engine\": {\n";
    file << "    \"caf_export_path\": \"" << engine_.caf_export_path << "\",\n";
    file << "    \"memory_alignment\": " << engine_.memory_alignment << ",\n";
    file << "    \"hot_reload_ip\": \"" << engine_.hot_reload_ip << "\",\n";
    file << "    \"hot_reload_port\": " << engine_.hot_reload_port << ",\n";
    file << "    \"auto_export_on_save\": " << (engine_.auto_export_on_save ? "true" : "false") << "\n";
    file << "  }\n";

    file << "}\n";
}

void SettingsManager::apply_profile(ArtisanProfile profile)
{
    current_profile_ = profile;

    switch (profile)
    {
        case ArtisanProfile::Junior:
            env_.independent_viewports = false;
            env_.docking_sensitivity = 0.8f;
            render_.show_rulers = true;
            render_.snap_to_pixel_grid = true;
            hw_.input_smoothing = 0.3f;
            break;

        case ArtisanProfile::TechnicalArtist:
            env_.independent_viewports = true;
            env_.docking_sensitivity = 0.3f;
            render_.show_rulers = true;
            render_.snap_to_pixel_grid = true;
            render_.dod_overlay_opacity = 0.5f;
            hw_.input_smoothing = 0.0f;
            break;

        case ArtisanProfile::Custom:
            break;
    }

    if (on_changed_)
        on_changed_();
}

}  // namespace convoy
