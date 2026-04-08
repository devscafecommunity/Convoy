# Convoy Architecture

## Design Philosophy

Convoy uses a **Host/Plugin architecture** with clear separation of concerns:

- **Core (Host)**: Window, event bus, filesystem, undo/redo system
- **Modules (Plugins)**: Stateless, communicate only via the event bus

This enables:
- Independent testing of each module
- Easy addition of new modules (Walker, Forge) without changing core
- Graceful error isolation
- Clear upgrade paths

## Core Modules

### Window Manager (`src/core/window_manager.h/cpp`)

**Responsibility**: GLFW + OpenGL context, ImGui integration, main loop

**Key Classes**:
- `WindowManager` - Singleton managing GLFW window and OpenGL state

**Data Flow**:
1. Main loop calls `WindowManager::BeginFrame()`
2. Submodules render via ImGui (ImDrawList)
3. `WindowManager::EndFrame()` swaps buffers

**Thread Safety**: Single-threaded (main thread only)

### Event Bus (`src/core/event_bus.h/cpp`)

**Responsibility**: Publish/subscribe message passing

**Key Classes**:
- `EventBus` - Singleton managing event subscriptions
- `Event` - Abstract base for all events

**Example**:
```cpp
// Subscribe to pixel drawn events
eventBus->Subscribe<PixelDrawnEvent>([](const auto& e) {
    std::cout << "Pixel at " << e.x << "," << e.y << '\n';
});

// Publish
eventBus->Publish(PixelDrawnEvent{10, 20, Color::RED});
```

**Guarantees**:
- Subscribers called synchronously
- Order of subscription = order of calls
- Single event per publish

### Filesystem (`src/core/filesystem.h/cpp`)

**Responsibility**: Project directory management, save/load coordination

**Key Functions**:
- `InitializeProjectDirectory()` - Creates `convoy_projects/` folder
- `SaveProject(const std::string& path, const Project&)` - Delegates to CVP format
- `LoadProject(const std::string& path)` - Delegates to CVP format

**Data Flow**:
1. User clicks Save
2. Filesystem calls `CVPFormat::Serialize()` with canvas + metadata
3. File written to `convoy_projects/project_name.cvp`
4. Event published: `ProjectSavedEvent`

### CVP Binary Format (`src/core/cvp_format.h/cpp`)

**Responsibility**: Binary serialization of canvas state

**Format Structure** (fixed, no TLV):
```
Offset  | Size  | Field
--------|-------|-------
0       | 4     | Magic: "CVPF"
4       | 4     | Version: 1
8       | 4     | Canvas width
12      | 4     | Canvas height
16      | 1     | Layer count (0-32)
17      | 1     | Active layer index
18      | N*W*H | Pixel data (RGBA per layer)
```

**Key Classes**:
- `CVPFormat` - Static methods for serialize/deserialize

**Validation**:
- Magic mismatch → throw `std::runtime_error`
- Version mismatch → throw `std::runtime_error`
- Layer count > 32 → clamp to 32

### Command System (`src/core/commands/`)

**Responsibility**: Undo/redo history management

**Key Classes**:
- `Command` - Abstract base
- `PixelCommand` - Concrete: paint single pixel
- `CommandManager` - Maintains undo/redo stacks

**Data Flow**:
1. User paints pixel
2. `PencilTool` creates `PixelCommand(x, y, color, old_color)`
3. `CommandManager::Execute()` calls `PixelCommand::Execute()`
4. Stacks updated: `[undo_stack.push(), redo_stack.clear()]`

**Guarantees**:
- Undo goes back one step
- Redo goes forward one step
- Drawing on redo stack clears future history

## Module: Architect (`src/modules/mod_architect/`)

**Responsibility**: Canvas, drawing tools, UI, viewport

### Canvas (`canvas.h/cpp`)

**Responsibility**: 2D pixel grid with layer support

**Key Classes**:
- `Canvas` - Holds up to 32 layers, composites on demand
- `Layer` - Single RGBA pixel grid (800×600 by default)

**Data Structures**:
```cpp
class Canvas {
    std::vector<Layer> layers;  // 0 = bottom
    int active_layer;           // 0-31
    int width, height;
};
```

**Operations**:
- `SetPixel(layer, x, y, color)` - Paint one pixel
- `GetPixel(layer, x, y)` - Read one pixel
- `Composite()` - Blend all layers into RGBA buffer
- `ClearLayer(layer)` - Fill with transparent
- `InsertLayer(position)` - Add new layer
- `DeleteLayer(layer)` - Remove layer (min 1 always present)

