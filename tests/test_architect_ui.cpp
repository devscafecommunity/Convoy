#include <gtest/gtest.h>
#include "modules/mod_architect/viewport.h"
#include "modules/mod_architect/architect_ui.h"
#include "modules/mod_architect/canvas.h"
#include "core/command_manager.h"

TEST(ViewportTest, Initialization) {
    convoy::architect::Viewport vp(800.0f, 600.0f);
    EXPECT_FLOAT_EQ(vp.get_zoom(), 1.0f);
    EXPECT_FLOAT_EQ(vp.get_offset().x, 0.0f);
    EXPECT_FLOAT_EQ(vp.get_offset().y, 0.0f);
}

TEST(ViewportTest, ZoomClamping) {
    convoy::architect::Viewport vp(800.0f, 600.0f);
    
    vp.set_zoom(0.05f);
    EXPECT_FLOAT_EQ(vp.get_zoom(), 0.1f);
    
    vp.set_zoom(50.0f);
    EXPECT_FLOAT_EQ(vp.get_zoom(), 32.0f);
    
    vp.set_zoom(2.0f);
    EXPECT_FLOAT_EQ(vp.get_zoom(), 2.0f);
}

TEST(ViewportTest, Panning) {
    convoy::architect::Viewport vp(800.0f, 600.0f);
    vp.pan(100.0f, -50.0f);
    
    EXPECT_FLOAT_EQ(vp.get_offset().x, 100.0f);
    EXPECT_FLOAT_EQ(vp.get_offset().y, -50.0f);
}

TEST(ViewportTest, CoordinateConversion) {
    convoy::architect::Viewport vp(800.0f, 600.0f);
    
    auto screen = vp.canvas_to_screen(10.0f, 20.0f);
    EXPECT_FLOAT_EQ(screen.x, 10.0f);
    EXPECT_FLOAT_EQ(screen.y, 20.0f);
    
    auto canvas = vp.screen_to_canvas(10.0f, 20.0f);
    EXPECT_FLOAT_EQ(canvas.x, 10.0f);
    EXPECT_FLOAT_EQ(canvas.y, 20.0f);
    
    vp.set_zoom(2.0f);
    vp.pan(50.0f, 100.0f);
    
    screen = vp.canvas_to_screen(10.0f, 20.0f);
    EXPECT_FLOAT_EQ(screen.x, 70.0f);
    EXPECT_FLOAT_EQ(screen.y, 140.0f);
    
    canvas = vp.screen_to_canvas(70.0f, 140.0f);
    EXPECT_FLOAT_EQ(canvas.x, 10.0f);
    EXPECT_FLOAT_EQ(canvas.y, 20.0f);
}

TEST(ToolSelectionTest, SetTool) {
    convoy::architect::Canvas canvas(64, 64);
    convoy::CommandManager cmd_mgr;
    convoy::architect::ArchitectUI ui;
    
    ui.initialize(&canvas, &cmd_mgr);
    ui.set_tool(convoy::ToolType::Pencil);
    ui.set_tool(convoy::ToolType::Eraser);
    ui.set_tool(convoy::ToolType::Bucket);
    
    SUCCEED();
}
