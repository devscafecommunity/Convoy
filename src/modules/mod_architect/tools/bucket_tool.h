#pragma once
#include "tool_base.h"
#include <queue>

namespace convoy {
namespace architect {

class BucketTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;

    void set_tolerance(int tolerance) { tolerance_ = tolerance; }

private:
    int tolerance_ = 10;

    bool colors_similar(Color c1, Color c2);
    void flood_fill(Canvas* canvas, int x, int y, Color target, Color fill);
};

} // namespace architect
} // namespace convoy
