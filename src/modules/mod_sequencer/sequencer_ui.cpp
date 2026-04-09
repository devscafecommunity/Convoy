#include "sequencer_ui.h"
#include <imgui.h>
#include <string>

namespace convoy { namespace sequencer {

void SequencerUI::render() {
    ImGui::Begin("Sequencer: Animation##Seq");
    ImGui::TextDisabled("Sequencer module - Fase 2 (Jun/Ago 2026)");
    ImGui::Separator();

    ImGui::SliderInt("FPS",    &fps_,         1, 60);
    ImGui::SliderInt("Frames", &frame_count_, 1, 64);
    ImGui::Separator();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 orig    = ImGui::GetCursorScreenPos();
    float avail_w  = ImGui::GetContentRegionAvail().x;
    float frame_w  = (frame_count_ > 0) ? avail_w / frame_count_ : avail_w;
    float height   = 32.0f;

    for (int i = 0; i < frame_count_; ++i) {
        float x   = orig.x + i * frame_w;
        ImU32 col = (i == current_frame_)
            ? IM_COL32(0, 255, 65, 200)
            : IM_COL32(42, 42, 42, 255);
        dl->AddRectFilled({x, orig.y}, {x + frame_w - 2, orig.y + height}, col);
        dl->AddRect      ({x, orig.y}, {x + frame_w - 2, orig.y + height},
                          IM_COL32(42, 42, 42, 255));
        std::string label = std::to_string(i + 1);
        dl->AddText({x + 4, orig.y + 8}, IM_COL32(200, 200, 200, 255), label.c_str());
    }
    ImGui::Dummy({avail_w, height});
    ImGui::Separator();

    if (ImGui::Button(playing_ ? "|| Pause" : "> Play"))
        playing_ = !playing_;
    ImGui::SameLine();
    if (ImGui::Button("|< Reset")) { current_frame_ = 0; playing_ = false; }

    ImGui::End();
}

}}
