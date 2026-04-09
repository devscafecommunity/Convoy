#pragma once
#include <cmath>
#include <algorithm>

namespace convoy {

struct Vec2 {
    float x, y;
    
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    
    float length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        float len = length();
        return len > 0.0f ? *this * (1.0f / len) : Vec2(0, 0);
    }
};

struct Rect {
    float x, y, w, h;
    
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    
    bool contains(const Vec2& point) const {
        return point.x >= x && point.x < x + w && point.y >= y && point.y < y + h;
    }
    
    Rect intersect(const Rect& other) const {
        float x1 = std::max(x, other.x);
        float y1 = std::max(y, other.y);
        float x2 = std::min(x + w, other.x + other.w);
        float y2 = std::min(y + h, other.y + other.h);
        float rw = x2 - x1;
        float rh = y2 - y1;
        if (rw <= 0.0f || rh <= 0.0f) return Rect(0.0f, 0.0f, 0.0f, 0.0f);
        return Rect(x1, y1, rw, rh);
    }
};

} // namespace convoy
