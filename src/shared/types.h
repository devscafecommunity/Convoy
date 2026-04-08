#pragma once
#include "math.h"
#include "color.h"
#include <string>
#include <vector>
#include <cstdint>

namespace convoy {

struct Layer {
    std::string name;
    std::vector<uint32_t> pixels;
    uint32_t width;
    uint32_t height;
    bool visible;
    bool locked;
    float opacity;
    
    Layer(const std::string& name, uint32_t w, uint32_t h)
        : name(name), width(w), height(h), visible(true), locked(false), opacity(1.0f) {
        pixels.resize(w * h, 0);
    }
};

struct Canvas {
    uint32_t width, height;
    std::vector<Layer> layers;
    int active_layer_index;
    
    Canvas(uint32_t w, uint32_t h) : width(w), height(h), active_layer_index(0) {
        layers.emplace_back("Layer 1", w, h);
    }
    
    Layer* get_active_layer() {
        if (active_layer_index >= 0 && active_layer_index < (int)layers.size()) {
            return &layers[active_layer_index];
        }
        return nullptr;
    }
};

struct SpriteMetadata {
    Vec2 pivot;
    Rect hitbox;
    std::string sprite_name;
};

enum class ToolType {
    Pencil,
    Eraser,
    Bucket,
    ColorPicker,
    Move,
};

} // namespace convoy
