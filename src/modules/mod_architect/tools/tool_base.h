#pragma once
#include "shared/types.h"
#include "modules/mod_architect/canvas.h"

namespace convoy {
namespace architect {

class Tool {
public:
    virtual ~Tool() = default;

    virtual void on_mouse_down(Canvas* canvas, int x, int y) = 0;
    virtual void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) = 0;
    virtual void on_mouse_up(Canvas* canvas, int x, int y) = 0;

    void set_foreground_color(Color color) { foreground_ = color; }
    void set_background_color(Color color) { background_ = color; }

protected:
    Color foreground_{255, 255, 255, 255};
    Color background_{0, 0, 0, 0};
};

} // namespace architect
} // namespace convoy
