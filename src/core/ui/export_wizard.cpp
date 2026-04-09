#include "export_wizard.h"

#include <imgui.h>

#include <cstring>

namespace convoy
{

void ExportWizard::render()
{
    if (!is_open_)
        return;

    ImGui::SetNextWindowSize({600, 450});
    if (!ImGui::Begin("Export Asset", &is_open_))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginChild("format_selection", {200, 0}))
    {
        ImGui::Text("Destination Format");
        ImGui::Separator();

        bool caf = selected_format_ == ExportFormat::CAF;
        bool png = selected_format_ == ExportFormat::PNG;
        bool tga = selected_format_ == ExportFormat::TGA;
        bool bmp = selected_format_ == ExportFormat::BMP;
        bool sheet = selected_format_ == ExportFormat::Spritesheet;
        bool json = selected_format_ == ExportFormat::JSON_Metadata;

        if (ImGui::RadioButton("Caffeine Engine (.caf)", caf))
            selected_format_ = ExportFormat::CAF;
        if (ImGui::RadioButton("Web/Generic (.png)", png))
            selected_format_ = ExportFormat::PNG;
        if (ImGui::RadioButton("TGA (.tga)", tga))
            selected_format_ = ExportFormat::TGA;
        if (ImGui::RadioButton("BMP (.bmp)", bmp))
            selected_format_ = ExportFormat::BMP;
        if (ImGui::RadioButton("Spritesheet (.png+json)", sheet))
            selected_format_ = ExportFormat::Spritesheet;
        if (ImGui::RadioButton("JSON Metadata Only", json))
            selected_format_ = ExportFormat::JSON_Metadata;
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if (ImGui::BeginChild("options", {0, 0}))
    {
        ImGui::Text("Basic Options");
        ImGui::Separator();

        ImGui::Text("Scale");
        int scale = settings_.scale;
        ImGui::RadioButton("1x", scale == 1);
        ImGui::SameLine();
        ImGui::RadioButton("2x", scale == 2);
        ImGui::SameLine();
        ImGui::RadioButton("4x", scale == 4);
        settings_.scale = scale;

        ImGui::Text("Padding");
        ImGui::SliderInt("##padding", &settings_.padding, 0, 16, "%dpx");

        ImGui::Checkbox("Transparency", &settings_.transparency);

        ImGui::Separator();
        ImGui::Text("Extra Options");

        ImGui::Checkbox("Export Collision as JSON", &settings_.export_collision_json);
        ImGui::Checkbox("Apply CRT Filters", &settings_.apply_crt_filters);

        ImGui::Separator();
        render_path_input();
    }
    ImGui::EndChild();

    ImGui::End();
}

void ExportWizard::render_path_input()
{
    ImGui::Text("Output Path");
    char path_buffer[512] = {};
    std::strncpy(path_buffer, output_path_.c_str(), sizeof(path_buffer) - 1);
    ImGui::InputText("##path", path_buffer, sizeof(path_buffer));
    output_path_ = path_buffer;

    if (ImGui::Button("Cancel"))
    {
        close();
    }

    ImGui::SameLine();

    if (ImGui::Button("Export!"))
    {
        if (on_export_)
        {
            on_export_(output_path_, selected_format_, settings_);
        }
        close();
    }
}

}  // namespace convoy
