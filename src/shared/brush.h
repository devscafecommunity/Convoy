#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace convoy
{

enum class BrushShape
{
    Circle,
    Square,
    Diamond,
    Star,
    Line,
    Custom
};

enum class PressureCurve
{
    Linear,
    Smooth,
    Sharp
};

enum class DynamicMaskMode
{
    None,
    PressureExpandsMask,
    PressureIncreasesOpacity
};

enum class MaskSource
{
    Parametric,
    Bitmask,
    Dynamic
};

struct BrushPoint
{
    float x, y;
    float pressure;
    float angle;
};

struct DynamicMaskConfig
{
    DynamicMaskMode mode = DynamicMaskMode::None;
    float rotation = 0.0f;
    bool follow_mouse = false;
    std::function<std::vector<uint8_t>(float pressure, float angle, int size)> generator;
};

class Brush
{
   public:
    std::string name = "Default";
    BrushShape shape = BrushShape::Circle;
    uint8_t size = 8;
    uint8_t hardness = 100;
    uint8_t spacing = 20;
    PressureCurve curve = PressureCurve::Linear;

    std::vector<uint8_t> custom_mask;
    bool has_custom_mask = false;

    DynamicMaskConfig dynamic_mask;
    bool use_dynamic_mask = false;

    static float map_pressure(float input, PressureCurve curve);
    static std::vector<uint8_t> generate_parametric_mask(BrushShape shape, int size, uint8_t hardness);
};

}  // namespace convoy
