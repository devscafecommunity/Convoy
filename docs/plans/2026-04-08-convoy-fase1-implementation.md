# Convoy Fase 1 Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Implement the core shell and Architect module for a pixel art editor with canvas, layers, drawing tools, and project save/load in binary .cvp format.

**Architecture:** 
- Modular Host/Plugin system where Core manages windows, events, and filesystem IO
- Modules load dynamically and communicate through an event bus
- Architect module provides drawing canvas with layer support, pixel-level tools, and metadata editing
- All changes tracked through undo/redo command manager
- Binary .cvp format with fixed structure for fast serialization

**Tech Stack:**
- **Language:** C++17 with CMake build system
- **UI:** Dear ImGui 1.89+ with docking support
- **Graphics:** OpenGL via GLAD + GLFW 3.4
- **Images:** stb_image + stb_image_write (header-only)
- **Math:** Custom minimal math library (Vec2, Rect, Color structs)
- **Serialization:** Custom binary format with endian safety

---

## Task 1: Project Structure & CMake Setup

**Files:**
- Create: `CMakeLists.txt`
- Create: `src/cmake/FindGLAD.cmake`
- Create: `src/cmake/FindImGui.cmake`
- Create: `src/cmake/FindGLFW.cmake`
- Create: `src/convoy/main.cpp`
- Create: `.clang-format`

**Step 1: Write CMakeLists.txt with all dependencies**

```cmake
cmake_minimum_required(VERSION 3.20)
project(Convoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake")

# Dependencies
find_package(GLFW3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLAD REQUIRED)
find_package(ImGui REQUIRED)

# Convoy Core Library
add_library(convoy_core
  src/core/window_manager.cpp
  src/core/workspace_system.cpp
  src/core/filesystem.cpp
  src/core/command_manager.cpp
  src/core/logger.cpp
  src/core/event_bus.cpp
  src/shared/math.cpp
  src/shared/types.cpp
)

target_include_directories(convoy_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(convoy_core
  GLFW::GLFW
  OpenGL::OpenGL
  GLAD::GLAD
  ImGui::ImGui
)

# Main Executable
add_executable(convoy
  src/convoy/main.cpp
)

target_link_libraries(convoy PRIVATE convoy_core)

# Compiler flags
if(MSVC)
  target_compile_options(convoy_core PRIVATE /W4 /WX)
  target_compile_options(convoy PRIVATE /W4 /WX)
else()
  target_compile_options(convoy_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_options(convoy PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Testing
enable_testing()
add_subdirectory(tests)
```

**Step 2: Create minimal find modules**

Create `src/cmake/FindGLAD.cmake`:
```cmake
find_package(OpenGL REQUIRED)

add_library(GLAD INTERFACE)
target_include_directories(GLAD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)
target_sources(GLAD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/glad.c)

add_library(GLAD::GLAD ALIAS GLAD)
```

Create `src/cmake/FindImGui.cmake`:
```cmake
add_library(ImGui INTERFACE)
target_include_directories(ImGui INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
target_sources(ImGui INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp
)
target_link_libraries(ImGui INTERFACE OpenGL::OpenGL GLFW::GLFW)

add_library(ImGui::ImGui ALIAS ImGui)
```

Create `src/cmake/FindGLFW.cmake`:
```cmake
find_package(GLFW3 CONFIG QUIET)
if(NOT GLFW3_FOUND)
  find_package(GLFW3 3.4 REQUIRED)
endif()
```

**Step 3: Create .clang-format**

```yaml
---
Language: Cpp
BasedOnStyle: Google
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 120
DerivePointerAlignment: true
PointerAlignment: Left
AlignOperands: AlignAfterOperator
AllowShortFunctionsOnASingleLine: Inline
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
BreakBeforeBraces: Allman
BinPackArguments: false
BinPackParameters: false
```

**Step 4: Create main.cpp entry point**

```cpp
// src/convoy/main.cpp
#include <iostream>
#include "core/window_manager.h"

int main(int argc, char* argv[]) {
    try {
        convoy::WindowManager window_manager;
        window_manager.initialize("Convoy - The Codex Art Station", 1600, 900);
        window_manager.run_loop();
        window_manager.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

**Step 5: Verify structure**

Run:
```bash
cd .worktrees/convoy-fase1
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

Expected: Build succeeds with no errors

**Step 6: Commit**

```bash
git add CMakeLists.txt src/cmake/ src/convoy/main.cpp .clang-format
git commit -m "feat: project structure and CMake build system"
```

---

## Task 2: Core Math Library & Type Definitions

**Files:**
- Create: `src/shared/math.h`
- Create: `src/shared/math.cpp`
- Create: `src/shared/types.h`
- Create: `src/shared/color.h`
- Create: `tests/test_math.cpp`

**Step 1: Write math header**

```cpp
// src/shared/math.h
#pragma once
#include <cmath>
#include <algorithm>

namespace convoy {

struct Vec2 {
    float x, y;
    
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    
    float length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const {
        float len = length();
        return len > 0.0f ? *this * (1.0f / len) : Vec2(0, 0);
    }
};

struct Rect {
    float x, y, w, h;
    
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    
    bool contains(const Vec2& point) const {
        return point.x >= x && point.x < x + w && point.y >= y && point.y < y + h;
    }
    
    Rect intersect(const Rect& other) const {
        float x1 = std::max(x, other.x);
        float y1 = std::max(y, other.y);
        float x2 = std::min(x + w, other.x + other.w);
        float y2 = std::min(y + h, other.y + other.h);
        return Rect(x1, y1, std::max(0.0f, x2 - x1), std::max(0.0f, y2 - y1));
    }
};

} // namespace convoy
```

**Step 2: Write types header**

```cpp
// src/shared/types.h
#pragma once
#include "math.h"
#include "color.h"
#include <string>
#include <vector>
#include <cstdint>

namespace convoy {

struct Layer {
    std::string name;
    std::vector<uint32_t> pixels;
    uint32_t width;
    uint32_t height;
    bool visible;
    bool locked;
    float opacity;
    
    Layer(const std::string& name, uint32_t w, uint32_h)
        : name(name), width(w), height(h), visible(true), locked(false), opacity(1.0f) {
        pixels.resize(w * h, 0);
    }
};

struct Canvas {
    uint32_t width, height;
    std::vector<Layer> layers;
    int active_layer_index;
    
    Canvas(uint32_t w, uint32_t h) : width(w), height(h), active_layer_index(0) {
        layers.emplace_back("Layer 1", w, h);
    }
    
    Layer* get_active_layer() {
        if (active_layer_index >= 0 && active_layer_index < (int)layers.size()) {
            return &layers[active_layer_index];
        }
        return nullptr;
    }
};

struct SpriteMetadata {
    Vec2 pivot;
    Rect hitbox;
    std::string sprite_name;
};

enum class ToolType {
    Pencil,
    Eraser,
    Bucket,
    ColorPicker,
    Move,
};

} // namespace convoy
```

