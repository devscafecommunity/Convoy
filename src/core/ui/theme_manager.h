#pragma once
#include <imgui.h>

namespace convoy {

struct DarkTerminalPalette {
    // #0A0A0A - deep black background
    static constexpr ImVec4 BG_DEEP     = {0.039f, 0.039f, 0.039f, 1.0f};
    // #151515 - panel background
    static constexpr ImVec4 BG_PANEL    = {0.082f, 0.082f, 0.082f, 1.0f};
    // #1F1F1F - widget background
    static constexpr ImVec4 BG_WIDGET   = {0.122f, 0.122f, 0.122f, 1.0f};
    // #2A2A2A - borders
    static constexpr ImVec4 BORDER      = {0.165f, 0.165f, 0.165f, 1.0f};
    // #00FF41 - Matrix green accent
    static constexpr ImVec4 ACCENT      = {0.0f,   1.0f,   0.255f, 1.0f};
    // #FF9000 - warning orange
    static constexpr ImVec4 WARNING     = {1.0f,   0.565f, 0.0f,   1.0f};
    // #E0E0E0 - primary text
    static constexpr ImVec4 TEXT        = {0.878f, 0.878f, 0.878f, 1.0f};
    // #787878 - dimmed text
    static constexpr ImVec4 TEXT_DIM    = {0.471f, 0.471f, 0.471f, 1.0f};
    // Active element (slightly lighter panel)
    static constexpr ImVec4 BG_ACTIVE   = {0.18f,  0.18f,  0.18f,  1.0f};
    // Hovered element
    static constexpr ImVec4 BG_HOVERED  = {0.14f,  0.14f,  0.14f,  1.0f};
};

class ThemeManager {
public:
    static void apply_dark_terminal();
};

} // namespace convoy
