#pragma once

#include <imgui.h>
#include <string>

namespace convoy {

enum class DockZone {
    Toolbar,    // Left slim bar  (~10% width)
    Canvas,     // Center          (~70%)
    Inspector,  // Right panel     (~20%)
    Bottom,     // Bottom panel    (~15%)
};

enum class LayoutPreset {
    Artisan,      // Drawing default - tools left, canvas center, inspector right, console bottom
    LevelDesign,  // World map editor - layers left, canvas center, tiles right
    Debugging,    // Diagnostics - console bottom, canvas center, props right
};

class DockSpaceManager {
public:
    DockSpaceManager();

    // Call after ImGui::NewFrame(), before module windows
    void begin_frame();
    // Call after all module ImGui::Begin/End pairs
    void end_frame();

    ImGuiID get_zone_id(DockZone zone) const;
    bool is_initialized() const { return initialized_; }

    // Layout management
    void set_initialized() { initialized_ = true; }
    void build_initial_layout();
    void switch_preset(LayoutPreset preset);
    void reset_to_default();

    // Persistence
    void save_layout();
    void load_layout();

    LayoutPreset current_preset() const { return current_preset_; }

private:
    void build_artisan_layout();
    void build_level_design_layout();
    void build_debugging_layout();

    ImGuiID root_id_       = 0;
    ImGuiID zone_toolbar_  = 0;
    ImGuiID zone_canvas_   = 0;
    ImGuiID zone_inspect_  = 0;
    ImGuiID zone_bottom_   = 0;
    bool    initialized_   = false;
    LayoutPreset current_preset_ = LayoutPreset::Artisan;
};

} // namespace convoy