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
    Pivot,
    Hitbox,
};

} // namespace convoy

