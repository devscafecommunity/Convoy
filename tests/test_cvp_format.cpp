#include <gtest/gtest.h>
#include "core/cvp_format.h"
#include "core/filesystem.h"
#include "modules/mod_architect/canvas.h"
#include <filesystem>

using namespace convoy;
using namespace convoy::architect;

TEST(CVPFormatTest, SaveAndLoadCanvas) {
    std::string test_file = "/tmp/test_canvas.cvp";
    
    Canvas original(32, 32);
    Color red(255, 0, 0, 255);
    original.set_pixel(10, 10, red);
    original.set_pixel(20, 20, red);
    
    EXPECT_TRUE(CVPFormat::save_to_file(test_file, &original));
    
    Canvas loaded(32, 32);
    EXPECT_TRUE(CVPFormat::load_from_file(test_file, &loaded));
    
    Color pixel = loaded.get_pixel(10, 10);
    EXPECT_EQ(pixel.r, 255);
    EXPECT_EQ(pixel.g, 0);
    EXPECT_EQ(pixel.b, 0);
    EXPECT_EQ(pixel.a, 255);
    
    pixel = loaded.get_pixel(20, 20);
    EXPECT_EQ(pixel.r, 255);
    
    std::filesystem::remove(test_file);
}

TEST(FileSystemTest, ProjectDirectory) {
    std::string test_project = "/tmp/test_project";
    
    EXPECT_TRUE(FileSystem::create_project_directory(test_project));
    EXPECT_TRUE(FileSystem::project_exists(test_project));
    
    Canvas original(64, 64);
    Color blue(0, 0, 255, 255);
    original.set_pixel(5, 5, blue);
    
    EXPECT_TRUE(FileSystem::save_canvas(test_project, &original));
    
    Canvas loaded(64, 64);
    EXPECT_TRUE(FileSystem::load_canvas(test_project, &loaded));
    
    Color pixel = loaded.get_pixel(5, 5);
    EXPECT_EQ(pixel.b, 255);
    EXPECT_EQ(pixel.r, 0);
    EXPECT_EQ(pixel.g, 0);
    
    std::filesystem::remove_all(test_project);
}
