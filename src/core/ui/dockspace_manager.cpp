#include "dockspace_manager.h"
#include <imgui_internal.h>

namespace convoy {

void DockSpaceManager::begin_frame() {
    ImGuiViewport* vp = ImGui::GetMainViewport();

    float menu_h = ImGui::GetFrameHeight();
    ImVec2 pos  = {vp->WorkPos.x,  vp->WorkPos.y + menu_h};
    ImVec2 size = {vp->WorkSize.x, vp->WorkSize.y - menu_h};

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar       | ImGuiWindowFlags_NoCollapse  |
        ImGuiWindowFlags_NoResize         | ImGuiWindowFlags_NoMove      |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking        | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0, 0));
    ImGui::Begin("##ConvoyShell", nullptr, flags);
    ImGui::PopStyleVar(3);

    root_id_ = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(root_id_, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

    if (!initialized_) {
        build_initial_layout(root_id_);
        initialized_ = true;
    }
}

void DockSpaceManager::end_frame() {
    ImGui::End();
}

ImGuiID DockSpaceManager::get_zone_id(DockZone zone) const {
    switch (zone) {
        case DockZone::ToolPalette: return zone_tools_;
        case DockZone::MainCanvas:  return zone_canvas_;
        case DockZone::Inspector:   return zone_inspect_;
    }
    return root_id_;
}

void DockSpaceManager::build_initial_layout(ImGuiID root_id) {
    ImGui::DockBuilderRemoveNode(root_id);
    ImGui::DockBuilderAddNode(root_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(root_id, ImGui::GetMainViewport()->WorkSize);

    ImGuiID remaining = root_id;

    ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Left, 0.08f, &zone_tools_, &remaining);
    ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.26f, &zone_inspect_, &remaining);
    zone_canvas_ = remaining;

    ImGui::DockBuilderFinish(root_id);
}

}
