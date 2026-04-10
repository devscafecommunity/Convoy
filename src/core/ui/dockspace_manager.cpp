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
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##ConvoyShell", nullptr, flags);
    ImGui::PopStyleVar(3);

    if (!initialized_) {
        build_initial_layout();
    }
}

void DockSpaceManager::end_frame() {
    ImGui::End();
}

ImGuiID DockSpaceManager::get_zone_id(DockZone zone) const {
    switch (zone) {
        case DockZone::Toolbar:  return zone_toolbar_;
        case DockZone::Canvas:   return zone_canvas_;
        case DockZone::Inspector: return zone_inspect_;
        case DockZone::Bottom:  return zone_bottom_;
    }
    return 0;
}

void DockSpaceManager::build_initial_layout() {
    ImGuiID main_id = ImGui::GetMainViewport()->ID;
    ImGui::DockBuilderRemoveNode(main_id);
    ImGui::DockBuilderAddNode(main_id, ImGuiDockNodeFlags_DockSpace);
    root_id_ = main_id;

    build_artisan_layout();
    initialized_ = true;
}

void DockSpaceManager::build_artisan_layout() {
    ImGuiID remaining = root_id_;

    ImGuiID id_right = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.20f, nullptr, &remaining);
    ImGuiID id_left = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Left, 0.10f, nullptr, &remaining);
    ImGuiID id_bottom = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Down, 0.15f, nullptr, &remaining);

    ImGui::DockBuilderDockWindow("Architect", remaining);
    ImGui::DockBuilderDockWindow("Inspector", id_right);
    ImGui::DockBuilderDockWindow("Toolbar", id_left);
    ImGui::DockBuilderDockWindow("Console", id_bottom);

    ImGui::DockBuilderFinish(root_id_);

    zone_toolbar_ = id_left;
    zone_canvas_ = remaining;
    zone_inspect_ = id_right;
    zone_bottom_ = id_bottom;
    current_preset_ = LayoutPreset::Artisan;
}

void DockSpaceManager::build_level_design_layout() {
    ImGui::DockBuilderRemoveNode(root_id_);
    ImGui::DockBuilderAddNode(root_id_, ImGuiDockNodeFlags_DockSpace);

    ImGuiID remaining = root_id_;

    ImGuiID id_left = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Left, 0.20f, nullptr, &remaining);
    ImGuiID id_right = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.20f, nullptr, &remaining);

    ImGui::DockBuilderDockWindow("Layers", id_left);
    ImGui::DockBuilderDockWindow("Canvas", remaining);
    ImGui::DockBuilderDockWindow("Tiles", id_right);

    ImGui::DockBuilderFinish(root_id_);

    zone_toolbar_ = id_left;
    zone_canvas_ = remaining;
    zone_inspect_ = id_right;
    zone_bottom_ = 0;
    current_preset_ = LayoutPreset::LevelDesign;
}

void DockSpaceManager::build_debugging_layout() {
    ImGui::DockBuilderRemoveNode(root_id_);
    ImGui::DockBuilderAddNode(root_id_, ImGuiDockNodeFlags_DockSpace);

    ImGuiID remaining = root_id_;

    ImGuiID id_bottom = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Down, 0.30f, nullptr, &remaining);
    ImGuiID id_right = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.25f, nullptr, &remaining);

    ImGui::DockBuilderDockWindow("Canvas", remaining);
    ImGui::DockBuilderDockWindow("Properties", id_right);
    ImGui::DockBuilderDockWindow("Console", id_bottom);

    ImGui::DockBuilderFinish(root_id_);

    zone_toolbar_ = 0;
    zone_canvas_ = remaining;
    zone_inspect_ = id_right;
    zone_bottom_ = id_bottom;
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

}  // namespace