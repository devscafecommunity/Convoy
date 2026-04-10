#pragma once
#include <cstdint>
#include <string>

namespace convoy
{

enum class ProjectMode : uint8_t
{
    PixelArt = 0,
    Drawing = 1,
    Isometric = 2,
    TopDown = 3
};

enum class GridType : uint8_t
{
    Square = 0,
    Diamond = 1,
    Hidden = 2
};

enum class SnapBehavior : uint8_t
{
    None = 0,
    Pixel = 1,
    Tile = 2,
    Angle = 4
};

struct GridSettings
{
    GridType type = GridType::Square;
    uint32_t tile_width = 32;
    uint32_t tile_height = 32;
    float angle = 0.0f;
};

struct SnapSettings
{
    bool enabled = true;
    bool snap_to_grid = true;
    bool snap_to_angle = false;
    float snap_angle = 0.0f;
};

class ProjectModeConfig
{
   public:
    ProjectMode mode = ProjectMode::PixelArt;
    uint32_t canvas_width = 640;
    uint32_t canvas_height = 360;
    uint32_t bit_depth = 32;
    GridSettings grid;
    SnapSettings snapping;

    static ProjectModeConfig default_pixel_art();
    static ProjectModeConfig default_isometric();
    static ProjectModeConfig default_top_down();
    static ProjectModeConfig default_drawing();
};

}  // namespace convoy
