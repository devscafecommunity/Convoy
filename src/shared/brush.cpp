#include "shared/brush.h"

#include <algorithm>
#include <cmath>

namespace convoy
{

float Brush::map_pressure(float input, PressureCurve curve)
{
    switch (curve)
    {
        case PressureCurve::Linear:
            return input;
        case PressureCurve::Smooth:
        {
            return input * input * (3.0f - 2.0f * input);
        }
        case PressureCurve::Sharp:
            return input * input;
    }
    return input;
}

std::vector<uint8_t> Brush::generate_parametric_mask(BrushShape shape, int size, uint8_t hardness)
{
    std::vector<uint8_t> mask(size * size * 4, 0);
    int center = size / 2;
    float radius = size / 2.0f;
    float hardness_factor = hardness / 100.0f;

    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            float dx = static_cast<float>(x - center);
            float dy = static_cast<float>(y - center);
            float dist = std::sqrt(dx * dx + dy * dy);
            float alpha = 0.0f;

            switch (shape)
            {
                case BrushShape::Circle:
                {
                    if (dist <= radius)
                    {
                        float edge = radius * (1.0f - hardness_factor);
                        if (dist < edge)
                            alpha = 255.0f;
                        else
                            alpha = 255.0f * (1.0f - (dist - edge) / (radius - edge));
                    }
                    break;
                }
                case BrushShape::Square:
                {
                    float max_dist = std::max(std::abs(dx), std::abs(dy));
                    if (max_dist <= radius)
                    {
                        float edge = radius * (1.0f - hardness_factor);
                        if (max_dist < edge)
                            alpha = 255.0f;
                        else
                            alpha = 255.0f * (1.0f - (max_dist - edge) / (radius - edge));
                    }
                    break;
                }
                case BrushShape::Diamond:
                {
                    float d = (std::abs(dx) + std::abs(dy));
                    if (d <= radius)
                    {
                        float edge = radius * (1.0f - hardness_factor);
                        if (d < edge)
                            alpha = 255.0f;
                        else
                            alpha = 255.0f * (1.0f - (d - edge) / (radius - edge));
                    }
                    break;
                }
                case BrushShape::Star:
                {
                    float angle = std::atan2(dy, dx);
                    float r = radius * (0.5f + 0.5f * std::cos(5.0f * angle));
                    if (dist <= r)
                    {
                        float edge = r * (1.0f - hardness_factor);
                        if (dist < edge)
                            alpha = 255.0f;
                        else
                            alpha = 255.0f * (1.0f - (dist - edge) / (r - edge));
                    }
                    break;
                }
                case BrushShape::Line:
                {
                    if (std::abs(dx) <= radius && std::abs(dy) <= 2.0f)
                        alpha = 255.0f;
                    else if (std::abs(dx) <= radius && std::abs(dy) <= 4.0f)
                        alpha = 128.0f;
                    break;
                }
                case BrushShape::Custom:
                    break;
            }

            mask[(y * size + x) * 4 + 0] = static_cast<uint8_t>(alpha);
            mask[(y * size + x) * 4 + 1] = static_cast<uint8_t>(alpha);
            mask[(y * size + x) * 4 + 2] = static_cast<uint8_t>(alpha);
            mask[(y * size + x) * 4 + 3] = static_cast<uint8_t>(alpha);
        }
    }

    return mask;
}

}  // namespace convoy
