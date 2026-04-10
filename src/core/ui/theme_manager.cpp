#include "theme_manager.h"

namespace convoy
{

void ThemeManager::apply_dark_terminal()
{
    ImGuiStyle& s = ImGui::GetStyle();
    using P = DarkTerminalPalette;

    s.WindowRounding = 2.0f;
    s.ChildRounding = 2.0f;
    s.FrameRounding = 2.0f;
    s.PopupRounding = 2.0f;
    s.ScrollbarRounding = 2.0f;
    s.GrabRounding = 2.0f;
    s.TabRounding = 2.0f;

    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.PopupBorderSize = 1.0f;

    s.WindowPadding = {8.0f, 6.0f};
    s.FramePadding = {6.0f, 3.0f};
    s.ItemSpacing = {6.0f, 4.0f};
    s.ScrollbarSize = 10.0f;
    s.GrabMinSize = 8.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg] = P::BG_PANEL;
    c[ImGuiCol_ChildBg] = P::BG_PANEL;
    c[ImGuiCol_PopupBg] = P::BG_PANEL;
    c[ImGuiCol_Border] = P::BORDER;
    c[ImGuiCol_BorderShadow] = {0, 0, 0, 0};
    c[ImGuiCol_FrameBg] = P::BG_WIDGET;
    c[ImGuiCol_FrameBgHovered] = P::BG_HOVERED;
    c[ImGuiCol_FrameBgActive] = P::BG_ACTIVE;
    c[ImGuiCol_TitleBg] = P::BG_DEEP;
    c[ImGuiCol_TitleBgActive] = P::BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed] = P::BG_DEEP;
    c[ImGuiCol_MenuBarBg] = P::BG_DEEP;
    c[ImGuiCol_ScrollbarBg] = P::BG_DEEP;
    c[ImGuiCol_ScrollbarGrab] = P::BORDER;
    c[ImGuiCol_ScrollbarGrabHovered] = P::BG_ACTIVE;
    c[ImGuiCol_ScrollbarGrabActive] = P::ACCENT;
    c[ImGuiCol_CheckMark] = P::ACCENT;
    c[ImGuiCol_SliderGrab] = P::ACCENT;
    c[ImGuiCol_SliderGrabActive] = P::ACCENT;
    c[ImGuiCol_Button] = P::BG_WIDGET;
    c[ImGuiCol_ButtonHovered] = P::BG_HOVERED;
    c[ImGuiCol_ButtonActive] = P::BG_ACTIVE;
    c[ImGuiCol_Header] = P::BG_WIDGET;
    c[ImGuiCol_HeaderHovered] = P::BG_HOVERED;
    c[ImGuiCol_HeaderActive] = P::BG_ACTIVE;
    c[ImGuiCol_Separator] = P::BORDER;
    c[ImGuiCol_SeparatorHovered] = P::ACCENT;
    c[ImGuiCol_SeparatorActive] = P::ACCENT;
    c[ImGuiCol_ResizeGrip] = P::BG_WIDGET;
    c[ImGuiCol_ResizeGripHovered] = P::ACCENT;
    c[ImGuiCol_ResizeGripActive] = P::ACCENT;
    c[ImGuiCol_Tab] = P::BG_DEEP;
    c[ImGuiCol_TabHovered] = P::BG_ACTIVE;
    c[ImGuiCol_TabActive] = P::BG_PANEL;
    c[ImGuiCol_TabUnfocused] = P::BG_DEEP;
    c[ImGuiCol_TabUnfocusedActive] = P::BG_PANEL;
    // Docking colors not available in this ImGui version
    // c[ImGuiCol_DockingPreview]       = {P::ACCENT.x, P::ACCENT.y, P::ACCENT.z, 0.3f};
    // c[ImGuiCol_DockingEmptyBg]       = P::BG_DEEP;
    c[ImGuiCol_PlotLines] = P::ACCENT;
    c[ImGuiCol_PlotHistogram] = P::ACCENT;
    c[ImGuiCol_TableHeaderBg] = P::BG_DEEP;
    c[ImGuiCol_TableBorderStrong] = P::BORDER;
    c[ImGuiCol_TableBorderLight] = P::BG_WIDGET;
    c[ImGuiCol_Text] = P::TEXT;
    c[ImGuiCol_TextDisabled] = P::TEXT_DIM;
    c[ImGuiCol_NavHighlight] = P::ACCENT;
    c[ImGuiCol_ModalWindowDimBg] = {0.0f, 0.0f, 0.0f, 0.6f};

    s.Colors[ImGuiCol_WindowBg].w = 1.0f;
}

}  // namespace convoy
