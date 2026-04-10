#include "new_project_dialog.h"

#include <imgui.h>

namespace convoy
{

void NewProjectDialog::render()
{
    if (!open_)
        return;

    ImGui::SetNextWindowSize({500, 400});
    if (!ImGui::Begin("Create New Project", &open_))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("##tabs"))
    {
        if (ImGui::BeginTabItem("Canvas Settings"))
        {
            ImGui::Text("Canvas Dimensions");
            ImGui::PushID(1);
            ImGui::InputScalar("Width", ImGuiDataType_U32, &width_);
            ImGui::InputScalar("Height", ImGuiDataType_U32, &height_);
            ImGui::PopID();

            ImGui::Separator();
            ImGui::Text("Tile Settings");
            ImGui::PushID(2);
            ImGui::InputScalar("Tile Width", ImGuiDataType_U32, &tile_w_);
            ImGui::InputScalar("Tile Height", ImGuiDataType_U32, &tile_h_);
            ImGui::PopID();

            ImGui::Separator();
            ImGui::Text("Bit Depth");
            const char* bit_depths[] = {"16-bit", "32-bit"};
            int bd = (bit_depth_ == 32) ? 1 : 0;
            if (ImGui::Combo("##bitdepth", &bd, bit_depths, 2))
            {
                bit_depth_ = (bd == 1) ? 32 : 16;
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Mode Config"))
        {
            ImGui::Text("Project Mode");
            ImGui::RadioButton("Pixel Art", (int*)&mode_, 0);
            ImGui::RadioButton("Drawing (Hi-Res)", (int*)&mode_, 1);
            ImGui::RadioButton("Isometric", (int*)&mode_, 2);
            ImGui::RadioButton("Top-Down", (int*)&mode_, 3);

            ImGui::Separator();
            ImGui::Text("Snapping");
            ImGui::Checkbox("Snap to Grid", &snap_grid_);
            ImGui::Checkbox("Snap to Angle", &snap_angle_);
            if (snap_angle_)
            {
                ImGui::SliderFloat("Angle", &angle_, 0.0f, 90.0f, "%.0f deg");
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Separator();
    if (ImGui::Button("Cancel"))
    {
        close();
    }
    ImGui::SameLine();
    if (ImGui::Button("Create!"))
    {
        ProjectModeConfig cfg;
        cfg.mode = mode_;
        cfg.canvas_width = width_;
        cfg.canvas_height = height_;
        cfg.grid.tile_width = tile_w_;
        cfg.grid.tile_height = tile_h_;
        cfg.bit_depth = bit_depth_;
        cfg.snapping.snap_to_grid = snap_grid_;
        cfg.snapping.snap_to_angle = snap_angle_;
        cfg.snapping.snap_angle = angle_;

        if (on_create_)
            on_create_(cfg);
        close();
    }

    ImGui::End();
}

}  // namespace convoy
