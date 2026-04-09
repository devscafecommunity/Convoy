#pragma once
#include <string>

#include "modules/mod_architect/canvas.h"
#include "shared/brush.h"

namespace convoy
{
namespace architect
{

class BrushSettings
{
   public:
    void render();

    Brush get_brush() const { return current_brush_; }

    void set_size(uint8_t size) { current_brush_.size = size; }
    void set_hardness(uint8_t hardness) { current_brush_.hardness = hardness; }
    void set_spacing(uint8_t spacing) { current_brush_.spacing = spacing; }
    void set_shape(BrushShape shape) { current_brush_.shape = shape; }
    void set_curve(PressureCurve curve) { current_brush_.curve = curve; }

    void set_custom_mask(const std::vector<uint8_t>& mask)
    {
        current_brush_.custom_mask = mask;
        current_brush_.has_custom_mask = !mask.empty();
    }

    void capture_from_canvas(const Canvas* canvas, int x, int y, int w, int h);
    bool import_from_png(const std::string& path);

   private:
    void render_preview_tab();
    void render_mask_settings_tab();
    void render_mask_source_tab();

    Brush current_brush_;
    bool is_open_ = true;
    int selected_tab_ = 0;
};

}  // namespace architect
}  // namespace convoy
