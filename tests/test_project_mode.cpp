#include <gtest/gtest.h>

#include "shared/project_mode.h"

using namespace convoy;

TEST(ProjectModeTest, ModeEnumValues)
{
    EXPECT_EQ(static_cast<int>(ProjectMode::PixelArt), 0);
    EXPECT_EQ(static_cast<int>(ProjectMode::Drawing), 1);
    EXPECT_EQ(static_cast<int>(ProjectMode::Isometric), 2);
    EXPECT_EQ(static_cast<int>(ProjectMode::TopDown), 3);
}

TEST(ProjectModeTest, GridTypeEnum)
{
    EXPECT_EQ(static_cast<int>(GridType::Square), 0);
    EXPECT_EQ(static_cast<int>(GridType::Diamond), 1);
    EXPECT_EQ(static_cast<int>(GridType::Hidden), 2);
}

TEST(ProjectModeTest, DefaultPixelArtConfig)
{
    auto cfg = ProjectModeConfig::default_pixel_art();
    EXPECT_EQ(cfg.mode, ProjectMode::PixelArt);
    EXPECT_EQ(cfg.canvas_width, 640);
    EXPECT_EQ(cfg.canvas_height, 360);
    EXPECT_EQ(cfg.grid.type, GridType::Square);
    EXPECT_EQ(cfg.grid.tile_width, 8);
    EXPECT_EQ(cfg.grid.tile_height, 8);
    EXPECT_TRUE(cfg.snapping.snap_to_grid);
}

TEST(ProjectModeTest, DefaultIsometricConfig)
{
    auto cfg = ProjectModeConfig::default_isometric();
    EXPECT_EQ(cfg.mode, ProjectMode::Isometric);
    EXPECT_EQ(cfg.grid.type, GridType::Diamond);
    EXPECT_EQ(cfg.grid.tile_width, 32);
    EXPECT_EQ(cfg.grid.tile_height, 16);
    EXPECT_FLOAT_EQ(cfg.grid.angle, 30.0f);
    EXPECT_TRUE(cfg.snapping.snap_to_angle);
    EXPECT_FLOAT_EQ(cfg.snapping.snap_angle, 30.0f);
}

TEST(ProjectModeTest, DefaultTopDownConfig)
{
    auto cfg = ProjectModeConfig::default_top_down();
    EXPECT_EQ(cfg.mode, ProjectMode::TopDown);
    EXPECT_EQ(cfg.grid.type, GridType::Square);
    EXPECT_EQ(cfg.grid.tile_width, 32);
    EXPECT_EQ(cfg.grid.tile_height, 32);
}

TEST(ProjectModeTest, DefaultDrawingConfig)
{
    auto cfg = ProjectModeConfig::default_drawing();
    EXPECT_EQ(cfg.mode, ProjectMode::Drawing);
    EXPECT_EQ(cfg.canvas_width, 1920);
    EXPECT_EQ(cfg.canvas_height, 1080);
    EXPECT_EQ(cfg.grid.type, GridType::Hidden);
    EXPECT_FALSE(cfg.snapping.enabled);
}
