#include "dod_visualizer.h"
#include "core/ui/theme_manager.h"
#include <imgui.h>
#include <cstdio>

namespace convoy { namespace architect {

void DODVisualizer::render_memory_bar(size_t used_bytes, size_t total_bytes) {
    float fraction = total_bytes > 0
        ? static_cast<float>(used_bytes) / static_cast<float>(total_bytes)
        : 0.0f;

    ImVec4 color = fraction > 0.8f
        ? DarkTerminalPalette::WARNING
        : DarkTerminalPalette::ACCENT;

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
    char overlay[64];
    snprintf(overlay, sizeof(overlay), "%zu KB / %zu KB",
             used_bytes / 1024, total_bytes / 1024);
    ImGui::ProgressBar(fraction, {-1, 0}, overlay);
    ImGui::PopStyleColor();
}

void DODVisualizer::render_alignment_badge(const AlignmentInfo& info) {
    if (info.is_aligned) {
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::ACCENT);
        ImGui::Text("ALIGNED [32-byte OK]  +0 bytes waste");
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::WARNING);
        ImGui::Text("UNALIGNED! Needs +%d bytes to align", info.waste_bytes);
        ImGui::PopStyleColor();
    }
}

void DODVisualizer::render(bool* open, const Canvas* canvas) {
    if (!ImGui::Begin("DOD Visualizer##Architect", open)) { ImGui::End(); return; }

    if (!canvas) {
        ImGui::TextDisabled("No canvas loaded");
        ImGui::End();
        return;
    }

    ImGui::TextDisabled("CANVAS DATA DIAGNOSTICS");
    ImGui::Separator();

    size_t vram     = compute_vram_estimate(canvas);
    size_t vram_max = 8 * 1024 * 1024;

    ImGui::Text("Layers: %zu", canvas->get_layers().size());
    ImGui::Text("VRAM Estimate:");
    render_memory_bar(vram, vram_max);

    ImGui::Separator();
    ImGui::TextDisabled("GPU ALIGNMENT CHECK (32-byte)");
    AlignmentInfo info = compute_alignment(canvas);
    ImGui::Text("Raw size: %zu bytes", info.data_bytes);
    render_alignment_badge(info);
    if (!info.is_aligned) {
        ImGui::TextDisabled("Aligned size would be: %zu bytes", info.aligned_bytes);
    }

    ImGui::End();
}

}}
