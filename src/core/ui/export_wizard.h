#pragma once
#include <functional>
#include <string>

#include "../export/exporter_plugin_interface.h"

namespace convoy
{

class ExportWizard
{
   public:
    using ExportCallback =
        std::function<void(const std::string& path, ExportFormat format, const ExportSettings& settings)>;

    void set_callback(ExportCallback cb) { on_export_ = cb; }
    void render();
    bool is_open() const { return is_open_; }
    void open() { is_open_ = true; }
    void close() { is_open_ = false; }

   private:
    void render_format_selection();
    void render_basic_options();
    void render_preview();
    void render_path_input();

    bool is_open_ = false;
    ExportFormat selected_format_ = ExportFormat::PNG;
    ExportSettings settings_;
    std::string output_path_ = "./export.png";
    ExportCallback on_export_;
};

}  // namespace convoy
