# CAF Exporter Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Implement binary CAF (Codex Asset File) exporter for pixel art assets with embedded collision metadata, enabling direct loading by Caffeine Engine.

**Architecture:** Write-only binary format with 32-byte aligned sections: Header → Metadata → Collision Map → Pixel Data. Exports from flattened Canvas RGBA buffer + collision layer bitmap.

**Tech Stack:** C++17, std::fstream for binary I/O, GoogleTest for unit tests

---

## Task 1: CAF Header and Metadata Structures

**Files:**
- Create: `src/core/export/caf_exporter.h`

**Step 1: Write the failing test**

Create `tests/test_caf_exporter.cpp`:

```cpp
#include <gtest/gtest.h>
#include "core/export/caf_exporter.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;

TEST(CAFHeaderTest, DefaultConstructs) {
    CAFHeader h;
    EXPECT_EQ(h.magic[0], 'C');
}

TEST(CAFMetadataTest, DefaultConstructs) {
    CAFMetadata m;
    EXPECT_EQ(m.width, 0);
    EXPECT_EQ(m.height, 0);
}
```

**Step 2: Run test to verify it fails**

Run: `cd build && make convoy_core && make convoy_tests && ./tests/convoy_tests --gtest_filter="CAFHeaderTest*"`

Expected: FAIL - files don't exist yet

**Step 3: Write the implementation**

Create `src/core/export/caf_exporter.h`:

```cpp
#pragma once
#include <cstdint>
#include <string>

namespace convoy {

#pragma pack(push, 1)
struct CAFHeader {
    char     magic[4] = {'C', 'A', 'F', '1'};
    uint16_t version = 1;
    uint16_t type = 0;  // 0=Sprite
    uint32_t file_size = 0;
    uint32_t data_offset = 0;
    uint32_t colmap_offset = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t  reserved[8] = {0};
};

struct CAFMetadata {
    int16_t  pivot_x = 0;
    int16_t  pivot_y = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    uint32_t colmap_size = 0;
    uint32_t pixel_data_size = 0;
    uint32_t frame_count = 0;
    uint32_t fps = 0;
    uint8_t  reserved[8] = {0};
};
#pragma pack(pop)

enum class CAFType : uint16_t {
    Sprite = 0,
    Tilemap = 1,
    Animation = 2
};

enum class CollisionType : uint8_t {
    Empty = 0,
    Solid = 1,
    Trigger = 2,
    Hurtbox = 3
};

class CAFExporter {
public:
    static bool export_to_file(const std::string& path,
                               const uint8_t* pixels,
                               uint32_t width, uint32_t height,
                               const uint8_t* colmap,
                               int16_t pivot_x, int16_t pivot_y);
};

} // namespace convoy
```

**Step 4: Run test to verify it passes**

Run: `cd build && make convoy_tests && ./tests/convoy_tests --gtest_filter="CAFHeaderTest*"`

Expected: PASS

**Step 5: Commit**

```bash
git add src/core/export/caf_exporter.h tests/test_caf_exporter.cpp
git commit -m "feat: add CAF header and metadata structures"
```

---

## Task 2: CAF Exporter Implementation

**Files:**
- Create: `src/core/export/caf_exporter.cpp`
- Modify: `CMakeLists.txt` (add export source to convoy_core)

**Step 1: Write the failing test**

Add to `tests/test_caf_exporter.cpp`:

```cpp
TEST(CAFExporterTest, ExportCreatesFile) {
    architect::Canvas canvas(64, 64);
    canvas.set_pixel(10, 10, Color{255, 0, 0, 255});
    
    std::vector<uint32_t> pixels;
    canvas.composite_to_texture(pixels);
    
    bool result = CAFExporter::export_to_file(
        "/tmp/test.caf",
        reinterpret_cast<const uint8_t*>(pixels.data()),
        64, 64, nullptr, 0, 0);
    
    EXPECT_TRUE(result);
    
    // Check file exists and has expected size
    std::ifstream f("/tmp/test.caf", std::ios::binary | std::ios::ate);
    ASSERT_TRUE(f.is_open());
    EXPECT_GT(f.tellg(), 0);
}
```

**Step 2: Run test to verify it fails**

Run: `cd build && make convoy_tests && ./tests/convoy_tests --gtest_filter="CAFExporterTest*"`

Expected: FAIL - caf_exporter.cpp doesn't exist

**Step 3: Write the implementation**

Create `src/core/export/caf_exporter.cpp`:

