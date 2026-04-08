#include "architect_ui.h"
#include "tools/pencil_tool.h"
#include "tools/eraser_tool.h"
#include "tools/bucket_tool.h"
#include <imgui.h>
#include <string>

namespace convoy {
namespace architect {

ArchitectUI::ArchitectUI() : viewport_(800, 600) {
    set_tool(ToolType::Pencil);
}

ArchitectUI::~ArchitectUI() {}

void ArchitectUI::initialize(Canvas* canvas, CommandManager* cmd_mgr) {
    canvas_ = canvas;
    cmd_mgr_ = cmd_mgr;
    viewport_.fit_to_canvas(canvas_);
}

void ArchitectUI::set_tool(ToolType type) {
    current_tool_type_ = type;
    switch (type) {
        case ToolType::Pencil:
            current_tool_ = std::make_unique<PencilTool>();
            break;
        case ToolType::Eraser:
            current_tool_ = std::make_unique<EraserTool>();
            break;
        case ToolType::Bucket:
            current_tool_ = std::make_unique<BucketTool>();
            break;
        default:
            current_tool_ = std::make_unique<PencilTool>();
            break;
    }
    if (current_tool_) {
        current_tool_->set_foreground_color(current_color_);
    }
}

void ArchitectUI::render() {
    render_toolbar();
    render_properties_panel();
    render_layers_panel();
    render_canvas_viewport();
}

void ArchitectUI::render_toolbar() {
    ImGui::Begin("Toolbar");

    float col[4] = { current_color_.r / 255.0f, current_color_.g / 255.0f, current_color_.b / 255.0f, current_color_.a / 255.0f };
    if (ImGui::ColorEdit4("Color", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        current_color_ = Color(static_cast<uint8_t>(col[0] * 255),
                               static_cast<uint8_t>(col[1] * 255),
                               static_cast<uint8_t>(col[2] * 255),
                               static_cast<uint8_t>(col[3] * 255));
        if (current_tool_) {
            current_tool_->set_foreground_color(current_color_);
        }
    }

    ImGui::Separator();

    if (ImGui::Button("Pencil")) set_tool(ToolType::Pencil);
    if (ImGui::Button("Eraser")) set_tool(ToolType::Eraser);
    if (ImGui::Button("Bucket")) set_tool(ToolType::Bucket);

    ImGui::End();
}

void ArchitectUI::render_properties_panel() {
    ImGui::Begin("Properties");

    if (canvas_) {
        ImGui::Text("Canvas Size: %d x %d", canvas_->width(), canvas_->height());
    }

    float zoom = viewport_.get_zoom();
    if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 32.0f)) {
        viewport_.set_zoom(zoom);
    }

    ImGui::End();
}

void ArchitectUI::render_layers_panel() {
    ImGui::Begin("Layers");

    if (canvas_) {
        auto& layers = canvas_->get_layers();
        for (size_t i = 0; i < layers.size(); ++i) {
            auto& layer = layers[i];
            
            bool is_active = (canvas_->get_active_layer() == &layer);
            if (ImGui::Selectable(layer.name.c_str(), is_active)) {
                canvas_->set_active_layer(static_cast<int>(i));
            }
            
            ImGui::SameLine();
            bool visible = layer.visible;
            if (ImGui::Checkbox(("V##" + std::to_string(i)).c_str(), &visible)) {
                layer.visible = visible;
            }
        }
    }

    ImGui::End();
}

void ArchitectUI::render_canvas_viewport() {
    ImGui::Begin("Canvas");

    ImGui::Text("Canvas Rendering Stub");

    handle_canvas_input();

    ImGui::End();
}

void ArchitectUI::handle_canvas_input() {
    if (!canvas_ || !current_tool_) return;

    if (ImGui::IsWindowHovered()) {
        ImVec2 mouse_pos_screen = ImGui::GetMousePos();
        ImVec2 window_pos = ImGui::GetWindowPos();
        
        float local_x = mouse_pos_screen.x - window_pos.x;
        float local_y = mouse_pos_screen.y - window_pos.y;

        Vec2 canvas_coords = viewport_.screen_to_canvas(local_x, local_y);
        int cx = static_cast<int>(canvas_coords.x);
        int cy = static_cast<int>(canvas_coords.y);

        bool mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);

        if (mouse_down && !is_painting_) {
            is_painting_ = true;
            last_mouse_x_ = cx;
            last_mouse_y_ = cy;
            current_tool_->on_mouse_down(canvas_, cx, cy);
        } else if (mouse_down && is_painting_) {
            if (cx != last_mouse_x_ || cy != last_mouse_y_) {
                current_tool_->on_mouse_drag(canvas_, last_mouse_x_, last_mouse_y_, cx, cy);
                last_mouse_x_ = cx;
                last_mouse_y_ = cy;
            }
        } else if (!mouse_down && is_painting_) {
            is_painting_ = false;
            current_tool_->on_mouse_up(canvas_, cx, cy);
        }
    } else if (is_painting_ && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        is_painting_ = false;
        current_tool_->on_mouse_up(canvas_, last_mouse_x_, last_mouse_y_);
    }
}

}
}