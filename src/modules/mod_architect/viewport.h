#pragma once
#include "shared/math.h"

namespace convoy {
namespace architect {

class Canvas;

class Viewport {
public:
    Viewport(float width, float height);
    ~Viewport() = default;

    void set_zoom(float zoom);
    float get_zoom() const;

    void pan(float dx, float dy);
    Vec2 get_offset() const;

    void resize(float width, float height);
    void zoom_to_point(float screen_x, float screen_y, float factor);

    Vec2 screen_to_canvas(float screen_x, float screen_y) const;
    Vec2 canvas_to_screen(float canvas_x, float canvas_y) const;

    void fit_to_canvas(const Canvas* canvas);

private:
    float width_;
    float height_;
    float zoom_;
    Vec2 offset_;
};

}
}
