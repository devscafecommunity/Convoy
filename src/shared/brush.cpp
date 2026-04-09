#include "shared/brush.h"

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

}  // namespace convoy
