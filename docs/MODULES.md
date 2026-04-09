# Project Convoy: Modules Reference

> Overview of Convoy's modular architecture and each module's responsibility.

## Module System Overview

Convoy uses a **plugin-style module system** where each module is independent but communicates through the core system.

```
WindowManager (Core)
    ├── ArchitectUI (mod_architect)
    ├── WalkerUI (mod_walker)
    ├── ForgeUI (mod_forge)
    └── SequencerUI (mod_sequencer)
```

---

## Core Module (`src/core/`)

**Responsibility**: Window management, rendering, UI framework, input handling

### Key Components

| Component | File | Purpose |
|---|---|---|
| `WindowManager` | `window_manager.cpp` | GLFW window, OpenGL context, ImGui lifecycle |
| `InputHandler` | `input/input_handler.cpp` | Keyboard/mouse input processing |
| `KeybindManager` | `keybind_manager.cpp` | Hotkey configuration and dispatch |
| `CommandManager` | `command_manager.cpp` | Undo/redo system |
| `ThemeManager` | `ui/theme_manager.cpp` | ImGui theme and styling |
| `DockSpaceManager` | `ui/dockspace_manager.cpp` | Dockable window layout |

### Key UI Components

- `ColorMaker`: Color creation panel
- `ColorSelector`: Color selection panel
- `PreferencesWindow`: Settings dialog
- `PressureCurveEditor`: Pressure sensitivity curve editor
- `ExportWizard`: Export options dialog
- `MainMenuBar`: Top menu bar with File, Edit, etc.

### Startup Sequence

```cpp
WindowManager::initialize()
    ├── glfwInit()           // GLFW window system
    ├── glfwCreateWindow()   // Create window
    ├── glfwMakeContextCurrent()  // Make GL context active
    ├── gladLoadGL()         // Load OpenGL functions (CRITICAL)
    ├── ImGui::CreateContext()    // ImGui context
    ├── ImGui_ImplGlfw_InitForOpenGL()  // GLFW ImGui backend
    ├── ImGui_ImplOpenGL3_Init()  // OpenGL ImGui backend
    └── Module initialization (architect_ui, etc.)
```

---

## Architect Module (`src/modules/mod_architect/`)

**Responsibility**: Pixel art editing tools and canvas

### Key Classes

| Class | File | Purpose |
|---|---|---|
| `Canvas` | `canvas.cpp` | Pixel grid with layers |
| `Layer` | `shared/types.h` | Single image layer |
| `ArchitectUI` | `architect_ui.cpp` | Architect panel UI |
| `BrushStroke` | `brush_stroke.cpp` | Stroke rendering system |
| `LivePreview` | `ui/live_preview.cpp` | Real-time preview window |

### Features

- Multi-layer canvas (max 32 layers)
- Pixel-by-pixel drawing API
- Brush stroke system (pressure, size)
- Live preview rendering
- Layer visibility/locking
- Opacity per layer

### Data Flow

```
User Input
    ↓
InputHandler.process_key()
    ↓
Hotkey dispatch (PENCIL_TOOL, etc.)
    ↓
ArchitectUI updates active tool
    ↓
Canvas.set_pixel() on mouse drag
    ↓
Layer.pixels updated
    ↓
composite_to_texture() for rendering
    ↓
OpenGL texture displayed
```

---

## Walker Module (`src/modules/mod_walker/`)

**Responsibility**: World/map editing with tiles and collision

### Key Components

- **TileGrid**: Large tile-based world
- **CollisionLayer**: Binary collision map (passable/blocked)
- **Layer**: Z-depth sorting for draws

### Not Yet Implemented

- Tile palette system
- Brush for placing tiles
- Collision editing UI
- Hot-linking with Architect

---

## Forge Module (`src/modules/mod_forge/`)

**Responsibility**: Spritesheet packing and optimization

### Key Components