**Step 3: Write color header**

```cpp
// src/shared/color.h
#pragma once
#include <cstdint>

namespace convoy {

struct Color {
    uint8_t r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
    
    uint32_t to_rgba() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }
    
    uint32_t to_abgr() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }
    
    static Color from_rgba(uint32_t rgba) {
        return Color(
            (rgba >> 24) & 0xFF,
            (rgba >> 16) & 0xFF,
            (rgba >> 8) & 0xFF,
            rgba & 0xFF
        );
    }
};

} // namespace convoy
```

**Step 4: Write tests**

```cpp
// tests/test_math.cpp
#include <gtest/gtest.h>
#include "shared/math.h"
#include "shared/color.h"

using namespace convoy;

TEST(MathTest, Vec2Operations) {
    Vec2 a(3, 4);
    EXPECT_FLOAT_EQ(a.length(), 5.0f);
    
    Vec2 b = a + Vec2(1, 1);
    EXPECT_EQ(b.x, 4.0f);
    EXPECT_EQ(b.y, 5.0f);
}

TEST(MathTest, RectContains) {
    Rect r(0, 0, 10, 10);
    EXPECT_TRUE(r.contains(Vec2(5, 5)));
    EXPECT_FALSE(r.contains(Vec2(10, 10)));
    EXPECT_FALSE(r.contains(Vec2(-1, 5)));
}

TEST(ColorTest, Conversion) {
    Color c(255, 128, 64, 200);
    uint32_t rgba = c.to_rgba();
    Color c2 = Color::from_rgba(rgba);
    EXPECT_EQ(c2.r, 255);
    EXPECT_EQ(c2.g, 128);
    EXPECT_EQ(c2.b, 64);
    EXPECT_EQ(c2.a, 200);
}
```

**Step 5: Add Google Test to CMakeLists.txt**

Add to main CMakeLists.txt:
```cmake
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# In tests/CMakeLists.txt
enable_testing()
add_executable(convoy_tests
  test_math.cpp
)
target_link_libraries(convoy_tests PRIVATE convoy_core gtest_main)
add_test(NAME ConvoyTests COMMAND convoy_tests)
```

**Step 6: Run tests**

Run:
```bash
cd build
ctest --verbose
```

Expected: All tests pass

**Step 7: Commit**

```bash
git add src/shared/math.h src/shared/color.h src/shared/types.h tests/test_math.cpp
git commit -m "feat: core math library and type definitions"
```

---

## Task 3: Window Manager & Event Bus

**Files:**
- Create: `src/core/window_manager.h`
- Create: `src/core/window_manager.cpp`
- Create: `src/core/event_bus.h`
- Create: `src/core/event_bus.cpp`
- Create: `tests/test_window_manager.cpp`

**Step 1: Write event bus header**

```cpp
// src/core/event_bus.h
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace convoy {

struct Event {
    virtual ~Event() = default;
    virtual std::string type() const = 0;
};

struct CanvasModifiedEvent : Event {
    int layer_id;
    std::string type() const override { return "CanvasModified"; }
};

struct ToolSelectedEvent : Event {
    int tool_id;
    std::string type() const override { return "ToolSelected"; }
};

class EventBus {
public:
    using EventCallback = std::function<void(const Event&)>;
    
    void subscribe(const std::string& event_type, EventCallback callback);
    void publish(const Event& event);
    void clear_subscriptions();
    
private:
    std::unordered_map<std::string, std::vector<EventCallback>> subscribers_;
};

} // namespace convoy
```

**Step 2: Write event bus implementation**

```cpp
// src/core/event_bus.cpp
#include "event_bus.h"

namespace convoy {

void EventBus::subscribe(const std::string& event_type, EventCallback callback) {
    subscribers_[event_type].push_back(callback);
}

void EventBus::publish(const Event& event) {
    auto it = subscribers_.find(event.type());
    if (it != subscribers_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
}

void EventBus::clear_subscriptions() {
    subscribers_.clear();
}

} // namespace convoy
```

**Step 3: Write window manager header**

```cpp
// src/core/window_manager.h
#pragma once
#include "event_bus.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace convoy {

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    
    void initialize(const std::string& title, int width, int height);
    void run_loop();
    void shutdown();
    
    GLFWwindow* get_window() const { return window_; }
    EventBus& get_event_bus() { return event_bus_; }
    
    bool should_close() const;
    
private:
    GLFWwindow* window_;
    EventBus event_bus_;
    bool initialized_;
    
    static void glfw_error_callback(int error, const char* description);
};

} // namespace convoy
```

**Step 4: Write window manager implementation**

```cpp
// src/core/window_manager.cpp
#include "window_manager.h"
#include "logger.h"
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdexcept>

namespace convoy {

WindowManager::WindowManager() : window_(nullptr), initialized_(false) {
    glfwSetErrorCallback(glfw_error_callback);
}

WindowManager::~WindowManager() {
    if (initialized_) {
        shutdown();
    }
}

void WindowManager::glfw_error_callback(int error, const char* description) {
    Logger::error("GLFW Error {}: {}", error, description);
}

void WindowManager::initialize(const std::string& title, int width, int height) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to load OpenGL function pointers");
    }
    
    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    initialized_ = true;
    Logger::info("Window initialized: {}x{}", width, height);
}

bool WindowManager::should_close() const {
    return window_ && glfwWindowShouldClose(window_);
}

void WindowManager::run_loop() {
    while (!should_close()) {
        glfwPollEvents();
        
        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Draw ImGui demo
        ImGui::ShowDemoWindow();
        
        ImGui::Render();
        
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        glfwSwapBuffers(window_);
    }
}

void WindowManager::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
    
    initialized_ = false;
    Logger::info("Window manager shutdown");
}

} // namespace convoy
```

**Step 5: Write logger header (minimal)**

```cpp
// src/core/logger.h
#pragma once
#include <string>
#include <iostream>

namespace convoy {

class Logger {
public:
    template<typename... Args>
    static void info(const std::string& msg, Args... args) {
        std::cout << "[INFO] " << format(msg, args...) << std::endl;
    }
    
    template<typename... Args>
    static void error(const std::string& msg, Args... args) {
        std::cerr << "[ERROR] " << format(msg, args...) << std::endl;
    }
    
private:
    template<typename T>
    static std::string format(const std::string& msg, T arg) {
        return msg;
    }
};

} // namespace convoy
```

**Step 6: Run and test**

Run:
```bash
cd build
cmake --build . --config Debug
./convoy
```

Expected: Window opens with ImGui demo visible

**Step 7: Commit**

```bash
git add src/core/window_manager.h src/core/window_manager.cpp src/core/event_bus.h src/core/event_bus.cpp src/core/logger.h
git commit -m "feat: window manager and event bus"
```

---

## Task 4: Canvas System & Drawing Tools

