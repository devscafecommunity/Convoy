#pragma once
#include "tool_base.h"
#include "shared/math.h"

namespace convoy { namespace architect {

class HitboxTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
    const Rect& get_hitbox() const { return hitbox_; }
    bool is_defining() const { return defining_; }

private:
    Rect hitbox_{0, 0, 0, 0};
    int  start_x_ = 0, start_y_ = 0;
    bool defining_ = false;
};

}}