- **AtlasPacker**: Bin-packing algorithm
- **MipmapGenerator**: Texture LOD generation
- **ExportSettings**: Packing configuration

### Features (Planned)

- Automatic sprite atlasing
- Mipmap generation
- Memory alignment for GPU optimization
- Batch export

### Not Yet Implemented

- UI for packing
- Export to .CAF format
- Optimization profiles

---

## Sequencer Module (`src/modules/mod_sequencer/`)

**Responsibility**: Animation timeline and state machines

### Key Components

- **Timeline**: Keyframe-based animation
- **StateMachine**: FSM for animation states
- **AnimationClip**: Single animation sequence

### Features (Planned)

- Timeline UI with frame scrubbing
- Keyframe editor
- State machine visual editor
- Frame-by-frame preview

### Not Yet Implemented

- Full UI
- State machine visualization
- Animation export

---

## Shared Module (`src/shared/`)

**Responsibility**: Common types used across all modules

### Key Types

| Type | File | Purpose |
|---|---|---|
| `Color` | `color.h` | RGBA color struct with conversions |
| `Layer` | `types.h` | Image layer with pixels vector |
| `Vec2` | `math.h` | 2D vector for positions |
| `Rect` | `math.h` | Rectangle with intersection tests |
| `SpriteMetadata` | `types.h` | Sprite metadata (pivot, hitbox) |
| `ToolType` | `types.h` | Enum of available tools |

### Color Math

- `Color::to_rgba()` - Convert to 32-bit RGBA
- `Color::to_abgr()` - Convert to 32-bit ABGR
- `Color::from_rgba()` - Create from 32-bit value
- Operator overloads: `==`, `!=`

---

## Module Interaction Pattern

### Adding a New Feature to Multiple Modules

Example: **Pressure Sensitivity**

1. **Shared**: Define `PressureSettings` struct
2. **Core**: Add `PressureCurveEditor` UI
3. **Architect**: Use pressure in `BrushStroke`
4. **Walker**: Optional pressure for tile brush
5. **Forge**: Optional pressure-based texel selection

```cpp
// shared/pressure.h
struct PressureSettings {
    float min_pressure;
    float max_pressure;
    PressureCurve curve;
};

// Architect module uses
float effective_size = brush_size * pressure_settings.evaluate(current_pressure);
```

---

## Module Initialization Order

```
1. Core WindowManager created
2. OpenGL/ImGui initialized
3. Each module's UI initialized via architect_ui.initialize(&canvas, &cmd_mgr)
4. Hotkeys registered for each module
5. Run loop starts
```

---

## Module Extension Example

To add a new module (e.g., `mod_camera`):

1. **Create directory**: `src/modules/mod_camera/`
2. **Create header**: `camera_ui.h` with public API
3. **Create implementation**: `camera_ui.cpp`
4. **Add to CMakeLists.txt**: New library target
5. **Link from core**: `target_link_libraries(convoy_core mod_camera)`
6. **Initialize**: Call in `window_manager.cpp`

---

## Current Module Status

| Module | Status | Completeness |
|---|---|---|
| **Core** | ✅ Working | 80% - Main loop functional, some UI missing |
| **Architect** | ✅ Working | 40% - Canvas works, tools not rendering |
| **Walker** | ⚠️ Partial | 10% - Compilation only |
| **Forge** | ⚠️ Partial | 5% - Compilation only |
| **Sequencer** | ⚠️ Partial | 5% - Compilation only |

---

## Module Responsibilities Checklist

When implementing a new module:

- [ ] Define public API in header
- [ ] Implement UI panel
- [ ] Register hotkeys (if applicable)
- [ ] Add to CMakeLists.txt
- [ ] Initialize in WindowManager
- [ ] Handle undo/redo via CommandManager
- [ ] Support undo for all mutations
- [ ] Validate all inputs
- [ ] Log errors via Logger
- [ ] Document public API
- [ ] Write tests