**Files:**
- Create: `src/modules/mod_architect/canvas.h`
- Create: `src/modules/mod_architect/canvas.cpp`
- Create: `src/modules/mod_architect/tools/tool_base.h`
- Create: `src/modules/mod_architect/tools/pencil_tool.h`
- Create: `src/modules/mod_architect/tools/pencil_tool.cpp`
- Create: `src/modules/mod_architect/tools/eraser_tool.h`
- Create: `src/modules/mod_architect/tools/eraser_tool.cpp`
- Create: `src/modules/mod_architect/tools/bucket_tool.h`
- Create: `src/modules/mod_architect/tools/bucket_tool.cpp`
- Create: `tests/test_canvas.cpp`

**Step 1: Write canvas header**

```cpp
// src/modules/mod_architect/canvas.h
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
```

**Step 2: Write canvas implementation**

```cpp
// src/modules/mod_architect/canvas.cpp
#include "canvas.h"
#include <algorithm>
#include <stdexcept>

namespace convoy {
namespace architect {

Canvas::Canvas(uint32_t width, uint32_t height) 
    : width_(width), height_(height), active_layer_(0) {
    layers_.emplace_back("Background", width, height);
}

bool Canvas::is_in_bounds(uint32_t x, uint32_t y) const {
    return x < width_ && y < height_;
}

void Canvas::set_pixel(uint32_t x, uint32_t y, Color color) {
    if (!is_in_bounds(x, y)) return;
    
    Layer* layer = get_active_layer();
    if (layer) {
        layer->pixels[y * width_ + x] = color.to_abgr();
    }
}

Color Canvas::get_pixel(uint32_t x, uint32_t y) const {
    if (!is_in_bounds(x, y)) return Color(0, 0, 0, 0);
    
    const Layer* layer = get_active_layer();
    if (layer) {
        return Color::from_rgba(layer->pixels[y * width_ + x]);
    }
    return Color(0, 0, 0, 0);
}

void Canvas::add_layer(const std::string& name) {
    if (layers_.size() >= 32) {
        throw std::runtime_error("Maximum 32 layers allowed");
    }
    layers_.emplace_back(name, width_, height_);
}

void Canvas::remove_layer(int index) {
    if (index < 0 || index >= (int)layers_.size() || layers_.size() == 1) {
        return;
    }
    layers_.erase(layers_.begin() + index);
    if (active_layer_ >= (int)layers_.size()) {
        active_layer_ = layers_.size() - 1;
    }
}

void Canvas::set_active_layer(int index) {
    if (index >= 0 && index < (int)layers_.size()) {
        active_layer_ = index;
    }
}

Layer* Canvas::get_active_layer() {
    if (active_layer_ >= 0 && active_layer_ < (int)layers_.size()) {
        return &layers_[active_layer_];
    }
    return nullptr;
}

const Layer* Canvas::get_active_layer() const {
    if (active_layer_ >= 0 && active_layer_ < (int)layers_.size()) {
        return &layers_[active_layer_];
    }
    return nullptr;
}

void Canvas::clear_active_layer() {
    Layer* layer = get_active_layer();
    if (layer) {
        std::fill(layer->pixels.begin(), layer->pixels.end(), 0);
    }
}

void Canvas::composite_to_texture(std::vector<uint32_t>& output) {
    if (output.size() != width_ * height_) {
        output.resize(width_ * height_);
    }
    
    std::fill(output.begin(), output.end(), 0xFF1F1F1F);
    
    for (const auto& layer : layers_) {
        if (!layer.visible) continue;
        
        float opacity = layer.opacity;
        for (uint32_t i = 0; i < layer.pixels.size(); ++i) {
            uint32_t src = layer.pixels[i];
            uint8_t src_a = (src >> 24) & 0xFF;
            uint8_t src_r = (src >> 16) & 0xFF;
            uint8_t src_g = (src >> 8) & 0xFF;
            uint8_t src_b = src & 0xFF;
            
            src_a = static_cast<uint8_t>(src_a * opacity);
            
            if (src_a > 0) {
                uint32_t dst = output[i];
                uint8_t dst_a = (dst >> 24) & 0xFF;
                uint8_t dst_r = (dst >> 16) & 0xFF;
                uint8_t dst_g = (dst >> 8) & 0xFF;
                uint8_t dst_b = dst & 0xFF;
                
                float alpha = src_a / 255.0f;
                dst_r = static_cast<uint8_t>(dst_r * (1 - alpha) + src_r * alpha);
                dst_g = static_cast<uint8_t>(dst_g * (1 - alpha) + src_g * alpha);
                dst_b = static_cast<uint8_t>(dst_b * (1 - alpha) + src_b * alpha);
                dst_a = static_cast<uint8_t>(std::max(dst_a, src_a));
                
                output[i] = (dst_a << 24) | (dst_r << 16) | (dst_g << 8) | dst_b;
            }
        }
    }
}

} // namespace architect
} // namespace convoy
```

**Step 3: Write tool base class**

```cpp
// src/modules/mod_architect/tools/tool_base.h
#pragma once
#include "shared/types.h"
#include "modules/mod_architect/canvas.h"

namespace convoy {
namespace architect {

class Tool {
public:
    virtual ~Tool() = default;
    
    virtual void on_mouse_down(Canvas* canvas, int x, int y) = 0;
    virtual void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) = 0;
    virtual void on_mouse_up(Canvas* canvas, int x, int y) = 0;
    
    void set_foreground_color(Color color) { foreground_ = color; }
    void set_background_color(Color color) { background_ = color; }
    
protected:
    Color foreground_{255, 255, 255, 255};
    Color background_{0, 0, 0, 0};
};

} // namespace architect
} // namespace convoy
```

**Step 4: Write pencil tool**

```cpp
// src/modules/mod_architect/tools/pencil_tool.h
#pragma once
#include "tool_base.h"

namespace convoy {
namespace architect {

class PencilTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
};

} // namespace architect
} // namespace convoy
```

```cpp
// src/modules/mod_architect/tools/pencil_tool.cpp
#include "pencil_tool.h"
#include <cmath>
#include <algorithm>

namespace convoy {
namespace architect {

void PencilTool::on_mouse_down(Canvas* canvas, int x, int y) {
    canvas->set_pixel(x, y, foreground_);
}

void PencilTool::on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) {
    int dx = std::abs(to_x - from_x);
    int dy = std::abs(to_y - from_y);
    int steps = std::max(dx, dy) + 1;
    
    for (int i = 0; i < steps; ++i) {
        float t = steps > 1 ? (float)i / (steps - 1) : 0;
        int x = from_x + (to_x - from_x) * t;
        int y = from_y + (to_y - from_y) * t;
        canvas->set_pixel(x, y, foreground_);
    }
}

void PencilTool::on_mouse_up(Canvas* canvas, int x, int y) {
    // No action needed
}

} // namespace architect
} // namespace convoy
```

**Step 5: Write eraser tool**

