#include <gtest/gtest.h>

#include "modules/mod_architect/tools/brush_stroke.h"
#include "shared/brush.h"

using namespace convoy;

TEST(BrushTest, DefaultConstructs)
{
    Brush b;
    EXPECT_EQ(b.size, 8);
    EXPECT_EQ(b.hardness, 100);
    EXPECT_EQ(b.shape, BrushShape::Circle);
    EXPECT_EQ(b.curve, PressureCurve::Linear);
}

TEST(BrushTest, PressureMapping)
{
    // Linear: direct mapping
    EXPECT_FLOAT_EQ(Brush::map_pressure(0.5f, PressureCurve::Linear), 0.5f);
    EXPECT_FLOAT_EQ(Brush::map_pressure(0.0f, PressureCurve::Linear), 0.0f);
    EXPECT_FLOAT_EQ(Brush::map_pressure(1.0f, PressureCurve::Linear), 1.0f);

    // Smooth: ease-in-out
    float smooth = Brush::map_pressure(0.5f, PressureCurve::Smooth);
    EXPECT_GT(smooth, 0.3f);
    EXPECT_LT(smooth, 0.7f);

    // Sharp: power curve
    float sharp = Brush::map_pressure(0.5f, PressureCurve::Sharp);
    EXPECT_LT(sharp, 0.5f);  // Smaller than linear at 0.5
}

TEST(BrushTest, CustomMaskSupport)
{
    Brush b;
    b.shape = BrushShape::Custom;
    std::vector<uint8_t> mask(8 * 8 * 4, 255);
    b.custom_mask = mask;
    b.has_custom_mask = true;

    EXPECT_EQ(b.shape, BrushShape::Custom);
    EXPECT_TRUE(b.has_custom_mask);
    EXPECT_EQ(b.custom_mask.size(), 8 * 8 * 4);
}

TEST(BrushStrokeTest, AddsPoints)
{
    architect::BrushStroke stroke;
    stroke.add_point(10.0f, 20.0f, 0.5f, 0.0f);
    ASSERT_EQ(stroke.point_count(), 1);

    stroke.add_point(15.0f, 25.0f, 0.8f, 0.1f);
    ASSERT_EQ(stroke.point_count(), 2);
}

TEST(BrushStrokeTest, ClearsOnReset)
{
    architect::BrushStroke stroke;
    stroke.add_point(10.0f, 20.0f, 0.5f, 0.0f);
    stroke.reset();
    ASSERT_EQ(stroke.point_count(), 0);
}

TEST(BrushStrokeTest, AppliesToCanvas)
{
    architect::Canvas canvas(32, 32);
    Brush brush;
    brush.size = 4;
    brush.curve = PressureCurve::Linear;

    architect::BrushStroke stroke;
    stroke.add_point(5.0f, 5.0f, 1.0f, 0.0f);
    stroke.apply(&canvas, brush, Color{255, 0, 0, 255});

    // Verify pixel was painted
    Color c = canvas.get_pixel(5, 5);
    EXPECT_EQ(c.r, 255);
}
