#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace convoy
{

enum class BrushShape
{
    Circle,
    Square,
    Custom
};

enum class PressureCurve
{
    Linear,
    Smooth,
    Sharp
};

struct BrushPoint
{
    float x, y;
    float pressure;
    float angle;
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

    static float map_pressure(float input, PressureCurve curve);
};

}  // namespace convoy
