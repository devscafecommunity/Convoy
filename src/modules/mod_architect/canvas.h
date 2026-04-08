#pragma once
#include "shared/types.h"
#include <vector>
#include <memory>

namespace convoy {
namespace architect {

class Canvas {
public:
    Canvas(uint32_t width, uint32_t height);

    void set_pixel(uint32_t x, uint32_t y, Color color);
    Color get_pixel(uint32_t x, uint32_t y) const;

    void add_layer(const std::string& name);
    void remove_layer(int index);
    void set_active_layer(int index);

    Layer* get_active_layer();
    const Layer* get_active_layer() const;

    const std::vector<Layer>& get_layers() const { return layers_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }

    void clear_active_layer();
    void composite_to_texture(std::vector<uint32_t>& output);

private:
    uint32_t width_, height_;
    std::vector<Layer> layers_;
    int active_layer_;

    bool is_in_bounds(uint32_t x, uint32_t y) const;
};

} // namespace architect
} // namespace convoy
