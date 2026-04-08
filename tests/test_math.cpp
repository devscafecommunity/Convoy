#include <gtest/gtest.h>
#include "shared/math.h"
#include "shared/color.h"

using namespace convoy;

// Vec2 Tests
TEST(MathTest, Vec2DefaultConstruction) {
    Vec2 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST(MathTest, Vec2ParameterizedConstruction) {
    Vec2 v(3.5f, 4.2f);
    EXPECT_FLOAT_EQ(v.x, 3.5f);
    EXPECT_FLOAT_EQ(v.y, 4.2f);
}

TEST(MathTest, Vec2Addition) {
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    Vec2 c = a + b;
    EXPECT_FLOAT_EQ(c.x, 4.0f);
    EXPECT_FLOAT_EQ(c.y, 6.0f);
}

TEST(MathTest, Vec2Subtraction) {
    Vec2 a(5.0f, 7.0f);
    Vec2 b(2.0f, 3.0f);
    Vec2 c = a - b;
    EXPECT_FLOAT_EQ(c.x, 3.0f);
    EXPECT_FLOAT_EQ(c.y, 4.0f);
}

TEST(MathTest, Vec2ScalarMultiplication) {
    Vec2 v(3.0f, 4.0f);
    Vec2 scaled = v * 2.0f;
    EXPECT_FLOAT_EQ(scaled.x, 6.0f);
    EXPECT_FLOAT_EQ(scaled.y, 8.0f);
}

TEST(MathTest, Vec2Length) {
    Vec2 v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.length(), 5.0f);
}

TEST(MathTest, Vec2Normalization) {
    Vec2 v(3.0f, 4.0f);
    Vec2 normalized = v.normalized();
    EXPECT_FLOAT_EQ(normalized.x, 0.6f);
    EXPECT_FLOAT_EQ(normalized.y, 0.8f);
}

TEST(MathTest, Vec2NormalizationZeroVector) {
    Vec2 v(0.0f, 0.0f);
    Vec2 normalized = v.normalized();
    EXPECT_FLOAT_EQ(normalized.x, 0.0f);
    EXPECT_FLOAT_EQ(normalized.y, 0.0f);
}

// Rect Tests
TEST(MathTest, RectDefaultConstruction) {
    Rect r;
    EXPECT_FLOAT_EQ(r.x, 0.0f);
    EXPECT_FLOAT_EQ(r.y, 0.0f);
    EXPECT_FLOAT_EQ(r.w, 0.0f);
    EXPECT_FLOAT_EQ(r.h, 0.0f);
}

TEST(MathTest, RectParameterizedConstruction) {
    Rect r(5.0f, 10.0f, 20.0f, 30.0f);
    EXPECT_FLOAT_EQ(r.x, 5.0f);
    EXPECT_FLOAT_EQ(r.y, 10.0f);
    EXPECT_FLOAT_EQ(r.w, 20.0f);
    EXPECT_FLOAT_EQ(r.h, 30.0f);
}

TEST(MathTest, RectContainsInside) {
    Rect r(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_TRUE(r.contains(Vec2(5.0f, 5.0f)));
}

TEST(MathTest, RectContainsAtOrigin) {
    Rect r(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_TRUE(r.contains(Vec2(0.0f, 0.0f)));
}

TEST(MathTest, RectContainsAtEdge) {
    Rect r(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_FALSE(r.contains(Vec2(10.0f, 10.0f)));
}

TEST(MathTest, RectContainsNegativeX) {
    Rect r(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_FALSE(r.contains(Vec2(-1.0f, 5.0f)));
}

TEST(MathTest, RectContainsNegativeY) {
    Rect r(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_FALSE(r.contains(Vec2(5.0f, -1.0f)));
}

TEST(MathTest, RectIntersectOverlapping) {
    Rect r1(0.0f, 0.0f, 10.0f, 10.0f);
    Rect r2(5.0f, 5.0f, 10.0f, 10.0f);
    Rect intersection = r1.intersect(r2);
    EXPECT_FLOAT_EQ(intersection.x, 5.0f);
    EXPECT_FLOAT_EQ(intersection.y, 5.0f);
    EXPECT_FLOAT_EQ(intersection.w, 5.0f);
    EXPECT_FLOAT_EQ(intersection.h, 5.0f);
}

TEST(MathTest, RectIntersectNoOverlap) {
    Rect r1(0.0f, 0.0f, 5.0f, 5.0f);
    Rect r2(10.0f, 10.0f, 5.0f, 5.0f);
    Rect intersection = r1.intersect(r2);
    EXPECT_FLOAT_EQ(intersection.w, 0.0f);
    EXPECT_FLOAT_EQ(intersection.h, 0.0f);
}

TEST(MathTest, RectIntersectAdjacent) {
    Rect r1(0.0f, 0.0f, 10.0f, 10.0f);
    Rect r2(10.0f, 0.0f, 10.0f, 10.0f);
    Rect intersection = r1.intersect(r2);
    EXPECT_FLOAT_EQ(intersection.w, 0.0f);
    EXPECT_FLOAT_EQ(intersection.h, 0.0f);
}

// Color Tests
TEST(ColorTest, ColorDefaultConstruction) {
    Color c;
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
    EXPECT_EQ(c.a, 255);
}

TEST(ColorTest, ColorParameterizedConstruction) {
    Color c(255, 128, 64, 200);
    EXPECT_EQ(c.r, 255);
    EXPECT_EQ(c.g, 128);
    EXPECT_EQ(c.b, 64);
    EXPECT_EQ(c.a, 200);
}

TEST(ColorTest, ColorDefaultAlpha) {
    Color c(100, 150, 200);
    EXPECT_EQ(c.r, 100);
    EXPECT_EQ(c.g, 150);
    EXPECT_EQ(c.b, 200);
    EXPECT_EQ(c.a, 255);
}

TEST(ColorTest, ToRGBA) {
    Color c(255, 128, 64, 200);
    uint32_t rgba = c.to_rgba();
    EXPECT_EQ((rgba >> 24) & 0xFF, 255);  // R
    EXPECT_EQ((rgba >> 16) & 0xFF, 128);  // G
    EXPECT_EQ((rgba >> 8) & 0xFF, 64);    // B
    EXPECT_EQ(rgba & 0xFF, 200);          // A
}

TEST(ColorTest, FromRGBA) {
    uint32_t rgba = (255 << 24) | (128 << 16) | (64 << 8) | 200;
    Color c = Color::from_rgba(rgba);
    EXPECT_EQ(c.r, 255);
    EXPECT_EQ(c.g, 128);
    EXPECT_EQ(c.b, 64);
    EXPECT_EQ(c.a, 200);
}

TEST(ColorTest, RGBARoundtrip) {
    Color original(255, 128, 64, 200);
    uint32_t rgba = original.to_rgba();
    Color restored = Color::from_rgba(rgba);
    EXPECT_EQ(restored.r, original.r);
    EXPECT_EQ(restored.g, original.g);
    EXPECT_EQ(restored.b, original.b);
    EXPECT_EQ(restored.a, original.a);
}

TEST(ColorTest, ToABGR) {
    Color c(255, 128, 64, 200);
    uint32_t abgr = c.to_abgr();
    EXPECT_EQ((abgr >> 24) & 0xFF, 200);  // A
    EXPECT_EQ((abgr >> 16) & 0xFF, 64);   // B
    EXPECT_EQ((abgr >> 8) & 0xFF, 128);   // G
    EXPECT_EQ(abgr & 0xFF, 255);          // R
}