```cpp
// src/modules/mod_architect/tools/eraser_tool.h
#pragma once
#include "tool_base.h"

namespace convoy {
namespace architect {

class EraserTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
};

} // namespace architect
} // namespace convoy
```

```cpp
// src/modules/mod_architect/tools/eraser_tool.cpp
#include "eraser_tool.h"

namespace convoy {
namespace architect {

void EraserTool::on_mouse_down(Canvas* canvas, int x, int y) {
    canvas->set_pixel(x, y, Color(0, 0, 0, 0));
}

void EraserTool::on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) {
    Color transparent(0, 0, 0, 0);
    int dx = std::abs(to_x - from_x);
    int dy = std::abs(to_y - from_y);
    int steps = std::max(dx, dy) + 1;
    
    for (int i = 0; i < steps; ++i) {
        float t = steps > 1 ? (float)i / (steps - 1) : 0;
        int x = from_x + (to_x - from_x) * t;
        int y = from_y + (to_y - from_y) * t;
        canvas->set_pixel(x, y, transparent);
    }
}

void EraserTool::on_mouse_up(Canvas* canvas, int x, int y) {
    // No action needed
}

} // namespace architect
} // namespace convoy
```

**Step 6: Write bucket tool**

```cpp
// src/modules/mod_architect/tools/bucket_tool.h
#pragma once
#include "tool_base.h"
#include <queue>

namespace convoy {
namespace architect {

class BucketTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
    
    void set_tolerance(int tolerance) { tolerance_ = tolerance; }
    
private:
    int tolerance_ = 10;
    
    bool colors_similar(Color c1, Color c2);
    void flood_fill(Canvas* canvas, int x, int y, Color target, Color fill);
};

} // namespace architect
} // namespace convoy
```

```cpp
// src/modules/mod_architect/tools/bucket_tool.cpp
#include "bucket_tool.h"

namespace convoy {
namespace architect {

bool BucketTool::colors_similar(Color c1, Color c2) {
    int dr = c1.r - c2.r;
    int dg = c1.g - c2.g;
    int db = c1.b - c2.b;
    int da = c1.a - c2.a;
    return (dr*dr + dg*dg + db*db + da*da) <= (tolerance_ * tolerance_);
}

void BucketTool::flood_fill(Canvas* canvas, int x, int y, Color target, Color fill) {
    std::queue<std::pair<int, int>> q;
    q.push({x, y});
    
    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();
        
        if (cx < 0 || cx >= (int)canvas->width() || cy < 0 || cy >= (int)canvas->height()) {
            continue;
        }
        
        Color current = canvas->get_pixel(cx, cy);
        if (!colors_similar(current, target)) {
            continue;
        }
        
        canvas->set_pixel(cx, cy, fill);
        
        q.push({cx + 1, cy});
        q.push({cx - 1, cy});
        q.push({cx, cy + 1});
        q.push({cx, cy - 1});
    }
}

void BucketTool::on_mouse_down(Canvas* canvas, int x, int y) {
    Color target = canvas->get_pixel(x, y);
    flood_fill(canvas, x, y, target, foreground_);
}

void BucketTool::on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) {
    // Bucket fill only on click
}

void BucketTool::on_mouse_up(Canvas* canvas, int x, int y) {
    // No action needed
}

} // namespace architect
} // namespace convoy
```

**Step 7: Write canvas tests**

```cpp
// tests/test_canvas.cpp
#include <gtest/gtest.h>
#include "modules/mod_architect/canvas.h"
#include "modules/mod_architect/tools/pencil_tool.h"

using namespace convoy;
using namespace convoy::architect;

TEST(CanvasTest, SetGetPixel) {
    Canvas canvas(64, 64);
    Color white(255, 255, 255, 255);
    
    canvas.set_pixel(10, 10, white);
    Color got = canvas.get_pixel(10, 10);
    
    EXPECT_EQ(got.r, 255);
    EXPECT_EQ(got.g, 255);
    EXPECT_EQ(got.b, 255);
    EXPECT_EQ(got.a, 255);
}

TEST(CanvasTest, LayerManagement) {
    Canvas canvas(64, 64);
    EXPECT_EQ(canvas.get_layers().size(), 1);
    
    canvas.add_layer("Layer 2");
    EXPECT_EQ(canvas.get_layers().size(), 2);
    
    canvas.set_active_layer(1);
    EXPECT_EQ(canvas.get_active_layer(), &canvas.get_layers()[1]);
}

TEST(CanvasTest, PencilToolDrawsLine) {
    Canvas canvas(64, 64);
    PencilTool pencil;
    pencil.set_foreground_color(Color(255, 0, 0, 255));
    
    pencil.on_mouse_drag(&canvas, 0, 0, 10, 0);
    
    for (int x = 0; x <= 10; ++x) {
        Color pixel = canvas.get_pixel(x, 0);
        EXPECT_EQ(pixel.r, 255);
    }
}
```

**Step 8: Update CMakeLists.txt for Architect module**

Add to main CMakeLists.txt:
```cmake
add_library(mod_architect
  src/modules/mod_architect/canvas.cpp
  src/modules/mod_architect/tools/pencil_tool.cpp
  src/modules/mod_architect/tools/eraser_tool.cpp
  src/modules/mod_architect/tools/bucket_tool.cpp
)
target_include_directories(mod_architect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mod_architect PUBLIC convoy_core)

target_link_libraries(convoy_tests PRIVATE mod_architect)
```

**Step 9: Run tests**

```bash
cd build
ctest --verbose
```

Expected: All canvas and tool tests pass

**Step 10: Commit**

```bash
git add src/modules/mod_architect/ tests/test_canvas.cpp
git commit -m "feat: canvas system with drawing tools (pencil, eraser, bucket)"
```

---

## Task 5: .CVP Binary Format & File IO

**Files:**
- Create: `src/core/cvp_format.h`
- Create: `src/core/cvp_format.cpp`
- Create: `src/core/filesystem.h`
- Create: `src/core/filesystem.cpp`
- Create: `tests/test_cvp_format.cpp`

**Step 1: Write CVP format header**

```cpp
// src/core/cvp_format.h
#pragma once
#include "shared/types.h"
#include <vector>
#include <string>
#include <cstdint>

namespace convoy {

struct CVPHeader {
    static constexpr const char* MAGIC = "CVPF";
    static constexpr uint16_t VERSION = 1;
    
    uint16_t version;
    uint16_t layer_count;
    uint32_t canvas_width;
    uint32_t canvas_height;
} __attribute__((packed));

class CVPFormat {
public:
    static bool save_to_file(const std::string& path, const architect::Canvas* canvas);
    static bool load_from_file(const std::string& path, architect::Canvas* canvas);
    
private:
    static const size_t HEADER_SIZE;
};

} // namespace convoy
```

**Step 2: Write CVP format implementation**

