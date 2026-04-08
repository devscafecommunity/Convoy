# Convoy API Reference

Public interfaces for all modules and core systems.

## Core

### WindowManager

**Header**: `src/core/window_manager.h`

```cpp
namespace convoy::core {

class WindowManager {
 public:
    static WindowManager& Instance();
    
    bool Initialize(int width, int height, const std::string& title);
    void Shutdown();
    bool IsRunning() const;
    
    void BeginFrame();
    void EndFrame();
    
    ImDrawList* GetDrawList();
    int GetWindowWidth() const;
    int GetWindowHeight() const;
};

}
```

**Usage**:
```cpp
auto& window = WindowManager::Instance();
window.Initialize(800, 600, "My App");

while (window.IsRunning()) {
    window.BeginFrame();
    // Render ImGui
    window.EndFrame();
}
```

### EventBus

**Header**: `src/core/event_bus.h`

```cpp
namespace convoy::core {

class Event { /* abstract */ };

class EventBus {
 public:
    static EventBus& Instance();
    
    template<typename EventType>
    void Subscribe(std::function<void(const EventType&)> handler);
    
    void Publish(const Event& event);
};

}
```

**Custom Events**:
```cpp
struct PixelDrawnEvent : public Event {
    int x, y;
    Color color;
};

struct ProjectSavedEvent : public Event {
    std::string path;
};
```

**Usage**:
```cpp
EventBus::Instance().Subscribe<PixelDrawnEvent>([](const auto& e) {
    std::cout << "Pixel drawn at " << e.x << "," << e.y << '\n';
});

EventBus::Instance().Publish(PixelDrawnEvent{10, 20, Color::RED});
```

### Filesystem

**Header**: `src/core/filesystem.h`

```cpp
namespace convoy::core {

class Filesystem {
 public:
    static void InitializeProjectDirectory();
    
    static std::string GetProjectDirectory();
    
    static bool SaveProject(const std::string& name, const Project& project);
    
    static Project LoadProject(const std::string& name);
};

}
```

**Usage**:
```cpp
Filesystem::InitializeProjectDirectory();

Project p;
// ... populate project ...
Filesystem::SaveProject("my_painting", p);

// Later:
auto loaded = Filesystem::LoadProject("my_painting");
```

### CommandManager

**Header**: `src/core/command_manager.h`

```cpp
namespace convoy::core {

class CommandManager {
 public:
    static CommandManager& Instance();
    
    void Execute(std::unique_ptr<Command> command);
    
    void Undo();
    void Redo();
    
    bool CanUndo() const;
    bool CanRedo() const;
    
    void Clear();
};

}
```

**Usage**:
```cpp
auto cmd = std::make_unique<PixelCommand>(canvas, layer, x, y, new_color, old_color);
CommandManager::Instance().Execute(std::move(cmd));

// User presses Ctrl+Z:
CommandManager::Instance().Undo();
```

### CVPFormat

**Header**: `src/core/cvp_format.h`

```cpp
namespace convoy::core {

class CVPFormat {
 public:
    static constexpr int VERSION = 1;
    
    static std::vector<uint8_t> Serialize(const Project& project);
    
    static Project Deserialize(const std::vector<uint8_t>& data);
};

}
```

**Binary Format**:
```
Offset  | Size  | Field
--------|-------|-------
0       | 4     | Magic: "CVPF" (0x43564946)
4       | 4     | Version (1)
8       | 4     | Canvas width (800)
12      | 4     | Canvas height (600)
16      | 1     | Layer count (1-32)
17      | 1     | Active layer index (0-31)
18      | var   | Pixel data (RGBA, one layer at a time)
```

---

## Shared

### Math Library

**Header**: `src/shared/math.h`

```cpp
namespace convoy::shared {

struct Vec2 {
    float x, y;
    
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    float Magnitude() const;
    Vec2 Normalized() const;
    float Dot(const Vec2& other) const;
};

struct Rect {
    float x, y, width, height;
    
    bool Contains(float px, float py) const;
    bool Intersects(const Rect& other) const;
};

std::vector<std::pair<int, int>> BresenhamLine(int x0, int y0, int x1, int y1);

}
```

### Types

**Header**: `src/shared/types.h`

