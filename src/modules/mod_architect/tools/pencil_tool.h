#pragma once
#include "tool_base.h"

namespace convoy {
namespace architect {

class PencilTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
};

} // namespace architect
} // namespace convoy
