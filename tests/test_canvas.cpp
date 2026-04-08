#include <gtest/gtest.h>
#include "modules/mod_architect/canvas.h"
#include "modules/mod_architect/tools/pencil_tool.h"
#include "modules/mod_architect/tools/eraser_tool.h"
#include "modules/mod_architect/tools/bucket_tool.h"

using namespace convoy;
using namespace convoy::architect;

TEST(CanvasTest, SetGetPixel) {
    Canvas canvas(64, 64);
    Color white(255, 255, 255, 255);

    canvas.set_pixel(10, 10, white);
    Color got = canvas.get_pixel(10, 10);

    EXPECT_EQ(got.r, 255);
    EXPECT_EQ(got.g, 255);
    EXPECT_EQ(got.b, 255);
    EXPECT_EQ(got.a, 255);
}

TEST(CanvasTest, LayerManagement) {
    Canvas canvas(64, 64);
    EXPECT_EQ(canvas.get_layers().size(), 1);

    canvas.add_layer("Layer 2");
    EXPECT_EQ(canvas.get_layers().size(), 2);

    canvas.set_active_layer(1);
    EXPECT_EQ(canvas.get_active_layer(), &canvas.get_layers()[1]);
}

TEST(CanvasTest, PencilToolDrawsLine) {
    Canvas canvas(64, 64);
    PencilTool pencil;
    pencil.set_foreground_color(Color(255, 0, 0, 255));

    pencil.on_mouse_drag(&canvas, 0, 0, 10, 0);

    for (int x = 0; x <= 10; ++x) {
        Color pixel = canvas.get_pixel(x, 0);
        EXPECT_EQ(pixel.r, 255);
    }
}

TEST(CanvasTest, EraserToolTransparency) {
    Canvas canvas(64, 64);
    Color white(255, 255, 255, 255);
    
    canvas.set_pixel(5, 5, white);
    EXPECT_EQ(canvas.get_pixel(5, 5).a, 255);

    EraserTool eraser;
    eraser.on_mouse_down(&canvas, 5, 5);

    Color result = canvas.get_pixel(5, 5);
    EXPECT_EQ(result.a, 0);
}

TEST(CanvasTest, BucketToolFloodFill) {
    Canvas canvas(32, 32);
    
    Color red(255, 0, 0, 255);
    Color blue(0, 0, 255, 255);
    
    canvas.set_pixel(10, 10, red);
    canvas.set_pixel(11, 10, red);
    canvas.set_pixel(12, 10, red);

    BucketTool bucket;
    bucket.set_foreground_color(blue);
    bucket.on_mouse_down(&canvas, 11, 10);

    Color result = canvas.get_pixel(11, 10);
    EXPECT_EQ(result.b, 255);
}
