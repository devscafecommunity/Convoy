#include "pressure_curve_editor.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>

namespace convoy
{

void PressureCurveEditor::render()
{
    if (!is_open_)
        return;

    ImGui::SetNextWindowSize({400, 350});
    if (!ImGui::Begin("Pressure Curve Editor", &is_open_))
    {
        ImGui::End();
        return;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    float canvas_w = 350;
    float canvas_h = 250;
    float padding = 20;

    ImVec2 origin = {canvas_pos.x + padding, canvas_pos.y + padding};
    float graph_w = canvas_w - padding * 2;
    float graph_h = canvas_h - padding * 2;

    ImU32 axis_col = IM_COL32(100, 100, 100, 255);
    ImU32 grid_col = IM_COL32(50, 50, 50, 100);
    ImU32 line_col = IM_COL32(0, 255, 65, 255);
    ImU32 point_col = IM_COL32(255, 255, 255, 255);
    ImU32 handle_col = IM_COL32(0, 255, 65, 200);

    draw_list->AddRectFilled(origin, {origin.x + graph_w, origin.y + graph_h}, IM_COL32(20, 20, 20, 255));

    for (int i = 0; i <= 4; ++i)
    {
        float x = origin.x + (graph_w / 4.0f) * i;
        float y = origin.y + (graph_h / 4.0f) * i;
        draw_list->AddLine({x, origin.y}, {x, origin.y + graph_h}, grid_col);
        draw_list->AddLine({origin.x, y}, {origin.x + graph_w, y}, grid_col);
    }

    draw_list->AddLine({origin.x, origin.y}, {origin.x + graph_w, origin.y}, axis_col, 2);
    draw_list->AddLine({origin.x, origin.y + graph_h}, {origin.x + graph_w, origin.y + graph_h}, axis_col, 2);
    draw_list->AddLine({origin.x, origin.y}, {origin.x, origin.y + graph_h}, axis_col, 2);
    draw_list->AddLine({origin.x + graph_w, origin.y}, {origin.x + graph_w, origin.y + graph_h}, axis_col, 2);

    if (curve_points_.size() >= 2)
    {
        for (size_t i = 0; i < curve_points_.size() - 1; ++i)
        {
            ImVec2 p1 = {origin.x + curve_points_[i].x * graph_w, origin.y + (1.0f - curve_points_[i].y) * graph_h};
            ImVec2 p2 = {origin.x + curve_points_[i + 1].x * graph_w,
                         origin.y + (1.0f - curve_points_[i + 1].y) * graph_h};
            draw_list->AddLine(p1, p2, line_col, 2);
        }
    }

    for (size_t i = 0; i < curve_points_.size(); ++i)
    {
        ImVec2 pt = {origin.x + curve_points_[i].x * graph_w, origin.y + (1.0f - curve_points_[i].y) * graph_h};
        draw_list->AddCircleFilled(pt, 8, point_col);
        draw_list->AddCircleFilled(pt, 6, handle_col);
    }

    ImGui::Dummy({canvas_w, canvas_h});

    ImGui::Text("Drag points to adjust curve");
    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        curve_points_ = {{0.0f, 0.0f}, {0.25f, 0.25f}, {0.5f, 0.5f}, {0.75f, 0.75f}, {1.0f, 1.0f}};
        if (on_curve_changed)
            on_curve_changed(curve_points_);
    }

    ImGui::SameLine();

    if (ImGui::Button("Apply"))
    {
        if (on_curve_changed)
            on_curve_changed(curve_points_);
    }

    ImGui::End();
}

}  // namespace convoy