```cpp
namespace convoy::shared {

struct Color {
    uint8_t r, g, b, a;  // 0-255
    
    static constexpr Color RED{255, 0, 0, 255};
    static constexpr Color GREEN{0, 255, 0, 255};
    static constexpr Color BLUE{0, 0, 255, 255};
    static constexpr Color WHITE{255, 255, 255, 255};
    static constexpr Color BLACK{0, 0, 0, 255};
    static constexpr Color TRANSPARENT{0, 0, 0, 0};
    
    uint32_t ToRGBA() const;   // As 32-bit RGBA
    uint32_t ToABGR() const;   // As 32-bit ABGR (OpenGL)
    static Color FromRGBA(uint32_t rgba);
};

struct Layer {
    int width, height;
    std::vector<Color> pixels;  // RGBA, row-major
    
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);
    void Clear();
};

struct Canvas {
    int width, height;
    std::vector<Layer> layers;
    int active_layer;
    
    void SetPixel(int layer, int x, int y, const Color& color);
    Color GetPixel(int layer, int x, int y) const;
    std::vector<uint32_t> Composite() const;  // Blended RGBA buffer
};

struct Project {
    Canvas canvas;
    std::string name;
    uint64_t created_timestamp;
};

}
```

---

## Modules

### Architect Module

**Header**: `src/modules/mod_architect/canvas.h`

```cpp
namespace convoy::modules::architect {

class Canvas {
 public:
    Canvas(int width, int height);
    
    void SetPixel(int layer, int x, int y, const Color& color);
    Color GetPixel(int layer, int x, int y) const;
    
    void InsertLayer(int position);
    void DeleteLayer(int layer);
    int GetLayerCount() const;
    int GetActiveLayer() const;
    void SetActiveLayer(int layer);
    
    std::vector<uint32_t> Composite() const;
    
    int width() const;
    int height() const;
};

}
```

**Tools Base Class**:

**Header**: `src/modules/mod_architect/tools/tool_base.h`

```cpp
namespace convoy::modules::architect {

class Tool {
 public:
    virtual ~Tool() = default;
    
    virtual void OnMouseDown(int x, int y, Canvas& canvas, Viewport& viewport) = 0;
    virtual void OnMouseDrag(int x, int y, Canvas& canvas, Viewport& viewport) = 0;
    virtual void OnMouseUp(int x, int y, Canvas& canvas, Viewport& viewport) = 0;
};

}
```

**Pencil Tool**:

**Header**: `src/modules/mod_architect/tools/pencil_tool.h`

```cpp
namespace convoy::modules::architect {

class PencilTool : public Tool {
 public:
    void SetColor(const Color& color);
    void OnMouseDown(int x, int y, Canvas& canvas, Viewport& viewport) override;
    void OnMouseDrag(int x, int y, Canvas& canvas, Viewport& viewport) override;
    void OnMouseUp(int x, int y, Canvas& canvas, Viewport& viewport) override;
};

}
```

**Eraser Tool**:

**Header**: `src/modules/mod_architect/tools/eraser_tool.h`

```cpp
namespace convoy::modules::architect {

class EraserTool : public Tool {
 public:
    void OnMouseDown(int x, int y, Canvas& canvas, Viewport& viewport) override;
    void OnMouseDrag(int x, int y, Canvas& canvas, Viewport& viewport) override;
    void OnMouseUp(int x, int y, Canvas& canvas, Viewport& viewport) override;
};

}
```

**Bucket Fill Tool**:

**Header**: `src/modules/mod_architect/tools/bucket_tool.h`

```cpp
namespace convoy::modules::architect {

class BucketTool : public Tool {
 public:
    void SetColor(const Color& color);
    void OnMouseDown(int x, int y, Canvas& canvas, Viewport& viewport) override;
    void OnMouseDrag(int x, int y, Canvas& canvas, Viewport& viewport) override;
    void OnMouseUp(int x, int y, Canvas& canvas, Viewport& viewport) override;
};

}
```

**Viewport**:

**Header**: `src/modules/mod_architect/viewport.h`

```cpp
namespace convoy::modules::architect {

class Viewport {
 public:
    Viewport(int screen_width, int screen_height, const Canvas& canvas);
    
    std::pair<int, int> ScreenToCanvas(int screen_x, int screen_y) const;
    std::pair<int, int> CanvasToScreen(int canvas_x, int canvas_y) const;
    
    void SetZoom(float factor);
    float GetZoom() const;
    
    void Pan(int dx, int dy);
};

}
```

**Architect UI**:

**Header**: `src/modules/mod_architect/architect_ui.h`

```cpp
namespace convoy::modules::architect {

class ArchitectUI {
 public:
    ArchitectUI(Canvas& canvas, Viewport& viewport);
    
    void Rend
