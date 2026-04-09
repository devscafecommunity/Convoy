#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace convoy
{

struct Color
{
    uint8_t r, g, b, a;

    static Color from_rgba(uint32_t rgba)
    {
        return {static_cast<uint8_t>((rgba >> 24) & 0xFF),
                static_cast<uint8_t>((rgba >> 16) & 0xFF),
                static_cast<uint8_t>((rgba >> 8) & 0xFF),
                static_cast<uint8_t>(rgba & 0xFF)};
    }

    uint32_t to_rgba() const
    {
        return (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) | (static_cast<uint32_t>(b) << 8) |
               static_cast<uint32_t>(a);
    }
};

enum class ColorModel
{
    HSV,
    HSL,
    HSI,
    HSY_Prime
};

enum class SelectorShape
{
    Triangle,
    Square,
    Wheel
};

enum class HarmonyType
{
    None,
    Complementary,
    Triad,
    Analogous,
    SplitComplementary
};

struct ColorHSV
{
    float h;
    float s;
    float v;
};

struct ColorHSL
{
    float h;
    float s;
    float l;
};

struct ColorHSY
{
    float h;
    float s;
    float y;
};

struct ColorLumaCoeffs
{
    float r;
    float g;
    float b;

    static ColorLumaCoeffs rec_709() { return {0.2126f, 0.7152f, 0.0722f}; }

    static ColorLumaCoeffs rec_601() { return {0.299f, 0.587f, 0.114f}; }
};

class ColorMath
{
   public:
    static ColorHSV rgb_to_hsv(Color rgb);
    static Color hsv_to_rgb(ColorHSV hsv);

    static ColorHSL rgb_to_hsl(Color rgb);
    static Color hsl_to_rgb(ColorHSL hsl);

    static ColorHSY rgb_to_hsy(Color rgb, const ColorLumaCoeffs& coeffs = ColorLumaCoeffs::rec_709());
    static Color hsy_to_rgb(ColorHSY hsy, const ColorLumaCoeffs& coeffs = ColorLumaCoeffs::rec_709());

    static float calculate_luma(Color rgb, const ColorLumaCoeffs& coeffs = ColorLumaCoeffs::rec_709());

    static Color apply_gamma(Color rgb, float gamma);
    static Color remove_gamma(Color rgb, float gamma);

    static Color darken(Color rgb, float amount);
    static Color lighten(Color rgb, float amount);
    static Color desaturate(Color rgb, float amount);
    static Color saturate(Color rgb, float amount);

    static Color shift_hue(Color rgb, float degrees);

    static std::vector<Color> generate_shadow_row(Color base, int count, bool warm);
    static std::vector<Color> generate_harmony(Color base, HarmonyType type);
};

class ColorState
{
   public:
    Color current = {128, 128, 128, 255};
    ColorModel model = ColorModel::HSY_Prime;
    SelectorShape shape = SelectorShape::Triangle;
    float gamma = 2.2f;
    ColorLumaCoeffs luma_coeffs = ColorLumaCoeffs::rec_709();

    std::vector<Color> history;
    std::vector<Color> common_colors;
    static constexpr size_t max_history = 20;

    void add_to_history(Color c);
    void extract_from_pixels(const uint8_t* pixels, int width, int height);
};

}  // namespace convoy
