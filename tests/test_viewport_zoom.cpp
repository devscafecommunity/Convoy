#include <gtest/gtest.h>
#include "modules/mod_architect/viewport.h"

using namespace convoy;
using namespace convoy::architect;

TEST(ViewportTest, ZoomToPoint_PreservesCanvasPoint) {
    Viewport vp(800, 600);
    vp.set_zoom(1.0f);

    // Canvas point at screen (100, 100) before zoom
    Vec2 before = vp.screen_to_canvas(100.0f, 100.0f);
    vp.zoom_to_point(100.0f, 100.0f, 2.0f);
    // After zoom, same screen point should map to same canvas point
    Vec2 after = vp.screen_to_canvas(100.0f, 100.0f);

    EXPECT_NEAR(before.x, after.x, 0.01f);
    EXPECT_NEAR(before.y, after.y, 0.01f);
    EXPECT_NEAR(vp.get_zoom(), 2.0f, 0.001f);
}

TEST(ViewportTest, ZoomClampsToLimits) {
    Viewport vp(800, 600);
    vp.set_zoom(1.0f);
    vp.zoom_to_point(400, 300, 1000.0f);
    EXPECT_EQ(vp.get_zoom(), 32.0f);
}
