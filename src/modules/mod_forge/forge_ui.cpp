#include "forge_ui.h"
#include <imgui.h>

namespace convoy { namespace forge {

void ForgeUI::render() {
    ImGui::Begin("Forge: Atlas Packer##Forge");
    ImGui::TextDisabled("Forge module - Fase 2 (Jun/Ago 2026)");
    ImGui::Separator();
    ImGui::Text("Sprites loaded: 0");
    ImGui::Text("Atlas size:     not packed");
    ImGui::Separator();
    if (ImGui::Button("Pack Atlas")) {
    }
    ImGui::End();
}

}}
