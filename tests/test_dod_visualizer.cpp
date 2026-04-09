#include <gtest/gtest.h>
#include "modules/mod_architect/ui/dod_visualizer.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;
using namespace convoy::architect;

TEST(DODVisualizerTest, ComputeAlignment_AlignedCanvas) {
    // 8x4 canvas = 128 bytes = exactly 4 * 32 -> aligned
    Canvas c(8, 4);
    auto info = DODVisualizer::compute_alignment(&c);
    EXPECT_EQ(info.data_bytes, 128u);
    EXPECT_TRUE(info.is_aligned);
    EXPECT_EQ(info.waste_bytes, 0);
}

TEST(DODVisualizerTest, ComputeAlignment_UnalignedCanvas) {
    // 5x5 = 100 bytes, next multiple of 32 = 128
    Canvas c(5, 5);
    auto info = DODVisualizer::compute_alignment(&c);
    EXPECT_EQ(info.data_bytes, 100u);
    EXPECT_FALSE(info.is_aligned);
    EXPECT_EQ(info.aligned_bytes, 128u);
    EXPECT_EQ(info.waste_bytes, 28);
}

TEST(DODVisualizerTest, VRAMEstimateScalesWithLayers) {
    Canvas c(64, 64);
    size_t single = DODVisualizer::compute_vram_estimate(&c);
    c.add_layer("L2");
    size_t two = DODVisualizer::compute_vram_estimate(&c);
    EXPECT_EQ(two, single * 2);
}
