#include "viewport.h"
#include "canvas.h"
#include <algorithm>

namespace convoy {
namespace architect {

Viewport::Viewport(float width, float height)
    : width_(width), height_(height), zoom_(1.0f), offset_(0.0f, 0.0f) {
}

void Viewport::set_zoom(float zoom) {
    zoom_ = std::min(32.0f, std::max(0.1f, zoom));
}

float Viewport::get_zoom() const {
    return zoom_;
}

void Viewport::pan(float dx, float dy) {
    offset_.x += dx;
    offset_.y += dy;
}

Vec2 Viewport::get_offset() const {
    return offset_;
}

Vec2 Viewport::screen_to_canvas(float screen_x, float screen_y) const {
    float canvas_x = (screen_x - offset_.x) / zoom_;
    float canvas_y = (screen_y - offset_.y) / zoom_;
    return Vec2(canvas_x, canvas_y);
}

Vec2 Viewport::canvas_to_screen(float canvas_x, float canvas_y) const {
    float screen_x = (canvas_x * zoom_) + offset_.x;
    float screen_y = (canvas_y * zoom_) + offset_.y;
    return Vec2(screen_x, screen_y);
}

void Viewport::resize(float width, float height) {
    width_  = width;
    height_ = height;
}

void Viewport::zoom_to_point(float screen_x, float screen_y, float factor) {
    Vec2 pre = screen_to_canvas(screen_x, screen_y);
    set_zoom(zoom_ * factor);
    offset_.x = screen_x - pre.x * zoom_;
    offset_.y = screen_y - pre.y * zoom_;
}

void Viewport::fit_to_canvas(const Canvas* canvas) {
    if (!canvas) return;

    float canvas_w = static_cast<float>(canvas->width());
    float canvas_h = static_cast<float>(canvas->height());

    float scale_x = width_ / (canvas_w * 1.05f);
    float scale_y = height_ / (canvas_h * 1.05f);
    
    set_zoom(std::min(scale_x, scale_y));

    float center_x = (width_ - canvas_w * zoom_) * 0.5f;
    float center_y = (height_ - canvas_h * zoom_) * 0.5f;
    
    offset_ = Vec2(center_x, center_y);
}

}
}