#include "caf_exporter.h"
#include "exporter_plugin_interface.h"

namespace convoy
{

ExportManager& ExportManager::instance()
{
    static ExportManager instance;
    return instance;
}

void ExportManager::register_exporter(std::shared_ptr<IExporterPlugin> exporter)
{
    exporters_.push_back(exporter);
}

IExporterPlugin* ExportManager::find_by_format(ExportFormat format) const
{
    for (const auto& exp : exporters_)
    {
        if (exp->get_format() == format)
            return exp.get();
    }
    return nullptr;
}

void ExportManager::export_canvas(const std::string& path,
                                  ExportFormat format,
                                  const std::vector<uint8_t>& pixel_data,
                                  int width,
                                  int height,
                                  const ExportSettings& settings)
{
    if (format == ExportFormat::CAF)
    {
        CAFExporter::export_to_file(path.c_str(),
                                    pixel_data.data(),
                                    static_cast<uint16_t>(width),
                                    static_cast<uint16_t>(height),
                                    nullptr,
                                    0,
                                    0);
        return;
    }

    auto* exporter = find_by_format(format);
    if (exporter)
    {
        exporter->export_to_file(path, pixel_data, width, height, settings);
    }
}

}  // namespace convoy
