#include "dockspace_manager.h"

#include <imgui.h>
#include <fstream>

namespace convoy {

DockSpaceManager::DockSpaceManager() = default;

void DockSpaceManager::begin_frame() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    float menu_h = ImGui::GetFrameHeight();
    ImVec2 pos = {vp->WorkPos.x, vp->WorkPos.y + menu_h};
    ImVec2 size = {vp->WorkSize.x, vp->WorkSize.y - menu_h};

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##ConvoyShell", nullptr, flags);
    ImGui::PopStyleVar(3);

    render_snap_zone_overlays();
}

void DockSpaceManager::end_frame() {
    ImGui::End();
}

ImGuiID DockSpaceManager::get_zone_id(DockZone zone) const {
    switch (zone) {
        case DockZone::Toolbar:   return zone_toolbar_;
        case DockZone::Canvas:    return zone_canvas_;
        case DockZone::Inspector: return zone_inspect_;
        case DockZone::Bottom:    return zone_bottom_;
    }
    return 0;
}

void DockSpaceManager::build_initial_layout() {
    current_preset_ = LayoutPreset::Artisan;
    initialized_ = true;
}

void DockSpaceManager::build_artisan_layout() {
    current_preset_ = LayoutPreset::Artisan;
}

void DockSpaceManager::build_level_design_layout() {
    current_preset_ = LayoutPreset::LevelDesign;
}

void DockSpaceManager::build_debugging_layout() {
    current_preset_ = LayoutPreset::Debugging;
}

void DockSpaceManager::switch_preset(LayoutPreset preset) {
    if (current_preset_ == preset) return;
    switch (preset) {
        case LayoutPreset::Artisan:     build_artisan_layout(); break;
        case LayoutPreset::LevelDesign: build_level_design_layout(); break;
        case LayoutPreset::Debugging:   build_debugging_layout(); break;
    }
}

void DockSpaceManager::reset_to_default() {
    build_artisan_layout();
}

void DockSpaceManager::save_layout() {
}

void DockSpaceManager::load_layout() {
    initialized_ = false;
}

void DockSpaceManager::render_snap_zone_overlays() {
    if (!show_snap_zones_) return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    float menu_h = ImGui::GetFrameHeight();
    float x0 = vp->WorkPos.x;
    float y0 = vp->WorkPos.y + menu_h;
    float w  = vp->WorkSize.x;
    float h  = vp->WorkSize.y - menu_h;

    float toolbar_w   = w * 0.10f;
    float inspector_w = w * 0.20f;
    float bottom_h    = h * 0.15f;

    ImU32 zone_col  = IM_COL32(64, 160, 255, 140);
    ImU32 label_col = IM_COL32(64, 160, 255, 210);

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    float t = 2.0f;

    dl->AddRect({x0,                    y0},          {x0 + toolbar_w,             y0 + h - bottom_h}, zone_col, 0.0f, 0, t);
    dl->AddRect({x0 + toolbar_w,        y0},          {x0 + w - inspector_w,       y0 + h - bottom_h}, zone_col, 0.0f, 0, t);
    dl->AddRect({x0 + w - inspector_w,  y0},          {x0 + w,                     y0 + h - bottom_h}, zone_col, 0.0f, 0, t);
    dl->AddRect({x0,                    y0 + h - bottom_h}, {x0 + w,               y0 + h},            zone_col, 0.0f, 0, t);

    dl->AddText({x0 + 4,                    y0 + 4},              label_col, "Toolbar");
    dl->AddText({x0 + toolbar_w + 4,        y0 + 4},              label_col, "Canvas");
    dl->AddText({x0 + w - inspector_w + 4,  y0 + 4},              label_col, "Inspector");
    dl->AddText({x0 + 4,                    y0 + h - bottom_h + 4}, label_col, "Console");
}

} // namespace convoy
