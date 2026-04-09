#include <gtest/gtest.h>
#include "modules/mod_architect/tools/pivot_tool.h"
#include "modules/mod_architect/tools/hitbox_tool.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;
using namespace convoy::architect;

TEST(PivotToolTest, SetsPositionOnClick) {
    PivotTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 10, 20);
    EXPECT_FLOAT_EQ(tool.get_pivot().x, 10.0f);
    EXPECT_FLOAT_EQ(tool.get_pivot().y, 20.0f);
}

TEST(PivotToolTest, UpdatesOnDrag) {
    PivotTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 0, 0);
    tool.on_mouse_drag(&c, 0, 0, 15, 25);
    EXPECT_FLOAT_EQ(tool.get_pivot().x, 15.0f);
    EXPECT_FLOAT_EQ(tool.get_pivot().y, 25.0f);
}

TEST(HitboxToolTest, BuildsCorrectRect) {
    HitboxTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 5, 5);
    tool.on_mouse_drag(&c, 5, 5, 20, 30);
    tool.on_mouse_up(&c, 20, 30);
    const Rect& hb = tool.get_hitbox();
    EXPECT_FLOAT_EQ(hb.x, 5.0f);
    EXPECT_FLOAT_EQ(hb.y, 5.0f);
    EXPECT_FLOAT_EQ(hb.w, 15.0f);
    EXPECT_FLOAT_EQ(hb.h, 25.0f);
    EXPECT_FALSE(tool.is_defining());
}

TEST(HitboxToolTest, HandlesReverseDirection) {
    HitboxTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 20, 30);
    tool.on_mouse_up(&c, 5, 5);
    const Rect& hb = tool.get_hitbox();
    EXPECT_FLOAT_EQ(hb.x, 5.0f);
    EXPECT_FLOAT_EQ(hb.y, 5.0f);
    EXPECT_FLOAT_EQ(hb.w, 15.0f);
    EXPECT_FLOAT_EQ(hb.h, 25.0f);
}
