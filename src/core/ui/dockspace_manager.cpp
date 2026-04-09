#include "dockspace_manager.h"

#include <imgui.h>

namespace convoy
{

void DockSpaceManager::begin_frame()
{
    ImGuiViewport* vp = ImGui::GetMainViewport();

    float menu_h = ImGui::GetFrameHeight();
    ImVec2 pos = {vp->WorkPos.x, vp->WorkPos.y + menu_h};
    ImVec2 size = {vp->WorkSize.x, vp->WorkSize.y - menu_h};

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                             ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##ConvoyShell", nullptr, flags);
    ImGui::PopStyleVar(3);

    if (!initialized_)
    {
        initialized_ = true;
    }
}

void DockSpaceManager::end_frame()
{
    ImGui::End();
}

ImGuiID DockSpaceManager::get_zone_id(DockZone) const
{
    return 0;
}

void DockSpaceManager::build_initial_layout(ImGuiID) {}

}  // namespace convoy