**Coordinate System**:
- (0,0) = top-left
- X increases right, Y increases down
- Out-of-bounds access is clamped

### Viewport (`viewport.h/cpp`)

**Responsibility**: Screen↔Canvas coordinate conversion, zoom handling

**Key Classes**:
- `Viewport` - Manages pan, zoom, screen-to-canvas mapping

**Operations**:
- `ScreenToCanvas(screen_x, screen_y)` → (canvas_x, canvas_y)
- `CanvasToScreen(canvas_x, canvas_y)` → (screen_x, screen_y)
- `SetZoom(float factor)` - Scale canvas (1.0 = 1 pixel = 1 screen pixel)
- `Pan(dx, dy)` - Move viewport

**Example**:
```cpp
// User clicks (100, 200) on screen
auto [cx, cy] = viewport.ScreenToCanvas(100, 200);
// If zoomed 2x, might be canvas (50, 100)
canvas.SetPixel(active_layer, cx, cy, current_color);
```

### Tools (`tools/`)

**Common Base**:
```cpp
class Tool {
    virtual void OnMouseDown(int x, int y, const Canvas&, Viewport&) = 0;
    virtual void OnMouseDrag(int x, int y, const Canvas&, Viewport&) = 0;
    virtual void OnMouseUp(int x, int y, const Canvas&, Viewport&) = 0;
};
```

**Pencil Tool** (`pencil_tool.h/cpp`):
- Draws continuous line when mouse dragged
- Uses Bresenham's line algorithm
- Creates `PixelCommand` for each pixel
- Undo-friendly (one command per stroke)

**Eraser Tool** (`eraser_tool.h/cpp`):
- Similar to Pencil, but sets alpha = 0 (transparent)
- No color selection needed

**Bucket Tool** (`bucket_tool.h/cpp`):
- Flood fill using BFS
- Fills contiguous pixels of same color
- Creates single `PixelCommand` (if we extend it)
- Max iterations: 10,000 pixels (prevents runaway)

### Architect UI (`architect_ui.h/cpp`)

**Responsibility**: ImGui toolbar, canvas display, layer panel

**Layout** (hard-coded ImGui):
```
┌────────────────────────────────────┐
│ Toolbar: [Pencil] [Eraser] [Bucket]│
├──────────────┬──────────────────────┤
│ Layers Panel │   Canvas Viewport    │
│  ☑ Layer 2   │   (scrollable)       │
│  ☑ Layer 1   │                      │
│  ☑ Layer 0   │                      │
└──────────────┴──────────────────────┘
```

**Key Methods**:
- `Render()` - Called each frame, draws entire UI
- `HandleInput()` - Mouse/keyboard from WindowManager
- `SelectTool(ToolType)` - Updates current tool
- `SelectLayer(int)` - Updates active layer

**Event Subscriptions**:
- `ProjectLoadedEvent` - Reinitialize canvas display
- `ProjectSavedEvent` - Update title bar

## Data Flow: User Draws a Pixel

```
1. User clicks canvas at (100, 150) screen coords
   ↓
2. ArchitectUI::HandleInput() captures click
   ↓
3. Viewport::ScreenToCanvas(100, 150) → (50, 75) canvas coords
   ↓
4. CurrentTool->OnMouseDown(50, 75)
   ↓
5. PencilTool creates PixelCommand(canvas, layer, 50, 75, RED, old_color)
   ↓
6. CommandManager::Execute(pixel_command)
   ↓
7. PixelCommand::Execute() calls canvas.SetPixel(layer, 50, 75, RED)
   ↓
8. EventBus::Publish(PixelDrawnEvent{...})
   ↓
9. Any subscribed effects react (e.g., re-render viewport)
   ↓
10. Next frame: Architect UI renders updated canvas to screen
```

## Design Patterns Used

| Pattern | Location | Purpose |
|---------|----------|---------|
| Singleton | WindowManager, EventBus, CommandManager | Global state with single entry point |
| Observer | EventBus + subscribers | Decoupled communication |
| Command | Command, PixelCommand, CommandManager | Undo/redo support |
| Factory | ToolFactory (implied) | Tool instantiation |
| Strategy | Tool subclasses | Runtime tool switching |

## Threading Model

**Current**: Single-threaded main loop
- All rendering on main thread
- All event handling on main thread
- No async operations

**Future** (Fase 2+): Could add:
- Background file I/O via std::future
- Async effects processing

---

**Last Updated**: 2026-04-08
