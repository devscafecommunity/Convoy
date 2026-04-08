#pragma once
#include <cstdint>

namespace convoy {

struct Color {
    uint8_t r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
    
    uint32_t to_rgba() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }
    
    uint32_t to_abgr() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }
    
    static Color from_rgba(uint32_t rgba) {
        return Color(
            (rgba >> 24) & 0xFF,
            (rgba >> 16) & 0xFF,
            (rgba >> 8) & 0xFF,
            rgba & 0xFF
        );
    }
};

} // namespace convoy
