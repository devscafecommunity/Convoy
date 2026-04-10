#pragma once
#include <memory>
#include <string>
#include <vector>

#include "shared/brush.h"
#include "ui_component.h"

namespace convoy
{

struct PluginInfo
{
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string type;
};

class IConvoyPlugin : public UIComponent
{
   public:
    virtual ~IConvoyPlugin() = default;
    virtual void on_load() = 0;
    virtual void on_unload() = 0;
    virtual PluginInfo info() const = 0;
};

class IBrushPlugin : public IConvoyPlugin
{
   public:
    virtual std::string get_brush_name() const = 0;
    virtual void on_stroke_begin(float x, float y, float pressure) = 0;
    virtual void on_stroke_update(float x, float y, float pressure) = 0;
    virtual void on_stroke_end() = 0;

    virtual std::vector<uint8_t> generate_mask(int size, float pressure, float angle) const { return {}; }

    virtual BrushShape get_shape() const { return BrushShape::Custom; }

    virtual DynamicMaskMode get_dynamic_mode() const { return DynamicMaskMode::None; }
};

class IProceduralBrushPlugin : public IBrushPlugin
{
   public:
    virtual std::vector<uint8_t> generate_perlin_noise_mask(int size, float scale) const = 0;
    virtual std::vector<uint8_t> generate_dither_mask(int size, int pattern_size) const = 0;
    virtual std::vector<uint8_t> generate_brush_stroke_mask(int size, float angle, float pressure) const = 0;
};

}  // namespace convoy
