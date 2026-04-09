#pragma once
#include "tool_base.h"
#include "shared/math.h"

namespace convoy { namespace architect {

class PivotTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
    Vec2 get_pivot() const { return pivot_; }

private:
    Vec2 pivot_{0.0f, 0.0f};
};

}}
