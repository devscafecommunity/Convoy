#include "shared/project_mode.h"

namespace convoy
{

ProjectModeConfig ProjectModeConfig::default_pixel_art()
{
    ProjectModeConfig cfg;
    cfg.mode = ProjectMode::PixelArt;
    cfg.canvas_width = 640;
    cfg.canvas_height = 360;
    cfg.grid.type = GridType::Square;
    cfg.grid.tile_width = 8;
    cfg.grid.tile_height = 8;
    cfg.snapping.snap_to_grid = true;
    return cfg;
}

ProjectModeConfig ProjectModeConfig::default_isometric()
{
    ProjectModeConfig cfg;
    cfg.mode = ProjectMode::Isometric;
    cfg.canvas_width = 640;
    cfg.canvas_height = 360;
    cfg.grid.type = GridType::Diamond;
    cfg.grid.tile_width = 32;
    cfg.grid.tile_height = 16;
    cfg.grid.angle = 30.0f;
    cfg.snapping.snap_to_grid = true;
    cfg.snapping.snap_to_angle = true;
    cfg.snapping.snap_angle = 30.0f;
    return cfg;
}

ProjectModeConfig ProjectModeConfig::default_top_down()
{
    ProjectModeConfig cfg;
    cfg.mode = ProjectMode::TopDown;
    cfg.canvas_width = 640;
    cfg.canvas_height = 360;
    cfg.grid.type = GridType::Square;
    cfg.grid.tile_width = 32;
    cfg.grid.tile_height = 32;
    cfg.snapping.snap_to_grid = true;
    return cfg;
}

ProjectModeConfig ProjectModeConfig::default_drawing()
{
    ProjectModeConfig cfg;
    cfg.mode = ProjectMode::Drawing;
    cfg.canvas_width = 1920;
    cfg.canvas_height = 1080;
    cfg.grid.type = GridType::Hidden;
    cfg.snapping.enabled = false;
    return cfg;
}

}  // namespace convoy
