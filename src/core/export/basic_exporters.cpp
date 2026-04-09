#include <fstream>
#include <sstream>

#include "exporter_plugin_interface.h"
#include <imgui.h>

namespace convoy
{

class JsonMetadataExporter : public IExporterPlugin
{
   public:
    std::string get_extension() const override { return "json"; }
    std::string get_display_name() const override { return "JSON Metadata"; }
    ExportFormat get_format() const override { return ExportFormat::JSON_Metadata; }

    bool export_to_file(const std::string& path,
                        const std::vector<uint8_t>& pixel_data,
                        int width,
                        int height,
                        const ExportSettings& settings) override
    {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        file << "{\n";
        file << "  \"width\": " << width << ",\n";
        file << "  \"height\": " << height << ",\n";
        file << "  \"scale\": " << settings.scale << ",\n";

        file << "  \"pivot\": {\"x\": 0, \"y\": 0},\n";

        file << "  \"collision\": {\n";
        file << "    \"width\": 0,\n";
        file << "    \"height\": 0,\n";
        file << "    \"boxes\": []\n";
        file << "  },\n";

        file << "  \"metadata\": {\n";
        file << "    \"format\": \"Convoy Export\",\n";
        file << "    \"version\": \"1.0\"\n";
        file << "  }\n";
        file << "}\n";

        return true;
    }

    void render_settings() override { ImGui::TextDisabled("JSON metadata export options"); }
};

class PngExporter : public IExporterPlugin
{
   public:
    std::string get_extension() const override { return "png"; }
    std::string get_display_name() const override { return "PNG Image"; }
    ExportFormat get_format() const override { return ExportFormat::PNG; }

    bool export_to_file(const std::string& path,
                        const std::vector<uint8_t>& pixel_data,
                        int width,
                        int height,
                        const ExportSettings& settings) override
    {
        return false;
    }

    void render_settings() override { ImGui::TextDisabled("PNG options - basic export"); }
};

}  // namespace convoy
