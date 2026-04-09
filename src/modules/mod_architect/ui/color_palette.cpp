#include "color_palette.h"
#include <imgui.h>
#include <cmath>

namespace convoy { namespace architect {

IndexedPalette IndexedPalette::make_default_16() {
    IndexedPalette p;
    p.name = "Default 16";
    p.colors = {
        {0,0,0,255},     {255,255,255,255}, {127,127,127,255}, {64,64,64,255},
        {255,0,0,255},   {0,255,0,255},     {0,0,255,255},     {255,255,0,255},
        {255,0,255,255}, {0,255,255,255},   {255,127,0,255},   {127,0,255,255},
        {0,127,255,255}, {255,0,127,255},   {0,255,127,255},   {127,255,0,255},
    };
    return p;
}

IndexedPalette IndexedPalette::make_db32() {
    IndexedPalette p;
    p.name = "DB32";
    p.colors = {
        {0,0,0,255},       {34,32,52,255},    {69,40,60,255},    {102,57,49,255},
        {143,86,59,255},   {223,113,38,255},  {217,160,102,255}, {238,195,154,255},
        {251,242,54,255},  {153,229,80,255},  {106,190,48,255},  {55,148,110,255},
        {75,105,47,255},   {82,75,36,255},    {50,60,57,255},    {63,63,116,255},
        {48,96,130,255},   {91,110,225,255},  {99,155,255,255},  {95,205,228,255},
        {203,219,252,255}, {255,255,255,255}, {155,173,183,255}, {132,126,135,255},
        {105,106,106,255}, {89,86,82,255},    {118,66,138,255},  {172,50,50,255},
        {217,87,99,255},   {215,123,186,255}, {143,151,74,255},  {138,111,48,255},
    };
    return p;
}

void ColorPalette::render(bool* open) {
    if (!ImGui::Begin("Palette##Architect", open)) { ImGui::End(); return; }

    ImGui::TextDisabled("%s", palette_.name.c_str());
    ImGui::Separator();

    float swatch = 18.0f;
    float pad    = 2.0f;
    float region = ImGui::GetContentRegionAvail().x;
    int   cols   = std::max(1, static_cast<int>((region + pad) / (swatch + pad)));

    for (int i = 0; i < static_cast<int>(palette_.colors.size()); ++i) {
        const Color& c = palette_.colors[i];

        ImGui::PushID(i);
        bool selected = (i == sel_idx_);

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            cursor, {cursor.x + swatch, cursor.y + swatch},
            IM_COL32(c.r, c.g, c.b, 255));
        if (selected) {
            ImGui::GetWindowDrawList()->AddRect(
                cursor, {cursor.x + swatch, cursor.y + swatch},
                IM_COL32(0, 255, 65, 255), 0, 0, 2.0f);
        }

        ImGui::InvisibleButton("##swatch", {swatch, swatch});
        if (ImGui::IsItemClicked()) {
            selected_  = c;
            sel_idx_   = i;
            if (on_select_) on_select_(c);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("R:%d G:%d B:%d", c.r, c.g, c.b);
        }

        ImGui::PopID();
        if ((i + 1) % cols != 0)
            ImGui::SameLine(0.0f, pad);
    }

    ImGui::End();
}

}}
