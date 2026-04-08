#include "canvas.h"
#include <algorithm>
#include <stdexcept>

namespace convoy {
namespace architect {

Canvas::Canvas(uint32_t width, uint32_t height) 
    : width_(width), height_(height), active_layer_(0) {
    layers_.emplace_back("Background", width, height);
}

bool Canvas::is_in_bounds(uint32_t x, uint32_t y) const {
    return x < width_ && y < height_;
}

void Canvas::set_pixel(uint32_t x, uint32_t y, Color color) {
    if (!is_in_bounds(x, y)) return;
    
    Layer* layer = get_active_layer();
    if (layer) {
        layer->pixels[y * width_ + x] = color.to_rgba();
    }
}

Color Canvas::get_pixel(uint32_t x, uint32_t y) const {
    if (!is_in_bounds(x, y)) return Color(0, 0, 0, 0);
    
    const Layer* layer = get_active_layer();
    if (layer) {
        return Color::from_rgba(layer->pixels[y * width_ + x]);
    }
    return Color(0, 0, 0, 0);
}

void Canvas::add_layer(const std::string& name) {
    if (layers_.size() >= 32) {
        throw std::runtime_error("Maximum 32 layers allowed");
    }
    layers_.emplace_back(name, width_, height_);
}

void Canvas::remove_layer(int index) {
    if (index < 0 || index >= (int)layers_.size() || layers_.size() == 1) {
        return;
    }
    layers_.erase(layers_.begin() + index);
    if (active_layer_ >= (int)layers_.size()) {
        active_layer_ = layers_.size() - 1;
    }
}

void Canvas::set_active_layer(int index) {
    if (index >= 0 && index < (int)layers_.size()) {
        active_layer_ = index;
    }
}

Layer* Canvas::get_active_layer() {
    if (active_layer_ >= 0 && active_layer_ < (int)layers_.size()) {
        return &layers_[active_layer_];
    }
    return nullptr;
}

const Layer* Canvas::get_active_layer() const {
    if (active_layer_ >= 0 && active_layer_ < (int)layers_.size()) {
        return &layers_[active_layer_];
    }
    return nullptr;
}

void Canvas::clear_active_layer() {
    Layer* layer = get_active_layer();
    if (layer) {
        std::fill(layer->pixels.begin(), layer->pixels.end(), 0);
    }
}

void Canvas::composite_to_texture(std::vector<uint32_t>& output) {
    if (output.size() != width_ * height_) {
        output.resize(width_ * height_);
    }
    
    std::fill(output.begin(), output.end(), 0xFF1F1F1F);
    
    for (const auto& layer : layers_) {
        if (!layer.visible) continue;
        
        float opacity = layer.opacity;
        for (uint32_t i = 0; i < layer.pixels.size(); ++i) {
            uint32_t src = layer.pixels[i];
            uint8_t src_a = (src >> 24) & 0xFF;
            uint8_t src_r = (src >> 16) & 0xFF;
            uint8_t src_g = (src >> 8) & 0xFF;
            uint8_t src_b = src & 0xFF;
            
            src_a = static_cast<uint8_t>(src_a * opacity);
            
            if (src_a > 0) {
                uint32_t dst = output[i];
                uint8_t dst_a = (dst >> 24) & 0xFF;
                uint8_t dst_r = (dst >> 16) & 0xFF;
                uint8_t dst_g = (dst >> 8) & 0xFF;
                uint8_t dst_b = dst & 0xFF;
                
                float alpha = src_a / 255.0f;
                dst_r = static_cast<uint8_t>(dst_r * (1 - alpha) + src_r * alpha);
                dst_g = static_cast<uint8_t>(dst_g * (1 - alpha) + src_g * alpha);
                dst_b = static_cast<uint8_t>(dst_b * (1 - alpha) + src_b * alpha);
                dst_a = static_cast<uint8_t>(std::max(dst_a, src_a));
                
                output[i] = (dst_a << 24) | (dst_r << 16) | (dst_g << 8) | dst_b;
            }
        }
    }
}

} // namespace architect
} // namespace convoy
