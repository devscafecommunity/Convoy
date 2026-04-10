#pragma once
#include <functional>
#include <vector>

namespace convoy
{

struct SplinePoint
{
    float x;
    float y;
};

class PressureCurveEditor
{
   public:
    void render();
    bool is_open() const { return is_open_; }
    void open() { is_open_ = true; }
    void close() { is_open_ = false; }

    std::vector<SplinePoint> get_curve() const { return curve_points_; }
    void set_curve(const std::vector<SplinePoint>& curve) { curve_points_ = curve; }

    std::function<void(const std::vector<SplinePoint>&)> on_curve_changed;

   private:
    bool is_open_ = false;
    std::vector<SplinePoint> curve_points_ = {{0.0f, 0.0f}, {0.25f, 0.25f}, {0.5f, 0.5f}, {0.75f, 0.75f}, {1.0f, 1.0f}};
    int selected_point_ = -1;
};

}  // namespace convoy