```cpp
#include "core/export/caf_exporter.h"
#include "core/logger.h"
#include <fstream>
#include <cstring>

namespace convoy {

namespace {

// Pad size to 32-byte boundary
constexpr size_t pad32(size_t size) {
    return (size + 31) & ~31u;
}

} // anonymous namespace

bool CAFExporter::export_to_file(const std::string& path,
                                  const uint8_t* pixels,
                                  uint32_t width, uint32_t height,
                                  const uint8_t* colmap,
                                  int16_t pivot_x, int16_t pivot_y) {
    // Calculate sizes
    const size_t pixel_data_size = width * height * 4;  // RGBA8
    const size_t colmap_bytes = (width * height + 3) / 4;  // 2 bits per pixel
    const size_t colmap_padded = pad32(colmap_bytes);
    
    const size_t header_size = sizeof(CAFHeader);
    const size_t metadata_size = sizeof(CAFMetadata);
    const size_t colmap_offset = pad32(header_size + metadata_size);
    const size_t data_offset = colmap_offset + colmap_padded;
    const size_t total_size = data_offset + pad32(pixel_data_size);
    
    // Create file
    std::ofstream f(path, std::ios::binary);
    if (!f.is_open()) {
        Logger::error("Failed to create CAF file: {}", path);
        return false;
    }
    
    // Write header
    CAFHeader header;
    std::memcpy(header.magic, "CAF1", 4);
    header.version = 1;
    header.type = 0;  // Sprite
    header.file_size = static_cast<uint32_t>(total_size);
    header.data_offset = static_cast<uint32_t>(data_offset);
    header.colmap_offset = static_cast<uint32_t>(colmap_offset);
    header.width = width;
    header.height = height;
    f.write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    // Write metadata
    CAFMetadata metadata;
    metadata.pivot_x = pivot_x;
    metadata.pivot_y = pivot_y;
    metadata.width = static_cast<uint16_t>(width);
    metadata.height = static_cast<uint16_t>(height);
    metadata.colmap_size = static_cast<uint32_t>(colmap_bytes);
    metadata.pixel_data_size = static_cast<uint32_t>(pixel_data_size);
    f.write(reinterpret_cast<const char*>(&metadata), sizeof(metadata));
    
    // Pad to 32-byte boundary
    size_t written = header_size + metadata_size;
    if (colmap_offset > written) {
        f.seekp(colmap_offset - written, std::ios::cur);
    }
    
    // Write collision map
    if (colmap && colmap_bytes > 0) {
        f.write(reinterpret_cast<const char*>(colmap), colmap_bytes);
    }
    f.seekp(colmap_offset + colmap_padded - colmap_offset, std::ios::cur);
    
    // Write pixel data
    f.write(reinterpret_cast<const char*>(pixels), pixel_data_size);
    
    f.close();
    Logger::info("Exported CAF: {} ({}x{})", path, width, height);
    return true;
}

} // namespace convoy
```

**Step 4: Run test to verify it passes**

Run: `cd build && make convoy_tests && ./tests/convoy_tests --gtest_filter="CAFExporterTest*"`

Expected: PASS

**Step 5: Commit**

```bash
git add src/core/export/caf_exporter.cpp src/core/export/caf_exporter.h tests/test_caf_exporter.cpp
git commit -m "feat: add CAF binary exporter implementation"
```

---

## Task 3: Integration with Architect UI

**Files:**
- Modify: `src/core/window_manager.cpp` (add export command)
- Modify: `src/core/input/input_handler.cpp` (bind Ctrl+D)

**Step 1: Write the test (integration check)**

Add to `tests/test_caf_exporter.cpp`:

```cpp
TEST(CAFExporterTest, RoundtripPixelData) {
    std::vector<uint8_t> pixels(64 * 64 * 4);
    for (size_t i = 0; i < pixels.size(); i += 4) {
        pixels[i] = 255;     // R
        pixels[i+1] = 128;   // G
        pixels[i+2] = 64;    // B
        pixels[i+3] = 255;   // A
    }
    
    // Export with no collision map
    bool ok = CAFExporter::export_to_file(
        "/tmp/roundtrip.caf",
        pixels.data(), 64, 64, nullptr, 32, 32);
    
    ASSERT_TRUE(ok);
    
    // Read back and verify header
    std::ifstream f("/tmp/roundtrip.caf", std::ios::binary);
    CAFHeader h;
    f.read(reinterpret_cast<char*>(&h), sizeof(h));
    
    EXPECT_EQ(h.width, 64);
    EXPECT_EQ(h.height, 64);
    EXPECT_EQ(h.data_offset > 0, true);
}
```

**Step 2: Run test to verify it fails**

Run: `cd build && make convoy_tests && ./tests/convoy_tests --gtest_filter="CAFExporterTest.Roundtrip*"`

Expected: FAIL if not implemented

**Step 3: Implement the missing pieces**

Add export method to WindowManager and bind the shortcut.

**Step 4: Run test to verify it passes**

Run: `cd build && make convoy_tests && ./tests/convoy_tests --gtest_filter="CAFExporterTest.Roundtrip*"`

Expected: PASS

**Step 5: Commit**

```bash
git add src/core/window_manager.cpp src/core/input/input_handler.cpp
git commit -m "feat: integrate CAF export with Ctrl+D shortcut"
```

---

## Verification

Run all tests:
```bash
cd build
ctest --output-on-failure
```

Build application:
```bash
cmake --build . --config Release -j$(nproc)
./bin/convoy
```

---

**Plan complete and saved to `docs/plans/2026-04-09-caf-exporter-implementation.md`. Two execution options:**

**1. Subagent-Driven (this session)** - I dispatch fresh subagent per task, review between tasks, fast iteration

**2. Parallel Session (separate)** - Open new session with executing-plans, batch execution with checkpoints

Which approach?