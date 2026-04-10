#include "shared/color_math.h"

#include <algorithm>
#include <cmath>

namespace convoy
{

ColorHSV ColorMath::rgb_to_hsv(Color rgb)
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;

    float h = 0.0f;
    if (delta > 0.0f)
    {
        if (max == r)
            h = 60.0f * std::fmod((g - b) / delta, 6.0f);
        else if (max == g)
            h = 60.0f * ((b - r) / delta + 2.0f);
        else
            h = 60.0f * ((r - g) / delta + 4.0f);
    }
    if (h < 0.0f)
        h += 360.0f;

    float s = (max > 0.0f) ? delta / max : 0.0f;
    float v = max;

    return {h, s, v};
}

Color ColorMath::hsv_to_rgb(ColorHSV hsv)
{
    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;

    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r, g, b;
    if (h < 60.0f)
    {
        r = c;
        g = x;
        b = 0;
    }
    else if (h < 120.0f)
    {
        r = x;
        g = c;
        b = 0;
    }
    else if (h < 180.0f)
    {
        r = 0;
        g = c;
        b = x;
    }
    else if (h < 240.0f)
    {
        r = 0;
        g = x;
        b = c;
    }
    else if (h < 300.0f)
    {
        r = x;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = x;
    }

    return {static_cast<uint8_t>((r + m) * 255.0f),
            static_cast<uint8_t>((g + m) * 255.0f),
            static_cast<uint8_t>((b + m) * 255.0f),
            255};
}

ColorHSL ColorMath::rgb_to_hsl(Color rgb)
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;

    float h = 0.0f;
    if (delta > 0.0f)
    {
        if (max == r)
            h = 60.0f * std::fmod((g - b) / delta, 6.0f);
        else if (max == g)
            h = 60.0f * ((b - r) / delta + 2.0f);
        else
            h = 60.0f * ((r - g) / delta + 4.0f);
    }
    if (h < 0.0f)
        h += 360.0f;

    float l = (max + min) / 2.0f;
    float s = (delta > 0.0f) ? delta / (1.0f - std::abs(2.0f * l - 1.0f)) : 0.0f;

    return {h, s, l};
}

Color ColorMath::hsl_to_rgb(ColorHSL hsl)
{
    float h = hsl.h;
    float s = hsl.s;
    float l = hsl.l;

    float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    float r, g, b;
    if (h < 60.0f)
    {
        r = c;
        g = x;
        b = 0;
    }
    else if (h < 120.0f)
    {
        r = x;
        g = c;
        b = 0;
    }
    else if (h < 180.0f)
    {
        r = 0;
        g = c;
        b = x;
    }
    else if (h < 240.0f)
    {
        r = 0;
        g = x;
        b = c;
    }
    else if (h < 300.0f)
    {
        r = x;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = x;
    }

    return {static_cast<uint8_t>((r + m) * 255.0f),
            static_cast<uint8_t>((g + m) * 255.0f),
            static_cast<uint8_t>((b + m) * 255.0f),
            255};
}

ColorHSY ColorMath::rgb_to_hsy(Color rgb, const ColorLumaCoeffs& coeffs)
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float y = coeffs.r * r + coeffs.g * g + coeffs.b * b;

    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;

    float h = 0.0f;
    if (delta > 0.0f)
    {
        if (max == r)
            h = 60.0f * std::fmod((g - b) / delta, 6.0f);
        else if (max == g)
            h = 60.0f * ((b - r) / delta + 2.0f);
        else
            h = 60.0f * ((r - g) / delta + 4.0f);
    }
    if (h < 0.0f)
        h += 360.0f;

    float s = (y > 0.0f) ? 1.0f - (min / y) : 0.0f;
    if (max == min)
        s = 0.0f;

    return {h, s, y};
}

Color ColorMath::hsy_to_rgb(ColorHSY hsy, const ColorLumaCoeffs& coeffs)
{
    float h = hsy.h;
    float s = hsy.s;
    float y = hsy.y;

    float c = y * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = y - c;

    float r, g, b;
    if (h < 60.0f)
    {
        r = c;
        g = x;
        b = 0;
    }
    else if (h < 120.0f)
    {
        r = x;
        g = c;
        b = 0;
    }
    else if (h < 180.0f)
    {
        r = 0;
        g = c;
        b = x;
    }
    else if (h < 240.0f)
    {
        r = 0;
        g = x;
        b = c;
    }
    else if (h < 300.0f)
    {
        r = x;
        g = 0;
        b = c;
    }
    else
    {
        r = c;
        g = 0;
        b = x;
    }

    r = (r + m);
    g = (g + m);
    b = (b + m);

    float rg_diff = coeffs.r - coeffs.g;
    float rg_sum = coeffs.r + coeffs.g;
    float rb_diff = coeffs.r - coeffs.b;
    float gb_diff = coeffs.g - coeffs.b;
    float gb_sum = coeffs.g + coeffs.b;

    if (rg_sum != 0.0f && gb_sum != 0.0f && rb_diff != 0.0f)
    {
        float tmp = (y - coeffs.r * r - coeffs.b * b) / coeffs.g;
        g = tmp;
    }

    return {static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f),
            static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f),
            static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f),
            255};
}