```cpp
// src/core/cvp_format.cpp
#include "cvp_format.h"
#include "logger.h"
#include <fstream>
#include <cstring>

namespace convoy {

const size_t CVPFormat::HEADER_SIZE = sizeof(CVPHeader);

bool CVPFormat::save_to_file(const std::string& path, const architect::Canvas* canvas) {
    if (!canvas) return false;
    
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        Logger::error("Failed to open file for writing: {}", path);
        return false;
    }
    
    // Write header
    CVPHeader header;
    std::memcpy(header.magic, CVPHeader::MAGIC, 4);
    header.version = CVPHeader::VERSION;
    header.layer_count = canvas->get_layers().size();
    header.canvas_width = canvas->width();
    header.canvas_height = canvas->height();
    
    file.write(reinterpret_cast<const char*>(&header), HEADER_SIZE);
    
    // Write metadata (empty for now)
    // Write layers
    for (const auto& layer : canvas->get_layers()) {
        // Layer name
        uint8_t name_len = std::min((uint8_t)64, (uint8_t)layer.name.size());
        file.write(reinterpret_cast<const char*>(&name_len), 1);
        file.write(layer.name.c_str(), name_len);
        
        // Layer flags
        uint8_t flags = (layer.visible ? 1 : 0) | (layer.locked ? 2 : 0);
        file.write(reinterpret_cast<const char*>(&flags), 1);
        
        // Opacity
        file.write(reinterpret_cast<const char*>(&layer.opacity), sizeof(float));
        
        // Pixel data
        size_t pixel_count = canvas->width() * canvas->height();
        file.write(reinterpret_cast<const char*>(layer.pixels.data()), pixel_count * 4);
    }
    
    file.close();
    Logger::info("Saved canvas to: {}", path);
    return true;
}

bool CVPFormat::load_from_file(const std::string& path, architect::Canvas* canvas) {
    if (!canvas) return false;
    
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        Logger::error("Failed to open file for reading: {}", path);
        return false;
    }
    
    // Read header
    CVPHeader header;
    file.read(reinterpret_cast<char*>(&header), HEADER_SIZE);
    
    if (std::strncmp(header.magic, CVPHeader::MAGIC, 4) != 0) {
        Logger::error("Invalid CVP file magic");
        return false;
    }
    
    if (header.version != CVPHeader::VERSION) {
        Logger::error("Unsupported CVP version: {}", header.version);
        return false;
    }
    
    // Create new canvas if dimensions don't match
    if (canvas->width() != header.canvas_width || canvas->height() != header.canvas_height) {
        *canvas = architect::Canvas(header.canvas_width, header.canvas_height);
    }
    
    // Read layers
    for (uint16_t i = 0; i < header.layer_count; ++i) {
        // Read layer name
        uint8_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), 1);
        
        std::string name(name_len, '\0');
        file.read(name.data(), name_len);
        
        // Read flags
        uint8_t flags;
        file.read(reinterpret_cast<char*>(&flags), 1);
        
        bool visible = (flags & 1) != 0;
        bool locked = (flags & 2) != 0;
        
        // Read opacity
        float opacity;
        file.read(reinterpret_cast<char*>(&opacity), sizeof(float));
        
        // Add layer if needed
        if (i == 0) {
            canvas->get_layers()[0].name = name;
        } else {
            canvas->add_layer(name);
        }
        
        Layer& layer = canvas->get_layers()[i];
        layer.visible = visible;
        layer.locked = locked;
        layer.opacity = opacity;
        
        // Read pixel data
        size_t pixel_count = header.canvas_width * header.canvas_height;
        file.read(reinterpret_cast<char*>(layer.pixels.data()), pixel_count * 4);
    }
    
    file.close();
    Logger::info("Loaded canvas from: {}", path);
    return true;
}

} // namespace convoy
```

**Step 3: Write filesystem header**

```cpp
// src/core/filesystem.h
#pragma once
#include "modules/mod_architect/canvas.h"
#include <string>

namespace convoy {

class FileSystem {
public:
    static bool project_exists(const std::string& path);
    static bool create_project_directory(const std::string& path);
    
    static bool save_canvas(const std::string& project_path, const architect::Canvas* canvas);
    static bool load_canvas(const std::string& project_path, architect::Canvas* canvas);
    
    static std::string get_canvas_path(const std::string& project_path);
};

} // namespace convoy
```

**Step 4: Write filesystem implementation**

```cpp
// src/core/filesystem.cpp
#include "filesystem.h"
#include "cvp_format.h"
#include "logger.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace convoy {

bool FileSystem::project_exists(const std::string& path) {
    return fs::exists(path) && fs::is_directory(path);
}

bool FileSystem::create_project_directory(const std::string& path) {
    try {
        fs::create_directories(path);
        Logger::info("Created project directory: {}", path);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Failed to create project directory: {}", e.what());
        return false;
    }
}

std::string FileSystem::get_canvas_path(const std::string& project_path) {
    return (fs::path(project_path) / "canvas.cvp").string();
}

bool FileSystem::save_canvas(const std::string& project_path, const architect::Canvas* canvas) {
    if (!project_exists(project_path)) {
        if (!create_project_directory(project_path)) {
            return false;
        }
    }
    
    std::string canvas_path = get_canvas_path(project_path);
    return CVPFormat::save_to_file(canvas_path, canvas);
}

bool FileSystem::load_canvas(const std::string& project_path, architect::Canvas* canvas) {
    std::string canvas_path = get_canvas_path(project_path);
    if (!fs::exists(canvas_path)) {
        Logger::error("Canvas file not found: {}", canvas_path);
        return false;
    }
    
    return CVPFormat::load_from_file(canvas_path, canvas);
}

} // namespace convoy
```

**Step 5: Write CVP format tests**

```cpp
// tests/test_cvp_format.cpp
#include <gtest/gtest.h>
#include "core/cvp_format.h"
#include "core/filesystem.h"
#include "modules/mod_architect/canvas.h"
#include <filesystem>

using namespace convoy;
using namespace convoy::architect;

TEST(CVPFormatTest, SaveAndLoadCanvas) {
    std::string test_file = "/tmp/test_canvas.cvp";
    
    // Create a canvas with some pixels
    Canvas original(32, 32);
    Color red(255, 0, 0, 255);
    original.set_pixel(10, 10, red);
    original.set_pixel(20, 20, red);
    
    // Save it
    EXPECT_TRUE(CVPFormat::save_to_file(test_file, &original));
    
    // Load it
    Canvas loaded(32, 32);
    EXPECT_TRUE(CVPFormat::load_from_file(test_file, &loaded));
    
    // Verify
    Color pixel = loaded.get_pixel(10, 10);
    EXPECT_EQ(pixel.r, 255);
    EXPECT_EQ(pixel.g, 0);
    EXPECT_EQ(pixel.b, 0);
    
    // Cleanup
    std::filesystem::remove(test_file);
}

TEST(FileSystemTest, ProjectDirectory) {
    std::string test_project = "/tmp/test_project";
    
    // Create project
    EXPECT_TRUE(FileSystem::create_project_directory(test_project));
    EXPECT_TRUE(FileSystem::project_exists(test_project));
    
    // Save and load canvas
    Canvas original(64, 64);
    Color blue(0, 0, 255, 255);
    original.set_pixel(5, 5, blue);
    
    EXPECT_TRUE(FileSystem::save_canvas(test_project, &original));
    
    Canvas loaded(64, 64);
    EXPECT_TRUE(FileSystem::load_canvas(test_project, &loaded));
    
    Color pixel = loaded.get_pixel(5, 5);
    EXPECT_EQ(pixel.b, 255);
    
    // Cleanup
    std::filesystem::remove_all(test_project);
}
```

