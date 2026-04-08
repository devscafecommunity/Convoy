#include "eraser_tool.h"
#include <cmath>
#include <algorithm>

namespace convoy {
namespace architect {

void EraserTool::on_mouse_down(Canvas* canvas, int x, int y) {
    canvas->set_pixel(x, y, Color(0, 0, 0, 0));
}

void EraserTool::on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) {
    Color transparent(0, 0, 0, 0);
    int dx = std::abs(to_x - from_x);
    int dy = std::abs(to_y - from_y);
    int steps = std::max(dx, dy) + 1;

    for (int i = 0; i < steps; ++i) {
        float t = steps > 1 ? (float)i / (steps - 1) : 0;
        int x = from_x + (to_x - from_x) * t;
        int y = from_y + (to_y - from_y) * t;
        canvas->set_pixel(x, y, transparent);
    }
}

void EraserTool::on_mouse_up(Canvas* canvas, int x, int y) {
}

} // namespace architect
} // namespace convoy