float ColorMath::calculate_luma(Color rgb, const ColorLumaCoeffs& coeffs)
{
    return coeffs.r * (rgb.r / 255.0f) + coeffs.g * (rgb.g / 255.0f) + coeffs.b * (rgb.b / 255.0f);
}

Color ColorMath::apply_gamma(Color rgb, float gamma)
{
    float r = std::pow(rgb.r / 255.0f, 1.0f / gamma);
    float g = std::pow(rgb.g / 255.0f, 1.0f / gamma);
    float b = std::pow(rgb.b / 255.0f, 1.0f / gamma);

    return {
        static_cast<uint8_t>(r * 255.0f), static_cast<uint8_t>(g * 255.0f), static_cast<uint8_t>(b * 255.0f), rgb.a};
}

Color ColorMath::remove_gamma(Color rgb, float gamma)
{
    float r = std::pow(rgb.r / 255.0f, gamma);
    float g = std::pow(rgb.g / 255.0f, gamma);
    float b = std::pow(rgb.b / 255.0f, gamma);

    return {
        static_cast<uint8_t>(r * 255.0f), static_cast<uint8_t>(g * 255.0f), static_cast<uint8_t>(b * 255.0f), rgb.a};
}

Color ColorMath::darken(Color rgb, float amount)
{
    ColorHSV hsv = rgb_to_hsv(rgb);
    hsv.v = std::max(0.0f, hsv.v - amount);
    return hsv_to_rgb(hsv);
}

Color ColorMath::lighten(Color rgb, float amount)
{
    ColorHSV hsv = rgb_to_hsv(rgb);
    hsv.v = std::min(1.0f, hsv.v + amount);
    return hsv_to_rgb(hsv);
}

Color ColorMath::desaturate(Color rgb, float amount)
{
    ColorHSV hsv = rgb_to_hsv(rgb);
    hsv.s = std::max(0.0f, hsv.s - amount);
    return hsv_to_rgb(hsv);
}

Color ColorMath::saturate(Color rgb, float amount)
{
    ColorHSV hsv = rgb_to_hsv(rgb);
    hsv.s = std::min(1.0f, hsv.s + amount);
    return hsv_to_rgb(hsv);
}

Color ColorMath::shift_hue(Color rgb, float degrees)
{
    ColorHSV hsv = rgb_to_hsv(rgb);
    hsv.h = std::fmod(hsv.h + degrees + 360.0f, 360.0f);
    return hsv_to_rgb(hsv);
}

std::vector<Color> ColorMath::generate_shadow_row(Color base, int count, bool warm)
{
    std::vector<Color> shadows;
    ColorHSL hsl = rgb_to_hsl(base);

    for (int i = 0; i < count; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(count - 1);
        hsl.l = std::clamp(hsl.l + (warm ? 0.1f : -0.1f), 0.0f, 1.0f);
        shadows.push_back(hsl_to_rgb(hsl));
    }

    return shadows;
}

std::vector<Color> ColorMath::generate_harmony(Color base, HarmonyType type)
{
    std::vector<Color> harmony;

    if (type == HarmonyType::None)
    {
        harmony.push_back(base);
        return harmony;
    }

    harmony.push_back(base);

    switch (type)
    {
        case HarmonyType::Complementary:
            harmony.push_back(shift_hue(base, 180.0f));
            break;

        case HarmonyType::Triad:
            harmony.push_back(shift_hue(base, 120.0f));
            harmony.push_back(shift_hue(base, 240.0f));
            break;

        case HarmonyType::Analogous:
            harmony.push_back(shift_hue(base, 30.0f));
            harmony.push_back(shift_hue(base, -30.0f));
            break;

        case HarmonyType::SplitComplementary:
            harmony.push_back(shift_hue(base, 150.0f));
            harmony.push_back(shift_hue(base, 210.0f));
            break;

        default:
            break;
    }

    return harmony;
}

void ColorState::add_to_history(Color c)
{
    auto it = std::find(history.begin(), history.end(), c);
    if (it != history.end())
        history.erase(it);

    history.insert(history.begin(), c);

    if (history.size() > max_history)
        history.resize(max_history);
}

void ColorState::extract_from_pixels(const uint8_t* pixels, int width, int height)
{
    common_colors.clear();

    for (int i = 0; i < width * height; ++i)
    {
        Color c = {pixels[i * 4 + 0], pixels[i * 4 + 1], pixels[i * 4 + 2], pixels[i * 4 + 3]};

        if (c.a < 128)
            continue;

        bool found = false;
        for (const auto& existing : common_colors)
        {
            if (std::abs(static_cast<int>(c.r) - static_cast<int>(existing.r)) < 8 &&
                std::abs(static_cast<int>(c.g) - static_cast<int>(existing.g)) < 8 &&
                std::abs(static_cast<int>(c.b) - static_cast<int>(existing.b)) < 8)
            {
                found = true;
                break;
            }
        }

        if (!found)
            common_colors.push_back(c);

        if (common_colors.size() >= 10)
            break;
    }
}

}  // namespace convoy
