#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace convoy
{

enum class Theme
{
    DarkTerminal,
    HighContrast,
    Custom
};

enum class TabletAPI
{
    WindowsInk,
    Wintab,
    Auto
};

enum class SamplingMode
{
    Nearest,
    Bilinear,
    Trilinear
};

enum class ArtisanProfile
{
    Junior,
    TechnicalArtist,
    Custom
};

struct EnvironmentSettings
{
    Theme theme = Theme::DarkTerminal;
    std::string language = "en";
    int auto_save_interval_seconds = 300;
    int max_backup_versions = 10;
    bool independent_viewports = false;
    float docking_sensitivity = 0.5f;
};

struct HardwareSettings
{
    TabletAPI tablet_api = TabletAPI::Auto;
    std::string pressure_curve_json;
    float input_smoothing = 0.0f;
    float min_pressure_threshold = 0.05f;
};

struct RenderingSettings
{
    bool vsync = true;
    SamplingMode sampling = SamplingMode::Nearest;
    bool enable_scanlines = false;
    bool enable_crt_effect = false;
    float dod_overlay_opacity = 0.5f;
    bool show_rulers = true;
    bool snap_to_pixel_grid = true;
};

struct EngineSettings
{
    std::string caf_export_path = "./exports";
    int memory_alignment = 32;
    std::string hot_reload_ip = "127.0.0.1";
    int hot_reload_port = 7890;
    bool auto_export_on_save = true;
};

struct KeybindEntry
{
    std::string action;
    std::string key;
    std::string modifiers;
};

struct KeybindSettings
{
    std::vector<KeybindEntry> bindings;
};

class SettingsManager
{
   public:
    static SettingsManager& instance();

    EnvironmentSettings& env() { return env_; }
    HardwareSettings& hw() { return hw_; }
    RenderingSettings& render() { return render_; }
    EngineSettings& engine() { return engine_; }
    KeybindSettings& keybinds() { return keybinds_; }

    const EnvironmentSettings& env() const { return env_; }
    const HardwareSettings& hw() const { return hw_; }
    const RenderingSettings& render() const { return render_; }
    const EngineSettings& engine() const { return engine_; }
    const KeybindSettings& keybinds() const { return keybinds_; }

    void load_from_file(const std::string& path);
    void save_to_file(const std::string& path) const;

    void apply_profile(ArtisanProfile profile);
    ArtisanProfile get_current_profile() const { return current_profile_; }

    void set_on_changed(std::function<void()> callback) { on_changed_ = callback; }

   private:
    SettingsManager() = default;
    EnvironmentSettings env_;
    HardwareSettings hw_;
    RenderingSettings render_;
    EngineSettings engine_;
    KeybindSettings keybinds_;
    ArtisanProfile current_profile_ = ArtisanProfile::TechnicalArtist;
    std::function<void()> on_changed_;
};

}  // namespace convoy