**Step 6: Update CMakeLists.txt**

Add to main CMakeLists.txt:
```cmake
target_sources(convoy_core PRIVATE
  src/core/cvp_format.cpp
  src/core/filesystem.cpp
)
```

**Step 7: Run tests**

```bash
cd build
ctest --verbose -R CVP
```

Expected: All file IO tests pass

**Step 8: Commit**

```bash
git add src/core/cvp_format.h src/core/cvp_format.cpp src/core/filesystem.h src/core/filesystem.cpp tests/test_cvp_format.cpp
git commit -m "feat: binary CVP format and filesystem IO"
```

---

## Task 6: Undo/Redo Command System

**Files:**
- Create: `src/core/command_manager.h`
- Create: `src/core/command_manager.cpp`
- Create: `src/core/commands/command.h`
- Create: `src/core/commands/pixel_command.h`
- Create: `src/core/commands/pixel_command.cpp`
- Create: `tests/test_command_manager.cpp`

**Step 1: Write command base class**

```cpp
// src/core/commands/command.h
#pragma once
#include <string>

namespace convoy {

class Command {
public:
    virtual ~Command() = default;
    
    virtual void execute() = 0;
    virtual void undo() = 0;
    
    virtual std::string description() const = 0;
};

} // namespace convoy
```

**Step 2: Write command manager header**

```cpp
// src/core/command_manager.h
#pragma once
#include "commands/command.h"
#include <memory>
#include <stack>
#include <string>

namespace convoy {

class CommandManager {
public:
    void execute(std::unique_ptr<Command> command);
    
    void undo();
    void redo();
    
    bool can_undo() const { return !undo_stack_.empty(); }
    bool can_redo() const { return !redo_stack_.empty(); }
    
    std::string undo_description() const;
    std::string redo_description() const;
    
    void clear();
    
private:
    std::stack<std::unique_ptr<Command>> undo_stack_;
    std::stack<std::unique_ptr<Command>> redo_stack_;
};

} // namespace convoy
```

**Step 3: Write command manager implementation**

```cpp
// src/core/command_manager.cpp
#include "command_manager.h"

namespace convoy {

void CommandManager::execute(std::unique_ptr<Command> command) {
    if (!command) return;
    
    command->execute();
    undo_stack_.push(std::move(command));
    
    // Clear redo stack when new command is executed
    while (!redo_stack_.empty()) {
        redo_stack_.pop();
    }
}

void CommandManager::undo() {
    if (!can_undo()) return;
    
    auto command = std::move(undo_stack_.top());
    undo_stack_.pop();
    
    command->undo();
    redo_stack_.push(std::move(command));
}

void CommandManager::redo() {
    if (!can_redo()) return;
    
    auto command = std::move(redo_stack_.top());
    redo_stack_.pop();
    
    command->execute();
    undo_stack_.push(std::move(command));
}

std::string CommandManager::undo_description() const {
    if (can_undo()) {
        return undo_stack_.top()->description();
    }
    return "Undo";
}

std::string CommandManager::redo_description() const {
    if (can_redo()) {
        return redo_stack_.top()->description();
    }
    return "Redo";
}

void CommandManager::clear() {
    while (!undo_stack_.empty()) undo_stack_.pop();
    while (!redo_stack_.empty()) redo_stack_.pop();
}

} // namespace convoy
```

**Step 4: Write pixel command**

```cpp
// src/core/commands/pixel_command.h
#pragma once
#include "command.h"
#include "shared/types.h"
#include "modules/mod_architect/canvas.h"

namespace convoy {

class PixelCommand : public Command {
public:
    PixelCommand(architect::Canvas* canvas, uint32_t x, uint32_t y, Color new_color);
    
    void execute() override;
    void undo() override;
    
    std::string description() const override;
    
private:
    architect::Canvas* canvas_;
    uint32_t x_, y_;
    Color old_color_;
    Color new_color_;
    bool executed_;
};

} // namespace convoy
```

```cpp
// src/core/commands/pixel_command.cpp
#include "pixel_command.h"

namespace convoy {

PixelCommand::PixelCommand(architect::Canvas* canvas, uint32_t x, uint32_t y, Color new_color)
    : canvas_(canvas), x_(x), y_(y), new_color_(new_color), executed_(false) {
    old_color_ = canvas->get_pixel(x, y);
}

void PixelCommand::execute() {
    if (!executed_) {
        canvas_->set_pixel(x_, y_, new_color_);
        executed_ = true;
    }
}

void PixelCommand::undo() {
    canvas_->set_pixel(x_, y_, old_color_);
    executed_ = false;
}

std::string PixelCommand::description() const {
    return "Paint pixel";
}

} // namespace convoy
```

**Step 5: Write command manager tests**

```cpp
// tests/test_command_manager.cpp
#include <gtest/gtest.h>
#include "core/command_manager.h"
#include "core/commands/pixel_command.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;
using namespace convoy::architect;

TEST(CommandManagerTest, ExecuteAndUndo) {
    CommandManager cm;
    Canvas canvas(32, 32);
    Color red(255, 0, 0, 255);
    
    auto command = std::make_unique<PixelCommand>(&canvas, 10, 10, red);
    cm.execute(std::move(command));
    
    Color pixel = canvas.get_pixel(10, 10);
    EXPECT_EQ(pixel.r, 255);
    
    EXPECT_TRUE(cm.can_undo());
    cm.undo();
    
    pixel = canvas.get_pixel(10, 10);
    EXPECT_EQ(pixel.r, 0);
}

TEST(CommandManagerTest, UndoRedo) {
    CommandManager cm;
    Canvas canvas(32, 32);
    Color blue(0, 0, 255, 255);
    
    auto cmd = std::make_unique<PixelCommand>(&canvas, 5, 5, blue);
    cm.execute(std::move(cmd));
    
    cm.undo();
    EXPECT_FALSE(cm.can_undo());
    EXPECT_TRUE(cm.can_redo());
    
    cm.redo();
    Color pixel = canvas.get_pixel(5, 5);
    EXPECT_EQ(pixel.b, 255);
}

TEST(CommandManagerTest, NewCommandClearsRedo) {
    CommandManager cm;
    Canvas canvas(32, 32);
    
    auto cmd1 = std::make_unique<PixelCommand>(&canvas, 1, 1, Color(255, 0, 0, 255));
    cm.execute(std::move(cmd1));
    
    cm.undo();
    EXPECT_TRUE(cm.can_redo());
    
    auto cmd2 = std::make_unique<PixelCommand>(&canvas, 2, 2, Color(0, 255, 0, 255));
    cm.execute(std::move(cmd2));
    
    EXPECT_FALSE(cm.can_redo());
}
```

