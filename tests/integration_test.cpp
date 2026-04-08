#include <gtest/gtest.h>
#include "modules/mod_architect/canvas.h"
#include "core/filesystem.h"
#include <filesystem>

using namespace convoy;
using namespace convoy::architect;

TEST(IntegrationTest, FullWorkflow) {
    std::string test_project = "/tmp/integration_test";
    
    if (std::filesystem::exists(test_project)) {
        std::filesystem::remove_all(test_project);
    }
    
    Canvas canvas(64, 64);
    Color red(255, 0, 0, 255);
    canvas.set_pixel(10, 10, red);
    
    EXPECT_TRUE(FileSystem::create_project_directory(test_project));
    EXPECT_TRUE(FileSystem::save_canvas(test_project, &canvas));
    
    Canvas loaded_canvas(64, 64);
    EXPECT_TRUE(FileSystem::load_canvas(test_project, &loaded_canvas));
    
    Color loaded_pixel = loaded_canvas.get_pixel(10, 10);
    EXPECT_EQ(loaded_pixel.r, red.r);
    EXPECT_EQ(loaded_pixel.g, red.g);
    EXPECT_EQ(loaded_pixel.b, red.b);
    EXPECT_EQ(loaded_pixel.a, red.a);
    
    std::filesystem::remove_all(test_project);
    EXPECT_FALSE(std::filesystem::exists(test_project));
}

TEST(IntegrationTest, CanvasWithLayers) {
    Canvas canvas(32, 32);
    EXPECT_EQ(canvas.get_layers().size(), 1);
    
    canvas.add_layer("Layer 2");
    canvas.add_layer("Layer 3");
    EXPECT_EQ(canvas.get_layers().size(), 3);
    
    Color blue(0, 0, 255, 255);
    canvas.set_active_layer(1);
    EXPECT_EQ(canvas.get_active_layer(), &canvas.get_layers()[1]);
    canvas.set_pixel(5, 5, blue);
    
    Color red(255, 0, 0, 255);
    canvas.set_active_layer(0);
    EXPECT_EQ(canvas.get_active_layer(), &canvas.get_layers()[0]);
    canvas.set_pixel(5, 5, red);
    
    std::vector<uint32_t> output;
    canvas.composite_to_texture(output);
    
    EXPECT_EQ(output.size(), 32 * 32);
    
    uint32_t composited_pixel = output[5 * 32 + 5];
    EXPECT_NE(composited_pixel, 0);
    
    uint8_t r = (composited_pixel >> 0) & 0xFF;
    uint8_t g = (composited_pixel >> 8) & 0xFF;
    uint8_t b = (composited_pixel >> 16) & 0xFF;
    uint8_t a = (composited_pixel >> 24) & 0xFF;
    
    EXPECT_TRUE((r > 0 || b > 0 || g > 0) && a > 0);
}
