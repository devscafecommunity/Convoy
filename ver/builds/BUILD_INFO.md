# Convoy Build Artifacts (Windows x64)

**Date**: 2026-04-09  
**Platform**: Windows 10.0.26200 (MSVC 19.44)  
**Build Type**: Release  
**Architecture**: x86-64

## Contents

### `win/` - Windows x64 Release Build

- **convoy_tests.exe** (486 KB)
  - Headless test executable (no graphics dependencies)
  - 55 unit tests passing
  - Run with: `convoy_tests.exe`

- **convoy_core.lib** (549 KB)
  - Core library (headless: event_bus, cvp_format, filesystem, command_manager, plugins)
  - Linked with GoogleTest v1.13.0

- **mod_architect.lib** (133 KB)
  - Architect module (pixel art editor)
  - Canvas, viewport, tools (pencil, eraser, bucket, pivot, hitbox)

### `linux/` - Linux x86-64 Release Build
(To be populated after Linux build - requires GCC, GLFW 3.4 from source)

## Test Results

```
[==========] 55 tests from 13 test suites ran. (40 ms total)
[  PASSED  ] 55 tests.
```

### Test Suites
- MathTest (2 tests)
- ColorTest (1 test)
- CanvasTest (1 test)
- ViewportTest (2 tests)
- CommandManagerTest (2 tests)
- CVPFormatTest (2 tests)
- PivotToolTest (1 test)
- HitboxToolTest (1 test)
- PluginRegistryTest (2 tests)
- HookSystemTest (8 tests)
- DODVisualizerTest (3 tests)
- IntegrationTest (2 tests)

## Build Configuration

- **CMake**: 3.31.4
- **Compiler**: MSVC 19.44.35222 (VS Build Tools 2022)
- **C++ Standard**: C++17 with /W4 /WX (warnings as errors)
- **Dependencies**:
  - GoogleTest v1.13.0 (FetchContent)
  - GLAD 3.3 core (third_party)
  - ImGui v1.91.9 (third_party, graphics build pending)

## Notes

- **Headless build**: Graphics dependencies (GLFW3, OpenGL, ImGui) not linked
- **Main executable**: Not built in this configuration (requires vcpkg/GLFW3 setup)
- **Full graphics build**: Planned for release workflow with CI integration

## To Rebuild

```bash
cd Convoy
mkdir build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

