# Quick Start Guide

> Get Convoy up and running in 5 minutes.

## 1. Prerequisites

- Linux with g++ 11+ compiler
- CMake 3.18+
- GLFW3, OpenGL dev libraries
- 4GB RAM minimum

```bash
sudo apt-get install build-essential cmake libglfw3-dev libgl1-mesa-dev
```

## 2. Clone & Build

```bash
git clone https://github.com/your-org/Convoy.git
cd Convoy && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel 4
```

**Expected time**: 2-3 minutes

## 3. Run

```bash
./convoy
```

**Expected**: Application window opens with 1600x900 ImGui interface.

## 4. First Code Change

### Add a Log Message

File: `src/convoy/main.cpp`

```cpp
int main(int argc, char* argv[]) {
    try {
        std::cerr << "🚛 Starting Convoy!" << std::endl;  // Add this
        convoy::WindowManager window_manager;
        // ...rest of code
    }
}
```

### Rebuild & Test

```bash
cd build
cmake --build .
./convoy
```

Should print: `🚛 Starting Convoy!`

## 5. Project Structure

```
Convoy/
├── src/
│   ├── convoy/           # Main entry point
│   ├── core/             # Window management, UI, input
│   ├── modules/          # Editor modules
│   │   ├── mod_architect/  # Pixel art tools
│   │   ├── mod_walker/     # World editor
│   │   ├── mod_forge/      # Texture packing
│   │   └── mod_sequencer/  # Animation system
│   └── shared/           # Common types (Color, Layer, etc.)
├── docs/                 # Documentation
├── build/                # Build output
└── CMakeLists.txt        # Build configuration
```

## 6. Naming Conventions

- **Classes**: `PascalCase` (e.g., `WindowManager`, `ColorSelector`)
- **Functions**: `snake_case` (e.g., `setup_defaults()`, `render_frame()`)
- **Variables**: `snake_case` (e.g., `window_manager`, `active_layer_`)
- **Constants**: `SCREAMING_SNAKE_CASE` (e.g., `MAX_LAYERS`)
- **Namespaces**: `lowercase` (e.g., `convoy`, `architect`)

## 7. Key Classes to Know

| Class | Location | Purpose |
|---|---|---|
| `WindowManager` | `src/core/` | Window & rendering lifecycle |
| `Canvas` | `src/modules/mod_architect/` | Pixel grid for drawing |
| `Layer` | `src/shared/types.h` | Image layer (pixels) |
| `Color` | `src/shared/color.h` | RGBA color struct |
| `InputHandler` | `src/core/input/` | Keyboard/mouse input |

## 8. Common Tasks

### Build Debug Version
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Run with Debugger
```bash
gdb ./convoy
(gdb) run
```

### Clean Build
```bash
cd build
rm -rf .
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Run Tests
```bash
cd build
ctest --verbose
```

## 9. Making Your First Feature

### Example: Add Brush Size Display

1. **Create test** (if doing TDD)
2. **Implement in Canvas class**
3. **Update UI to show value**
4. **Test with application**
5. **Commit**

See [DEVELOPMENT.md](DEVELOPMENT.md) for detailed workflow.

## 10. Next Steps

- **Architecture**: Read [ARCHITECTURE.md](ARCHITECTURE.md)
- **Development**: Read [DEVELOPMENT.md](DEVELOPMENT.md)
- **API Reference**: Read [API.md](API.md)
- **Modules**: Read [MODULES.md](MODULES.md)
- **Build Help**: Read [BUILD.md](BUILD.md) or [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

## Troubleshooting

| Problem | Solution |
|---|---|
| Build fails | Check [BUILD.md](BUILD.md) and [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |
| Segfault | GLAD not initialized (see [TROUBLESHOOTING.md](TROUBLESHOOTING.md)) |
| Can't find header | Check include path in CMakeLists.txt |
| No window appears | Check X11 display or use WSLg on WSL2 |

---

**Questions?** Check the [documentation index](README.md) or ask the team.
