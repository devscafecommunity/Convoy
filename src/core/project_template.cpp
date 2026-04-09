#include "project_template.h"

#include <fstream>
#include <sstream>

namespace convoy
{

bool ProjectTemplate::load_from_file(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();

    size_t name_pos = json.find("\"name\"");
    if (name_pos != std::string::npos)
    {
        size_t start = json.find(":", name_pos);
        size_t q1 = json.find("\"", start);
        size_t q2 = json.find("\"", q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos)
        {
            name = json.substr(q1 + 1, q2 - q1 - 1);
        }
    }

    if (json.find("\"PixelArt\"") != std::string::npos)
        mode = ProjectMode::PixelArt;
    else if (json.find("\"Isometric\"") != std::string::npos)
        mode = ProjectMode::Isometric;
    else if (json.find("\"TopDown\"") != std::string::npos)
        mode = ProjectMode::TopDown;
    else if (json.find("\"Drawing\"") != std::string::npos)
        mode = ProjectMode::Drawing;

    return true;
}

bool ProjectTemplate::save_to_file(const std::string& path) const
{
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << "{\n";
    file << "  \"name\": \"" << name << "\",\n";
    file << "  \"mode\": \"";

    switch (mode)
    {
        case ProjectMode::PixelArt:
            file << "PixelArt";
            break;
        case ProjectMode::Drawing:
            file << "Drawing";
            break;
        case ProjectMode::Isometric:
            file << "Isometric";
            break;
        case ProjectMode::TopDown:
            file << "TopDown";
            break;
    }

    file << "\",\n";
    file << "  \"canvas_width\": " << config.canvas_width << ",\n";
    file << "  \"canvas_height\": " << config.canvas_height << ",\n";
    file << "  \"bit_depth\": " << config.bit_depth << ",\n";
    file << "  \"grid_type\": " << static_cast<int>(config.grid.type) << ",\n";
    file << "  \"tile_width\": " << config.grid.tile_width << ",\n";
    file << "  \"tile_height\": " << config.grid.tile_height << ",\n";
    file << "  \"snap_to_grid\": " << (config.snapping.snap_to_grid ? "true" : "false") << "\n";
    file << "}\n";

    return true;
}

void TemplateManager::load_builtin_templates()
{
    ProjectTemplate tmpl;
    tmpl.name = "Pixel Art";
    tmpl.mode = ProjectMode::PixelArt;
    tmpl.config = ProjectModeConfig::default_pixel_art();
    templates_.push_back(tmpl);

    tmpl.name = "Isometric";
    tmpl.mode = ProjectMode::Isometric;
    tmpl.config = ProjectModeConfig::default_isometric();
    templates_.push_back(tmpl);

    tmpl.name = "Top-Down";
    tmpl.mode = ProjectMode::TopDown;
    tmpl.config = ProjectModeConfig::default_top_down();
    templates_.push_back(tmpl);

    tmpl.name = "Drawing (Hi-Res)";
    tmpl.mode = ProjectMode::Drawing;
    tmpl.config = ProjectModeConfig::default_drawing();
    templates_.push_back(tmpl);
}

std::vector<ProjectTemplate> TemplateManager::get_templates() const
{
    return templates_;
}

const ProjectTemplate* TemplateManager::find(const std::string& name) const
{
    for (const auto& t : templates_)
    {
        if (t.name == name)
            return &t;
    }
    return nullptr;
}

}  // namespace convoy
