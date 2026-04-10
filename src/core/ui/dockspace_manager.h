#pragma once

#include <imgui.h>
#include <string>

namespace convoy {

enum class DockZone {
    Toolbar,
    Canvas,
    Inspector,
    Bottom,
};

enum class LayoutPreset {
    Artisan,
    LevelDesign,
    Debugging,
};

class DockSpaceManager {
public:
    DockSpaceManager();

    void begin_frame();
    void end_frame();

    ImGuiID get_zone_id(DockZone zone) const;
    bool is_initialized() const { return initialized_; }

    void set_initialized() { initialized_ = true; }
    void build_initial_layout();
    void switch_preset(LayoutPreset preset);
    void reset_to_default();

    void save_layout();
    void load_layout();

    void toggle_snap_zones() { show_snap_zones_ = !show_snap_zones_; }
    bool snap_zones_visible() const { return show_snap_zones_; }

    LayoutPreset current_preset() const { return current_preset_; }

private:
    void build_artisan_layout();
    void build_level_design_layout();
    void build_debugging_layout();
    void render_snap_zone_overlays();

    ImGuiID root_id_       = 0;
    ImGuiID zone_toolbar_  = 0;
    ImGuiID zone_canvas_   = 0;
    ImGuiID zone_inspect_  = 0;
    ImGuiID zone_bottom_   = 0;
    bool    initialized_   = false;
    bool    show_snap_zones_ = false;
    LayoutPreset current_preset_ = LayoutPreset::Artisan;
};

} // namespace convoy
