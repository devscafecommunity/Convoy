#pragma once
#include <functional>
#include <string>

#include "shared/project_mode.h"

namespace convoy
{

class NewProjectDialog
{
   public:
    using CreateCallback = std::function<void(const ProjectModeConfig&)>;

    void set_callback(CreateCallback cb) { on_create_ = cb; }
    void render();
    bool is_open() const { return open_; }
    void open() { open_ = true; }
    void close() { open_ = false; }

   private:
    bool open_ = false;
    CreateCallback on_create_;

    ProjectMode mode_ = ProjectMode::PixelArt;
    uint32_t width_ = 640;
    uint32_t height_ = 360;
    uint32_t tile_w_ = 32;
    uint32_t tile_h_ = 32;
    uint32_t bit_depth_ = 32;
    bool snap_grid_ = true;
    bool snap_angle_ = false;
    float angle_ = 30.0f;
};

}  // namespace convoy
