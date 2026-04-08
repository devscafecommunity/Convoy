#pragma once
#include "viewport.h"
#include "canvas.h"
#include "tools/tool_base.h"
#include "../../core/command_manager.h"
#include "../../shared/types.h"
#include <memory>

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

private:
    void render_toolbar();
    void render_canvas_viewport();
    void render_layers_panel();
    void render_properties_panel();
    void handle_canvas_input();

    Canvas* canvas_{nullptr};
    CommandManager* cmd_mgr_{nullptr};
    Viewport viewport_;
    std::unique_ptr<Tool> current_tool_;
    ToolType current_tool_type_{ToolType::Pencil};

    Color current_color_{255, 255, 255, 255};
    bool is_painting_{false};
    int last_mouse_x_{0};
    int last_mouse_y_{0};
};

}
}