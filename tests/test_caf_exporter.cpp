#include <gtest/gtest.h>

#include <fstream>

#include "core/export/caf_exporter.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;

TEST(CAFHeaderTest, DefaultConstructs)
{
    CAFHeader h;
    EXPECT_EQ(h.magic[0], 'C');
}

TEST(CAFMetadataTest, DefaultConstructs)
{
    CAFMetadata m;
    EXPECT_EQ(m.width, 0);
    EXPECT_EQ(m.height, 0);
}

TEST(CAFExporterTest, ExportCreatesFile)
{
    architect::Canvas canvas(64, 64);
    canvas.set_pixel(10, 10, Color{255, 0, 0, 255});

    std::vector<uint32_t> pixels;
    canvas.composite_to_texture(pixels);

    bool result = CAFExporter::export_to_file(
        "/tmp/test.caf", reinterpret_cast<const uint8_t*>(pixels.data()), 64, 64, nullptr, 0, 0);

    EXPECT_TRUE(result);

    std::ifstream f("/tmp/test.caf", std::ios::binary | std::ios::ate);
    ASSERT_TRUE(f.is_open());
    EXPECT_GT(f.tellg(), 0);
}

TEST(CAFExporterTest, RoundtripPixelData)
{
    std::vector<uint8_t> pixels(64 * 64 * 4);
    for (size_t i = 0; i < pixels.size(); i += 4)
    {
        pixels[i] = 255;
        pixels[i + 1] = 128;
        pixels[i + 2] = 64;
        pixels[i + 3] = 255;
    }

    bool ok = CAFExporter::export_to_file("/tmp/roundtrip.caf", pixels.data(), 64, 64, nullptr, 32, 32);
    ASSERT_TRUE(ok);

    std::ifstream f("/tmp/roundtrip.caf", std::ios::binary);
    CAFHeader h;
    f.read(reinterpret_cast<char*>(&h), sizeof(h));

    EXPECT_EQ(h.width, 64);
    EXPECT_EQ(h.height, 64);
    EXPECT_GT(h.data_offset, 0);
}
