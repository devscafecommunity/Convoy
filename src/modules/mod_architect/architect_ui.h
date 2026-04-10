#pragma once
#include "viewport.h"
#include "canvas.h"
#include "tools/tool_base.h"
#include "core/command_manager.h"
#include "core/gfx/render_texture.h"
#include "shared/types.h"
#include "ui/dod_visualizer.h"
#include <imgui.h>
#include <memory>
#include <cstdint>

struct GLFWwindow;

namespace convoy {
namespace architect {

class ArchitectUI {
public:
    ArchitectUI();
    ~ArchitectUI();

    void initialize(Canvas* canvas, CommandManager* cmd_mgr);
    void render();
    void set_tool(ToolType type);
    Tool* get_current_tool() const { return current_tool_.get(); }

    void toggle_grid()              { show_grid_      = !show_grid_; }
    void toggle_collision_overlay() { show_collision_ = !show_collision_; }
    void toggle_dod_visualizer()    { show_dod_       = !show_dod_; }

private:
    void render_toolbar();
    void render_canvas_viewport();
    void render_layers_panel();
    void render_properties_panel();
    void handle_canvas_input(ImVec2 canvas_origin, ImVec2 canvas_size);

    void upload_canvas_texture();
    void ensure_canvas_texture();

    Canvas*         canvas_{nullptr};
    CommandManager* cmd_mgr_{nullptr};
    Viewport        viewport_;
    std::unique_ptr<Tool> current_tool_;
    ToolType        current_tool_type_{ToolType::Pencil};
    Color           current_color_{255, 255, 255, 255};

    // GPU canvas texture (updated on pixel change)
    uint32_t canvas_tex_id_  = 0;
    bool     canvas_dirty_   = true;

    bool is_painting_    = false;
    bool is_panning_     = false;
    int  last_mouse_x_   = 0;
    int  last_mouse_y_   = 0;
    float pan_start_x_   = 0.0f;
    float pan_start_y_   = 0.0f;

    bool show_grid_ = true;
    bool show_collision_ = false;
    bool show_dod_       = false;
    DODVisualizer dod_visualizer_;

    Vec2 pivot_point_{0.0f, 0.0f};
    Rect hitbox_{0.0f, 0.0f, 0.0f, 0.0f};
    bool hitbox_defined_ = false;
};

}
}
