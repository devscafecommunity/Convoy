#include "brush_settings.h"

#include <imgui.h>

#include <cmath>

namespace convoy
{
namespace architect
{

void BrushSettings::render()
{
    if (!ImGui::Begin("Brush Studio", &is_open_))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("##brush_tabs"))
    {
        if (ImGui::BeginTabItem("Preview"))
        {
            render_preview_tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Mask Settings"))
        {
            render_mask_settings_tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Mask Source"))
        {
            render_mask_source_tab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void BrushSettings::render_preview_tab()
{
    ImGui::TextDisabled("Preview");
    ImGui::Separator();

    float preview_size = 128.0f;
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 center = {cursor_pos.x + preview_size / 2, cursor_pos.y + preview_size / 2};
    float radius = current_brush_.size * 2.0f;

    if (current_brush_.has_custom_mask && !current_brush_.custom_mask.empty())
    {
        ImGui::Image((ImTextureID) nullptr, {preview_size, preview_size});
    }
    else
    {
        ImU32 col = IM_COL32(255, 255, 255, 200);
        switch (current_brush_.shape)
        {
            case BrushShape::Circle:
                draw_list->AddCircleFilled(center, radius, col);
                break;
            case BrushShape::Square:
                draw_list->AddRectFilled(
                    {center.x - radius, center.y - radius}, {center.x + radius, center.y + radius}, col);
                break;
            case BrushShape::Diamond:
            {
                ImVec2 pts[4] = {{center.x, center.y - radius},
                                 {center.x + radius, center.y},
                                 {center.x, center.y + radius},
                                 {center.x - radius, center.y}};
                draw_list->AddConvexPolyFilled(pts, 4, col);
                break;
            }
            case BrushShape::Star:
            {
                const int segments = 5;
                ImVec2 pts[segments * 2];
                for (int i = 0; i < segments * 2; ++i)
                {
                    float angle = i * 3.14159f / segments - 3.14159f / 2;
                    float r = (i % 2 == 0) ? radius : radius * 0.5f;
                    pts[i] = {center.x + r * std::cos(angle), center.y + r * std::sin(angle)};
                }
                draw_list->AddConvexPolyFilled(pts, segments * 2, col);
                break;
            }
            case BrushShape::Line:
                draw_list->AddLine({center.x - radius, center.y}, {center.x + radius, center.y}, col, 4.0f);
                break;
            case BrushShape::Custom:
                draw_list->AddCircleFilled(center, radius, col);
                break;
        }
    }

    ImGui::Dummy({preview_size, preview_size});
}

void BrushSettings::render_mask_settings_tab()
{
    ImGui::Text("Shape");
    const char* shapes[] = {"Circle", "Square", "Diamond", "Star", "Line", "Custom"};
    int current_shape = static_cast<int>(current_brush_.shape);
    if (ImGui::Combo("##shape", &current_shape, shapes, 6))
    {
        set_shape(static_cast<BrushShape>(current_shape));
    }

    float size = current_brush_.size;
    if (ImGui::SliderFloat("Size", &size, 1.0f, 64.0f, "%.0f"))
    {
        set_size(static_cast<uint8_t>(size));
    }

    float hardness = current_brush_.hardness;
    if (ImGui::SliderFloat("Hardness", &hardness, 0.0f, 100.0f, "%.0f%%"))
    {
        set_hardness(static_cast<uint8_t>(hardness));
    }

    float spacing = current_brush_.spacing;
    if (ImGui::SliderFloat("Spacing", &spacing, 0.0f, 100.0f, "%.0f%%"))
    {
        set_spacing(static_cast<uint8_t>(spacing));
    }

    ImGui::Separator();
    ImGui::Text("Rotation");

    bool follow_mouse = current_brush_.dynamic_mask.follow_mouse;
    if (ImGui::Checkbox("Follow Mouse", &follow_mouse))
    {
        current_brush_.dynamic_mask.follow_mouse = follow_mouse;
    }

    float rotation = current_brush_.dynamic_mask.rotation;
    if (ImGui::SliderFloat("Angle", &rotation, 0.0f, 360.0f, "%.0f deg"))
    {
        current_brush_.dynamic_mask.rotation = rotation;
    }

    ImGui::Separator();
    ImGui::Text("Pressure Curve");
    const char* curves[] = {"Linear", "Smooth", "Sharp"};
    int current_curve = static_cast<int>(current_brush_.curve);
    if (ImGui::Combo("##curve", &current_curve, curves, 3))
    {
        set_curve(static_cast<PressureCurve>(current_curve));
    }

    ImGui::Separator();
    ImGui::Text("Dynamic Mask");

    int mask_mode = static_cast<int>(current_brush_.dynamic_mask.mode);
    const char* mask_modes[] = {"None", "Pressure expands mask", "Pressure increases opacity"};
    if (ImGui::Combo("##dmode", &mask_mode, mask_modes, 3))
    {
        current_brush_.dynamic_mask.mode = static_cast<DynamicMaskMode>(mask_mode);
        current_brush_.use_dynamic_mask = (mask_mode > 0);
    }
}

void BrushSettings::render_mask_source_tab()
{
    ImGui::Text("Mask Source");
    ImGui::Separator();

    if (ImGui::Button("Parametric (Generate)"))
    {
        auto mask = Brush::generate_parametric_mask(current_brush_.shape, current_brush_.size, current_brush_.hardness);
        set_custom_mask(mask);
        current_brush_.shape = BrushShape::Custom;
    }

    ImGui::SameLine();
    if (ImGui::Button("Grab from Selection"))
    {
    }

    ImGui::SameLine();
    if (ImGui::Button("Import PNG"))
    {
    }

    ImGui::Separator();
    ImGui::Text("Saved Brushes");
    ImGui::TextDisabled("(Brush library coming soon)");
}

void BrushSettings::capture_from_canvas(const Canvas* canvas, int x, int y, int w, int h)
{
    if (!canvas)
        return;

    std::vector<uint8_t> mask(w * h * 4);

    for (int dy = 0; dy < h; ++dy)
    {
        for (int dx = 0; dx < w; ++dx)
        {
            Color c = canvas->get_pixel(x + dx, y + dy);
            uint8_t gray = static_cast<uint8_t>((c.r + c.g + c.b) / 3);
            mask[(dy * w + dx) * 4 + 0] = gray;
            mask[(dy * w + dx) * 4 + 1] = gray;
            mask[(dy * w + dx) * 4 + 2] = gray;
            mask[(dy * w + dx) * 4 + 3] = c.a;
        }
    }

    set_custom_mask(mask);
    set_shape(BrushShape::Custom);
}

bool BrushSettings::import_from_png(const std::string& path)
{
    return false;
}

}  // namespace architect
}  // namespace convoy