**Step 6: Update CMakeLists.txt**

Add to main CMakeLists.txt:
```cmake
target_sources(convoy_core PRIVATE
  src/core/command_manager.cpp
  src/core/commands/pixel_command.cpp
)

target_sources(convoy_tests PRIVATE
  tests/test_command_manager.cpp
)
```

**Step 7: Run tests**

```bash
cd build
ctest --verbose -R Command
```

Expected: All command manager tests pass

**Step 8: Commit**

```bash
git add src/core/command_manager.h src/core/command_manager.cpp src/core/commands/ tests/test_command_manager.cpp
git commit -m "feat: undo/redo command system"
```

---

## Task 7: Architect UI & Integration

**Files:**
- Create: `src/modules/mod_architect/architect_ui.h`
- Create: `src/modules/mod_architect/architect_ui.cpp`
- Create: `src/modules/mod_architect/viewport.h`
- Create: `src/modules/mod_architect/viewport.cpp`

**Step 1: Write viewport header**

```cpp
// src/modules/mod_architect/viewport.h
#pragma once
#include "shared/math.h"
#include "canvas.h"
#include <vector>

namespace convoy {
namespace architect {

class Viewport {
public:
    Viewport(uint32_t width, uint32_t height);
    
    void set_zoom(float zoom);
    float get_zoom() const { return zoom_; }
    
    void pan(float dx, float dy);
    Vec2 get_offset() const { return offset_; }
    
    Vec2 screen_to_canvas(float screen_x, float screen_y) const;
    Vec2 canvas_to_screen(float canvas_x, float canvas_y) const;
    
    void fit_to_canvas(const Canvas* canvas);
    
    uint32_t viewport_width() const { return viewport_width_; }
    uint32_t viewport_height() const { return viewport_height_; }
    
private:
    float zoom_;
    Vec2 offset_;
    uint32_t viewport_width_, viewport_height_;
};

} // namespace architect
} // namespace convoy
```

**Step 2: Write viewport implementation**

```cpp
// src/modules/mod_architect/viewport.cpp
#include "viewport.h"
#include <algorithm>
#include <cmath>

namespace convoy {
namespace architect {

Viewport::Viewport(uint32_t width, uint32_t height)
    : zoom_(1.0f), offset_(0, 0), viewport_width_(width), viewport_height_(height) {}

void Viewport::set_zoom(float zoom) {
    zoom_ = std::max(0.1f, std::min(32.0f, zoom));
}

void Viewport::pan(float dx, float dy) {
    offset_.x += dx;
    offset_.y += dy;
}

Vec2 Viewport::screen_to_canvas(float screen_x, float screen_y) const {
    return Vec2(
        (screen_x - offset_.x) / zoom_,
        (screen_y - offset_.y) / zoom_
    );
}

Vec2 Viewport::canvas_to_screen(float canvas_x, float canvas_y) const {
    return Vec2(
        canvas_x * zoom_ + offset_.x,
        canvas_y * zoom_ + offset_.y
    );
}

void Viewport::fit_to_canvas(const Canvas* canvas) {
    float zoom_x = (float)viewport_width_ / canvas->width();
    float zoom_y = (float)viewport_height_ / canvas->height();
    zoom_ = std::min(zoom_x, zoom_y) * 0.95f;
    offset_ = Vec2(10, 10);
}

} // namespace architect
} // namespace convoy
```

**Step 3: Write architect UI header**

```cpp
// src/modules/mod_architect/architect_ui.h
#pragma once
#include "canvas.h"
#include "viewport.h"
#include "tools/tool_base.h"
#include "shared/types.h"
#include <memory>
#include <vector>

namespace convoy {
namespace architect {

class ArchitectUI {
public:
    ArchitectUI();
    
    void initialize(Canvas* canvas, CommandManager* command_manager);
    void render();
    
    void set_tool(ToolType tool_type);
    
private:
    Canvas* canvas_;
    CommandManager* command_manager_;
    std::unique_ptr<Viewport> viewport_;
    std::unique_ptr<Tool> current_tool_;
    
    Color foreground_color_{255, 255, 255, 255};
    Color background_color_{0, 0, 0, 0};
    
    bool is_painting_{false};
    int last_mouse_x_{0}, last_mouse_y_{0};
    
    void render_toolbar();
    void render_canvas_viewport();
    void render_layers_panel();
    void render_properties_panel();
    void handle_canvas_input();
};

} // namespace architect
} // namespace convoy
```

**Step 4: Write architect UI implementation (abbreviated)**

```cpp
// src/modules/mod_architect/architect_ui.cpp
#include "architect_ui.h"
#include "tools/pencil_tool.h"
#include "tools/eraser_tool.h"
#include "tools/bucket_tool.h"
#include <imgui.h>

namespace convoy {
namespace architect {

ArchitectUI::ArchitectUI() {
    viewport_ = std::make_unique<Viewport>(800, 600);
    current_tool_ = std::make_unique<PencilTool>();
}

void ArchitectUI::initialize(Canvas* canvas, CommandManager* command_manager) {
    canvas_ = canvas;
    command_manager_ = command_manager;
    if (canvas_) {
        viewport_->fit_to_canvas(canvas_);
    }
}

void ArchitectUI::set_tool(ToolType tool_type) {
    switch (tool_type) {
        case ToolType::Pencil:
            current_tool_ = std::make_unique<PencilTool>();
            break;
        case ToolType::Eraser:
            current_tool_ = std::make_unique<EraserTool>();
            break;
        case ToolType::Bucket:
            current_tool_ = std::make_unique<BucketTool>();
            break;
        default:
            break;
    }
    current_tool_->set_foreground_color(foreground_color_);
}

void ArchitectUI::render_toolbar() {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoMove);
    
    ImVec4 fg(foreground_color_.r / 255.0f, foreground_color_.g / 255.0f,
              foreground_color_.b / 255.0f, foreground_color_.a / 255.0f);
    ImGui::ColorButton("Foreground", fg);
    ImGui::SameLine();
    
    if (ImGui::Button("Pencil##tool")) set_tool(ToolType::Pencil);
    ImGui::SameLine();
    if (ImGui::Button("Eraser##tool")) set_tool(ToolType::Eraser);
    ImGui::SameLine();
    if (ImGui::Button("Bucket##tool")) set_tool(ToolType::Bucket);
    
    ImGui::End();
}

void ArchitectUI::render_layers_panel() {
    ImGui::Begin("Layers");
    
    if (!canvas_) {
        ImGui::End();
        return;
    }
    
    int index = 0;
    for (auto& layer : canvas_->get_layers()) {
        if (ImGui::Selectable(layer.name.c_str())) {
            canvas_->set_active_layer(index);
        }
        index++;
    }
    
    ImGui::End();
}

void ArchitectUI::render_canvas_viewport() {
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Canvas");
    
    if (canvas_) {
        std::vector<uint32_t> texture(canvas_->width() * canvas_->height());
        canvas_->composite_to_texture(texture);
        
        ImGui::Image((void*)(intptr_t)0, ImVec2(800, 600));
        handle_canvas_input();
    }
    
    ImGui::End();
}

void ArchitectUI::render_properties_panel() {
    ImGui::Begin("Properties");
    ImGui::SliderFloat("Zoom", &viewport_->get_zoom(), 0.1f, 32.0f);
    ImGui::End();
}

void ArchitectUI::handle_canvas_input() {
    ImGuiIO& io = ImGui::GetIO();
    
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            int x = (int)io.MousePos.x;
            int y = (int)io.MousePos.y;
            
            auto canvas_pos = viewport_->screen_to_canvas(io.MousePos.x, io.MousePos.y);
            
            if (!is_painting_) {
                is_painting_ = true;
                current_tool_->on_mouse_down(canvas_, canvas_pos.x, canvas_pos.y);
            } else {
                current_tool_->on_mouse_drag(canvas_, last_mouse_x_, last_mouse_y_,
                                           canvas_pos.x, canvas_pos.y);
            }
            
            last_mouse_x_ = canvas_pos.x;
            last_mouse_y_ = canvas_pos.y;
        } else if (is_painting_) {
            auto canvas_pos = viewport_->screen_to_canvas(io.MousePos.x, io.MousePos.y);
            current_tool_->on_mouse_up(canvas_, canvas_pos.x, canvas_pos.y);
            is_painting_ = false;
        }
    }
}

void ArchitectUI::render() {
    render_toolbar();
    render_canvas_viewport();
    render_layers_panel();
    render_properties_panel();
}

} // namespace architect
} // namespace convoy
```

