#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace convoy
{

struct ExportSettings
{
    int scale = 1;
    int padding = 0;
    bool transparency = true;
    bool export_collision_json = false;
    bool apply_crt_filters = false;
};

enum class ExportFormat
{
    CAF,
    PNG,
    TGA,
    BMP,
    Spritesheet,
    JSON_Metadata
};

class IExporterPlugin
{
   public:
    virtual ~IExporterPlugin() = default;

    virtual std::string get_extension() const = 0;
    virtual std::string get_display_name() const = 0;
    virtual ExportFormat get_format() const = 0;

    virtual bool export_to_file(const std::string& path,
                                const std::vector<uint8_t>& pixel_data,
                                int width,
                                int height,
                                const ExportSettings& settings) = 0;

    virtual void render_settings() = 0;
};

class ExportManager
{
   public:
    static ExportManager& instance();

    void register_exporter(std::shared_ptr<IExporterPlugin> exporter);
    const std::vector<std::shared_ptr<IExporterPlugin>>& get_all_exporters() const { return exporters_; }
    IExporterPlugin* find_by_format(ExportFormat format) const;

    void export_canvas(const std::string& path,
                       ExportFormat format,
                       const std::vector<uint8_t>& pixel_data,
                       int width,
                       int height,
                       const ExportSettings& settings);

   private:
    ExportManager() = default;
    std::vector<std::shared_ptr<IExporterPlugin>> exporters_;
};

}  // namespace convoy
