#include "walker_ui.h"
#include <imgui.h>

namespace convoy { namespace walker {

void WalkerUI::render() {
    ImGui::Begin("Walker: World Builder##Walker");
    ImGui::TextDisabled("Walker module - Fase 3 (Set/Nov 2026)");
    ImGui::Separator();

    ImGui::Text("Grid Size:");
    ImGui::SameLine();
    ImGui::RadioButton("16x16", &grid_size_, 16);
    ImGui::SameLine();
    ImGui::RadioButton("32x32", &grid_size_, 32);

    ImGui::Checkbox("Debug Physics (Collision Overlay)", &show_collision_);
    ImGui::Separator();

    render_tile_grid_placeholder();
    ImGui::End();

    render_tile_selector();
}

void WalkerUI::render_tile_grid_placeholder() {
    ImVec2 avail  = ImGui::GetContentRegionAvail();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin  = ImGui::GetCursorScreenPos();

    float step = static_cast<float>(grid_size_);
    ImU32 col  = show_collision_
        ? IM_COL32(255, 144, 0, 60)
        : IM_COL32(42, 42, 42, 120);

    for (float x = 0; x < avail.x; x += step)
        dl->AddLine({origin.x + x, origin.y},
                    {origin.x + x, origin.y + avail.y}, col);
    for (float y = 0; y < avail.y; y += step)
        dl->AddLine({origin.x, origin.y + y},
                    {origin.x + avail.x, origin.y + y}, col);

    ImGui::Dummy(avail);
}

void WalkerUI::render_tile_selector() {
    ImGui::Begin("Tile Selector##Walker");
    ImGui::TextDisabled("Tile assets will appear here");
    ImGui::End();
}

}}
