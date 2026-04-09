#include "hitbox_tool.h"
#include <algorithm>

namespace convoy { namespace architect {

void HitboxTool::on_mouse_down(Canvas*, int x, int y) {
    start_x_ = x; start_y_ = y;
    hitbox_   = {static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f};
    defining_ = true;
}

void HitboxTool::on_mouse_drag(Canvas*, int, int, int to_x, int to_y) {
    float x1 = static_cast<float>(std::min(start_x_, to_x));
    float y1 = static_cast<float>(std::min(start_y_, to_y));
    float x2 = static_cast<float>(std::max(start_x_, to_x));
    float y2 = static_cast<float>(std::max(start_y_, to_y));
    hitbox_ = {x1, y1, x2 - x1, y2 - y1};
}

void HitboxTool::on_mouse_up(Canvas*, int to_x, int to_y) {
    on_mouse_drag(nullptr, start_x_, start_y_, to_x, to_y);
    defining_ = false;
}

}}
