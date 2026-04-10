#include "architect_ui.h"
#include "tools/pencil_tool.h"
#include "tools/eraser_tool.h"
#include "tools/bucket_tool.h"
#include "tools/pivot_tool.h"
#include "tools/hitbox_tool.h"
#include <imgui.h>
#include <glad/glad.h>
#include <string>
#include <vector>

namespace convoy {
namespace architect {

ArchitectUI::ArchitectUI() : viewport_(800, 600) {
    set_tool(ToolType::Pencil);
}

ArchitectUI::~ArchitectUI() {
    if (canvas_tex_id_) glDeleteTextures(1, &canvas_tex_id_);
}

void ArchitectUI::initialize(Canvas* canvas, CommandManager* cmd_mgr) {
    canvas_   = canvas;
    cmd_mgr_  = cmd_mgr;
    viewport_.fit_to_canvas(canvas_);
    canvas_dirty_ = true;
}

void ArchitectUI::set_tool(ToolType type) {
    current_tool_type_ = type;
    switch (type) {
        case ToolType::Pencil:  current_tool_ = std::make_unique<PencilTool>();  break;
        case ToolType::Eraser:  current_tool_ = std::make_unique<EraserTool>();  break;
        case ToolType::Bucket:  current_tool_ = std::make_unique<BucketTool>();  break;
        case ToolType::Pivot:   current_tool_ = std::make_unique<PivotTool>();   break;
        case ToolType::Hitbox:  current_tool_ = std::make_unique<HitboxTool>();  break;
        default:                current_tool_ = std::make_unique<PencilTool>();  break;
    }
    if (current_tool_) current_tool_->set_foreground_color(current_color_);
}

void ArchitectUI::ensure_canvas_texture() {
    if (canvas_tex_id_ != 0) return;
    glGenTextures(1, &canvas_tex_id_);
    glBindTexture(GL_TEXTURE_2D, canvas_tex_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ArchitectUI::upload_canvas_texture() {
    if (!canvas_ || !canvas_dirty_) return;
    ensure_canvas_texture();

    std::vector<uint32_t> pixels;
    canvas_->composite_to_texture(pixels);

    glBindTexture(GL_TEXTURE_2D, canvas_tex_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<int>(canvas_->width()),
                 static_cast<int>(canvas_->height()),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    canvas_dirty_ = false;
}

void ArchitectUI::render() {
    upload_canvas_texture();
    render_toolbar();
    render_canvas_viewport();
    render_layers_panel();
    render_properties_panel();
}

void ArchitectUI::render_toolbar() {
    ImGui::Begin("Toolbar");

    float col[4] = {
        current_color_.r / 255.0f, current_color_.g / 255.0f,
        current_color_.b / 255.0f, current_color_.a / 255.0f
    };
    if (ImGui::ColorEdit4("##Color", col,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        current_color_ = Color(
            static_cast<uint8_t>(col[0] * 255),
            static_cast<uint8_t>(col[1] * 255),
            static_cast<uint8_t>(col[2] * 255),
            static_cast<uint8_t>(col[3] * 255));
        if (current_tool_) current_tool_->set_foreground_color(current_color_);
    }
    ImGui::Separator();

    auto tool_btn = [&](const char* label, ToolType type) {
        bool active = (current_tool_type_ == type);
        if (active) ImGui::PushStyleColor(ImGuiCol_Button,
                        {0.0f, 1.0f, 0.255f, 0.3f});
        if (ImGui::Button(label, {-1, 0})) set_tool(type);
        if (active) ImGui::PopStyleColor();
    };

    tool_btn("[P] Pencil",  ToolType::Pencil);
    tool_btn("[E] Eraser",  ToolType::Eraser);
    tool_btn("[B] Bucket",  ToolType::Bucket);
    ImGui::Separator();
    tool_btn("[V] Pivot",   ToolType::Pivot);
    tool_btn("[H] Hitbox",  ToolType::Hitbox);
    ImGui::Separator();
    ImGui::Checkbox("Grid", &show_grid_);

    ImGui::End();
}

void ArchitectUI::render_canvas_viewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Architect");
    ImGui::PopStyleVar();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    viewport_.resize(avail.x, avail.y);

    ImVec2 origin = ImGui::GetCursorScreenPos();

    if (canvas_tex_id_ && canvas_) {
        float cw = static_cast<float>(canvas_->width());
        float ch = static_cast<float>(canvas_->height());
        Vec2 tl = viewport_.canvas_to_screen(0.0f, 0.0f);
        Vec2 br = viewport_.canvas_to_screen(cw, ch);

        ImVec2 img_tl = {origin.x + tl.x, origin.y + tl.y};
        ImVec2 img_br = {origin.x + br.x, origin.y + br.y};

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(intptr_t)canvas_tex_id_,
            img_tl, img_br);

        if (show_grid_ && viewport_.get_zoom() >= 4.0f) {
            float zoom = viewport_.get_zoom();
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImU32 grid_col = IM_COL32(42, 42, 42, 200);

            float step_canvas = 1.0f;
            if (zoom < 8.0f) step_canvas = 2.0f;
            if (zoom < 4.0f) step_canvas = 4.0f;

            float step_screen = step_canvas * zoom;
            float start_x = origin.x + tl.x;
            float start_y = origin.y + tl.y;
            float end_x   = origin.x + br.x;
            float end_y   = origin.y + br.y;

            for (float x = start_x; x <= end_x; x += step_screen)
                dl->AddLine({x, start_y}, {x, end_y}, grid_col);
            for (float y = start_y; y <= end_y; y += step_screen)
                dl->AddLine({start_x, y}, {end_x, y}, grid_col);
        }

        if (current_tool_type_ == ToolType::Pivot || hitbox_defined_) {
            Vec2 pv = viewport_.canvas_to_screen(pivot_point_.x, pivot_point_.y);
            ImVec2 pscreen = {origin.x + pv.x, origin.y + pv.y};
            float sz = 8.0f;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddLine({pscreen.x - sz, pscreen.y}, {pscreen.x + sz, pscreen.y},
                        IM_COL32(0, 255, 65, 255), 1.5f);
            dl->AddLine({pscreen.x, pscreen.y - sz}, {pscreen.x, pscreen.y + sz},
                        IM_COL32(0, 255, 65, 255), 1.5f);
            dl->AddCircle(pscreen, 4.0f, IM_COL32(0, 255, 65, 255));
        }

        if (hitbox_defined_) {
            Vec2 htl = viewport_.canvas_to_screen(hitbox_.x, hitbox_.y);
            Vec2 hbr = viewport_.canvas_to_screen(hitbox_.x + hitbox_.w,
                                                   hitbox_.y + hitbox_.h);
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRect({origin.x + htl.x, origin.y + htl.y},
                        {origin.x + hbr.x, origin.y + hbr.y},
                        IM_COL32(255, 144, 0, 200), 0.0f, 0, 1.5f);
        }
    }

    ImGui::SetCursorScreenPos(origin);
    ImGui::InvisibleButton("##canvas_input", avail);
    handle_canvas_input(origin, avail);

    ImGui::End();
}

void ArchitectUI::handle_canvas_input(ImVec2 canvas_origin, ImVec2 /*canvas_size*/) {
    if (!canvas_ || !current_tool_) return;

    ImGuiIO& io = ImGui::GetIO();

    if (ImGui::IsItemHovered()) {
        if (io.MouseWheel != 0.0f) {
            ImVec2 mouse = ImGui::GetMousePos();
            float local_x = mouse.x - canvas_origin.x;
            float local_y = mouse.y - canvas_origin.y;
            float factor = (io.MouseWheel > 0) ? 1.15f : (1.0f / 1.15f);
            viewport_.zoom_to_point(local_x, local_y, factor);
        }
    }

    bool alt_held  = io.KeyAlt;
    bool mid_down  = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    bool pan_active = mid_down || (alt_held && ImGui::IsMouseDown(ImGuiMouseButton_Left));

    if (ImGui::IsItemHovered() && pan_active && !is_panning_) {
        is_panning_  = true;
        pan_start_x_ = io.MousePos.x;
        pan_start_y_ = io.MousePos.y;
    }
    if (is_panning_) {
        if (pan_active) {
            viewport_.pan(io.MouseDelta.x, io.MouseDelta.y);
        } else {
            is_panning_ = false;
        }
        return;
    }

    if (!ImGui::IsItemHovered() && !is_painting_) return;
    if (alt_held) return;

    ImVec2 mouse = ImGui::GetMousePos();
    float local_x = mouse.x - canvas_origin.x;
    float local_y = mouse.y - canvas_origin.y;
    Vec2 cc = viewport_.screen_to_canvas(local_x, local_y);
    int cx = static_cast<int>(cc.x);
    int cy = static_cast<int>(cc.y);

    bool lmb = ImGui::IsMouseDown(ImGuiMouseButton_Left);

    if (lmb && !is_painting_) {
        is_painting_  = true;
        last_mouse_x_ = cx;
        last_mouse_y_ = cy;
        current_tool_->on_mouse_down(canvas_, cx, cy);
        canvas_dirty_ = true;

        if (current_tool_type_ == ToolType::Pivot) {
            pivot_point_ = {static_cast<float>(cx), static_cast<float>(cy)};
        }
    } else if (lmb && is_painting_) {
        if (cx != last_mouse_x_ || cy != last_mouse_y_) {
            current_tool_->on_mouse_drag(canvas_, last_mouse_x_, last_mouse_y_, cx, cy);
            last_mouse_x_ = cx;
            last_mouse_y_ = cy;
            canvas_dirty_ = true;

            if (current_tool_type_ == ToolType::Hitbox) {
                if (auto* ht = dynamic_cast<HitboxTool*>(current_tool_.get()))
                    hitbox_ = ht->get_hitbox();
            }
        }
    } else if (!lmb && is_painting_) {
        is_painting_ = false;
        current_tool_->on_mouse_up(canvas_, cx, cy);
        canvas_dirty_ = true;

        if (current_tool_type_ == ToolType::Hitbox) {
            if (auto* ht = dynamic_cast<HitboxTool*>(current_tool_.get())) {
                hitbox_         = ht->get_hitbox();
                hitbox_defined_ = (hitbox_.w > 0 && hitbox_.h > 0);
            }
        }
    }
}

void ArchitectUI::render_layers_panel() {
    ImGui::Begin("Layers##Architect");
    if (canvas_) {
        auto& layers = canvas_->get_layers();
        for (size_t i = 0; i < layers.size(); ++i) {
            auto& layer    = layers[i];
            bool is_active = (canvas_->get_active_layer() == &layer);
            if (ImGui::Selectable(layer.name.c_str(), is_active))
                canvas_->set_active_layer(static_cast<int>(i));
            ImGui::SameLine();
            bool vis = layer.visible;
            if (ImGui::Checkbox(("##V" + std::to_string(i)).c_str(), &vis))
                layer.visible = vis;
        }
        ImGui::Separator();
        if (ImGui::SmallButton("+##addlayer")) {
            canvas_->add_layer("Layer " + std::to_string(layers.size() + 1));
        }
    }
    ImGui::End();
}

void ArchitectUI::render_properties_panel() {
    ImGui::Begin("Properties##Architect");
    if (canvas_) {
        ImGui::TextDisabled("Canvas");
        ImGui::Text("%d x %d px", canvas_->width(), canvas_->height());
        ImGui::Separator();
        ImGui::TextDisabled("Viewport");
        float zoom = viewport_.get_zoom();
        if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 32.0f, "%.1fx"))
            viewport_.set_zoom(zoom);
        ImGui::Separator();
        ImGui::TextDisabled("Metadata");
        ImGui::Text("Pivot: (%.0f, %.0f)", pivot_point_.x, pivot_point_.y);
        if (hitbox_defined_)
            ImGui::Text("Hitbox: %.0f,%.0f  %.0fx%.0f",
                        hitbox_.x, hitbox_.y, hitbox_.w, hitbox_.h);
        else
            ImGui::TextDisabled("No hitbox defined");
    }
    ImGui::End();
}

}
}
