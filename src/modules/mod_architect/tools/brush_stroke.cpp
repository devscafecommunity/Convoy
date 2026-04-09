#include "brush_stroke.h"

#include <cmath>

namespace convoy
{
namespace architect
{

void BrushStroke::add_point(float x, float y, float pressure, float angle)
{
    points_.push_back({x, y, pressure, angle});
}

void BrushStroke::apply(Canvas* canvas, const Brush& brush, Color color)
{
    if (points_.empty() || !canvas)
        return;

    for (size_t i = 0; i < points_.size(); ++i)
    {
        const BrushPoint& point = points_[i];

        float mapped_pressure = map_pressure(point.pressure, brush.curve);

        Color adjusted = color;
        adjusted.a = static_cast<uint8_t>(static_cast<float>(color.a) * mapped_pressure);

        render_stamp(canvas, point, brush, adjusted);
    }
}

void BrushStroke::reset()
{
    points_.clear();
}

float BrushStroke::map_pressure(float raw_pressure, PressureCurve curve)
{
    return Brush::map_pressure(raw_pressure, curve);
}

bool BrushStroke::should_render(const BrushPoint& prev, const BrushPoint& curr, const Brush& brush)
{
    float dx = curr.x - prev.x;
    float dy = curr.y - prev.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    return dist >= (brush.size * brush.spacing / 100.0f);
}

void BrushStroke::render_stamp(Canvas* canvas, const BrushPoint& point, const Brush& brush, Color color)
{
    int half_size = static_cast<int>(brush.size) / 2;
    int cx = static_cast<int>(point.x);
    int cy = static_cast<int>(point.y);

    for (int dy = -half_size; dy <= half_size; ++dy)
    {
        for (int dx = -half_size; dx <= half_size; ++dx)
        {
            int px = cx + dx;
            int py = cy + dy;

            bool in_shape = false;
            if (brush.shape == BrushShape::Circle)
            {
                float dist = std::sqrt(static_cast<float>(dx * dx + dy * dy));
                in_shape = dist <= half_size;
            }
            else if (brush.shape == BrushShape::Square)
            {
                in_shape = true;
            }
            else if (brush.shape == BrushShape::Custom && brush.has_custom_mask)
            {
                int mx = dx + half_size;
                int my = dy + half_size;
                if (mx >= 0 && mx < static_cast<int>(brush.size) && my >= 0 && my < static_cast<int>(brush.size))
                {
                    size_t idx = (my * brush.size + mx) * 4 + 3;
                    if (idx < brush.custom_mask.size())
                    {
                        in_shape = brush.custom_mask[idx] > 128;
                    }
                }
            }

            if (in_shape)
            {
                canvas->set_pixel(px, py, color);
            }
        }
    }
}

}  // namespace architect
}  // namespace convoy