**Step 5: Integrate into main application**

Update `src/convoy/main.cpp`:
```cpp
#include "core/window_manager.h"
#include "modules/mod_architect/architect_ui.h"
#include "core/command_manager.h"

int main() {
    try {
        convoy::WindowManager window_manager;
        window_manager.initialize("Convoy", 1600, 900);
        
        convoy::CommandManager cmd_manager;
        convoy::architect::Canvas canvas(256, 256);
        convoy::architect::ArchitectUI architect_ui;
        
        architect_ui.initialize(&canvas, &cmd_manager);
        
        // Main loop
        while (!window_manager.should_close()) {
            architect_ui.render();
            window_manager.get_event_bus().publish(convoy::CanvasModifiedEvent{0});
        }
        
        window_manager.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

**Step 6: Commit**

```bash
git add src/modules/mod_architect/architect_ui.h src/modules/mod_architect/architect_ui.cpp src/modules/mod_architect/viewport.h src/modules/mod_architect/viewport.cpp src/convoy/main.cpp
git commit -m "feat: architect UI and viewport integration"
```

---

## Task 8: Final Integration & Testing

**Files:**
- Modify: `CMakeLists.txt`
- Create: `tests/integration_test.cpp`

**Step 1: Update CMakeLists.txt for full integration**

Already covered in previous steps, ensure all modules link correctly.

**Step 2: Write integration test**

```cpp
// tests/integration_test.cpp
#include <gtest/gtest.h>
#include "core/window_manager.h"
#include "core/command_manager.h"
#include "core/filesystem.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;
using namespace convoy::architect;

TEST(IntegrationTest, FullWorkflow) {
    // Create canvas
    Canvas canvas(64, 64);
    
    // Draw something
    Color red(255, 0, 0, 255);
    canvas.set_pixel(10, 10, red);
    
    // Save to file
    std::string test_project = "/tmp/integration_test";
    EXPECT_TRUE(FileSystem::create_project_directory(test_project));
    EXPECT_TRUE(FileSystem::save_canvas(test_project, &canvas));
    
    // Load from file
    Canvas loaded(64, 64);
    EXPECT_TRUE(FileSystem::load_canvas(test_project, &loaded));
    
    // Verify
    Color pixel = loaded.get_pixel(10, 10);
    EXPECT_EQ(pixel.r, 255);
    
    // Cleanup
    std::filesystem::remove_all(test_project);
}

TEST(IntegrationTest, CanvasWithLayers) {
    Canvas canvas(32, 32);
    
    // Add layers
    canvas.add_layer("Layer 2");
    canvas.add_layer("Layer 3");
    
    EXPECT_EQ(canvas.get_layers().size(), 3);
    
    // Paint on different layers
    Color blue(0, 0, 255, 255);
    canvas.set_active_layer(1);
    canvas.set_pixel(5, 5, blue);
    
    canvas.set_active_layer(0);
    Color red(255, 0, 0, 255);
    canvas.set_pixel(5, 5, red);
    
    // Verify compositing
    std::vector<uint32_t> composite(32 * 32);
    canvas.composite_to_texture(composite);
    
    // Result should be blend of both colors
    uint32_t pixel = composite[5 * 32 + 5];
    uint8_t r = (pixel >> 16) & 0xFF;
    uint8_t b = pixel & 0xFF;
    
    EXPECT_GT(r, 0);
    EXPECT_GT(b, 0);
}
```

**Step 3: Build and run all tests**

```bash
cd build
cmake --build . --config Debug
ctest --verbose
```

Expected: All tests pass

**Step 4: Final commit**

```bash
git add tests/integration_test.cpp
git commit -m "feat: integration tests for complete workflow"
```

**Step 5: Verify build quality**

```bash
cd build
cmake --build . --config Debug -- -j4
```

Expected: Builds with no warnings or errors

---

## Definition of Done Checklist

- [ ] Task 1: CMake builds successfully on Windows and Linux
- [ ] Task 2: Math library and types tested
- [ ] Task 3: Window manager opens GLFW window with ImGui
- [ ] Task 4: Canvas system with all 5 tools working
- [ ] Task 5: Binary .cvp format save/load passing tests
- [ ] Task 6: Undo/Redo command system fully functional
- [ ] Task 7: UI rendering and input handling integrated
- [ ] Task 8: All integration tests passing
- [ ] Code compiles with no warnings
- [ ] All commits follow repository style

---

## Notes for Implementation

1. **Incremental Building**: Don't try to build everything at once. Each task should compile independently.
2. **Testing First**: Write tests before implementation when possible. Tests document expected behavior.
3. **Minimal Dependencies**: The project uses only header-only libraries (stb_image) + standard dependencies (GLFW, ImGui, OpenGL).
4. **Cross-Platform**: Use `std::filesystem` for file operations, platform-agnostic paths.
5. **Command Line Building**:
   ```bash
   mkdir -p build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   cmake --build . --config Debug
   ctest --verbose
   ```

---

*Plan saved. Ready for execution.*
