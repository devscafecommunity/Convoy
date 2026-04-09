#pragma once
#include <vector>

#include "modules/mod_architect/canvas.h"
#include "shared/brush.h"

namespace convoy
{
namespace architect
{

class BrushStroke
{
   public:
    void add_point(float x, float y, float pressure, float angle);
    void apply(Canvas* canvas, const Brush& brush, Color color);
    void reset();
    size_t point_count() const { return points_.size(); }

   private:
    std::vector<BrushPoint> points_;

    float map_pressure(float raw_pressure, PressureCurve curve);
    void render_stamp(Canvas* canvas, const BrushPoint& point, const Brush& brush, Color color);
    bool should_render(const BrushPoint& prev, const BrushPoint& curr, const Brush& brush);
};

}  // namespace architect
}  // namespace convoy
