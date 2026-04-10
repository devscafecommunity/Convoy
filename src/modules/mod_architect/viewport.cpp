#include "viewport.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>

#include "canvas.h"

namespace convoy
{
namespace architect
{

Viewport::Viewport(float width, float height) : width_(width), height_(height), zoom_(1.0f), offset_(0.0f, 0.0f) {}

void Viewport::set_zoom(float zoom)
{
    zoom_ = std::min(32.0f, std::max(0.1f, zoom));
}

float Viewport::get_zoom() const
{
    return zoom_;
}

void Viewport::pan(float dx, float dy)
{
    offset_.x += dx;
    offset_.y += dy;
}

Vec2 Viewport::get_offset() const
{
    return offset_;
}

Vec2 Viewport::screen_to_canvas(float screen_x, float screen_y) const
{
    float canvas_x = (screen_x - offset_.x) / zoom_;
    float canvas_y = (screen_y - offset_.y) / zoom_;
    return Vec2(canvas_x, canvas_y);
}

Vec2 Viewport::canvas_to_screen(float canvas_x, float canvas_y) const
{
    float screen_x = (canvas_x * zoom_) + offset_.x;
    float screen_y = (canvas_y * zoom_) + offset_.y;
    return Vec2(screen_x, screen_y);
}

void Viewport::resize(float width, float height)
{
    width_ = width;
    height_ = height;
}

void Viewport::zoom_to_point(float screen_x, float screen_y, float factor)
{
    Vec2 pre = screen_to_canvas(screen_x, screen_y);
    set_zoom(zoom_ * factor);
    offset_.x = screen_x - pre.x * zoom_;
    offset_.y = screen_y - pre.y * zoom_;
}

void Viewport::fit_to_canvas(const Canvas* canvas)
{
    if (!canvas)
        return;

    float canvas_w = static_cast<float>(canvas->width());
    float canvas_h = static_cast<float>(canvas->height());

    float scale_x = width_ / (canvas_w * 1.05f);
    float scale_y = height_ / (canvas_h * 1.05f);

    set_zoom(std::min(scale_x, scale_y));

    float center_x = (width_ - canvas_w * zoom_) * 0.5f;
    float center_y = (height_ - canvas_h * zoom_) * 0.5f;

    offset_ = Vec2(center_x, center_y);
}

void Viewport::set_mode(ProjectMode mode)
{
    mode_ = mode;
}

void Viewport::set_grid_settings(const GridSettings& grid)
{
    grid_ = grid;
}

void Viewport::set_snap_settings(const SnapSettings& snap)
{
    snap_ = snap;
}

Vec2 Viewport::snap_to_grid(const Vec2& pos) const
{
    if (!snap_.enabled || !snap_.snap_to_grid)
        return pos;

    float tw = static_cast<float>(grid_.tile_width);
    float th = static_cast<float>(grid_.tile_height);

    if (grid_.type == GridType::Diamond)
    {
        return pos;
    }

    float snapped_x = std::round(pos.x / tw) * tw;
    float snapped_y = std::round(pos.y / th) * th;
    return {snapped_x, snapped_y};
}

Vec2 Viewport::snap_to_angle(const Vec2& pos) const
{
    if (!snap_.enabled || !snap_.snap_to_angle)
        return pos;

    float angle = snap_.snap_angle * 3.14159f / 180.0f;
    float snapped = std::round(angle / (3.14159f / 8.0f)) * (3.14159f / 8.0f);
    return pos;
}

void Viewport::render_grid(ImDrawList* draw_list, ImVec2 origin, float canvas_width, float canvas_height)
{
    if (grid_.type == GridType::Hidden)
        return;

    ImU32 grid_col = IM_COL32(42, 42, 42, 100);
    float tw = grid_.tile_width * zoom_;
    float th = grid_.tile_height * zoom_;

    if (grid_.type == GridType::Diamond)
    {
        for (float y = 0; y < canvas_height * zoom_ + height_; y += th)
        {
            ImVec2 p1 = {origin.x, origin.y + y};
            ImVec2 p2 = {origin.x + width_, origin.y + y + tw * 0.5f};
            draw_list->AddLine(p1, p2, grid_col, 1.0f);
        }
        for (float y = 0; y < canvas_height * zoom_ + height_; y += th)
        {
            ImVec2 p1 = {origin.x, origin.y + y + tw * 0.5f};
            ImVec2 p2 = {origin.x + width_, origin.y + y};
            draw_list->AddLine(p1, p2, grid_col, 1.0f);
        }
    }
    else
    {
        for (float x = 0; x < canvas_width * zoom_ + width_; x += tw)
        {
            draw_list->AddLine(
                {origin.x + x, origin.y}, {origin.x + x, origin.y + canvas_height * zoom_ + height_}, grid_col, 1.0f);
        }
        for (float y = 0; y < canvas_height * zoom_ + height_; y += th)
        {
            draw_list->AddLine(
                {origin.x, origin.y + y}, {origin.x + canvas_width * zoom_ + width_, origin.y + y}, grid_col, 1.0f);
        }
    }
}

}  // namespace architect
}  // namespace convoy