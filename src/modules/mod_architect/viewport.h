#pragma once
#include <imgui.h>

#include "shared/math.h"
#include "shared/project_mode.h"

namespace convoy
{
namespace architect
{

class Canvas;

class Viewport
{
   public:
    Viewport(float width, float height);
    ~Viewport() = default;

    void set_zoom(float zoom);
    float get_zoom() const;

    void pan(float dx, float dy);
    Vec2 get_offset() const;

    void resize(float width, float height);
    void zoom_to_point(float screen_x, float screen_y, float factor);

    Vec2 screen_to_canvas(float screen_x, float screen_y) const;
    Vec2 canvas_to_screen(float canvas_x, float canvas_y) const;

    void set_mode(ProjectMode mode);
    void set_grid_settings(const GridSettings& grid);
    void set_snap_settings(const SnapSettings& snap);

    Vec2 snap_to_grid(const Vec2& pos) const;
    Vec2 snap_to_angle(const Vec2& pos) const;

    void render_grid(ImDrawList* draw_list, ImVec2 origin, float canvas_width, float canvas_height);

    void fit_to_canvas(const Canvas* canvas);

   private:
    float width_;
    float height_;
    float zoom_;
    Vec2 offset_;
    ProjectMode mode_ = ProjectMode::PixelArt;
    GridSettings grid_;
    SnapSettings snap_;
};

}  // namespace architect
}  // namespace convoy
