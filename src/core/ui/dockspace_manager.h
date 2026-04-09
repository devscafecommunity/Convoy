#pragma once
#include <imgui.h>

namespace convoy {

enum class DockZone {
    ToolPalette,  // Left slim bar  (~8% width)
    MainCanvas,   // Center          (~68%)
    Inspector,    // Right panel     (~24%)
};

class DockSpaceManager {
public:
    // Call after ImGui::NewFrame(), before module windows
    void begin_frame();
    // Call after all module ImGui::Begin/End pairs
    void end_frame();

    ImGuiID get_zone_id(DockZone zone) const;
    bool is_initialized() const { return initialized_; }

private:
    void build_initial_layout(ImGuiID root_id);

    ImGuiID root_id_      = 0;
    ImGuiID zone_tools_   = 0;
    ImGuiID zone_canvas_  = 0;
    ImGuiID zone_inspect_ = 0;
    bool    initialized_  = false;
};

} // namespace convoy
