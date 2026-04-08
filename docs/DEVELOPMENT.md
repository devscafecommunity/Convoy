# Convoy Development Guide

## Code Organization

### Directory Structure
```
src/
  core/               # Host layer (reusable)
    commands/         # Undo/redo command classes
  shared/             # Data types, math, colors
  modules/
    mod_architect/    # Fase 1 module
      tools/          # Tool implementations
  convoy/             # Main executable
tests/                # All test files
docs/
  plans/              # Implementation planning
  README.md           # User guide
  ARCHITECTURE.md     # Design documentation
  DEVELOPMENT.md      # This file
  API.md              # Public API reference
```

## Building

### Prerequisites

**Windows (MSVC)**:
- Visual Studio 2019+ Community Edition (free)
- CMake 3.18+
- Git

**Linux (GCC/Clang)**:
```bash
sudo apt-get install build-essential cmake git libgl1-mesa-dev libxrandr-dev
```

### Build Steps

```bash
cd .worktrees/convoy-fase1
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..      # Debug symbols + no optimizations
cmake --build . --config Debug -j4      # Parallel build with 4 cores
```

### Incremental Builds

After initial build, edit code and rebuild:
```bash
cd build
cmake --build . --config Debug
```

### Clean Build

```bash
rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..     # Release = optimized
cmake --build . --config Release -j4
```

## Testing

### Run All Tests

```bash
cd build
ctest --output-on-failure -V             # -V = verbose
```

### Run Specific Test

```bash
ctest -R test_name                        # Run tests matching regex
ctest -R "math|canvas"                    # Run math AND canvas tests
ctest -E test_to_skip                     # Run all EXCEPT matching
```

### Run Test with Debugger (Linux)

```bash
gdb ./bin/test_math
(gdb) run
(gdb) backtrace                           # On crash
```

### Test File Locations

```
tests/
  test_math.cpp              # Math library (Vec2, Rect, Color)
  test_window_manager.cpp    # Window/ImGui integration
  test_canvas.cpp            # Canvas + tools (pencil, eraser, bucket)
  test_cvp_format.cpp        # Binary save/load
  test_command_manager.cpp   # Undo/redo system
  test_architect_ui.cpp      # Viewport + UI
  integration_test.cpp       # Full workflow (draw → save → load)
```

## Code Style

### .clang-format

Project uses **Google C++ Style Guide** with 120-column line width.

**Auto-format entire file**:
```bash
clang-format -i src/modules/mod_architect/canvas.cpp
```

**Check formatting (no changes)**:
```bash
clang-format --dry-run -Werror src/file.cpp
```

### Naming Conventions

| Element | Convention | Example |
|---------|-----------|------|
| Class | PascalCase | `PencilTool`, `EventBus` |
| Function | snake_case | `set_pixel`, `on_mouse_down` |
| Constant | SCREAMING_SNAKE | `MAX_LAYERS`, `DEFAULT_WIDTH` |
| Member variable | snake_case_ (trailing _) | `layer_count_`, `current_tool_` |
| Local variable | snake_case | `canvas_x`, `total_pixels` |
| Struct | PascalCase | `Event`, `Color` |

### Header Files

**Always use `#pragma once`** (not `#ifndef` guards):

```cpp
#pragma once

#include <vector>
#include "shared/types.h"

namespace convoy {
namespace core {

class MyClass {
  // ...
};

}  // namespace core
}  // namespace convoy
```

### Namespaces

- `convoy` - Root namespace
- `convoy::core` - Window, events, filesystem, commands
- `convoy::shared` - Math, types, colors
- `convoy::modules::architect` - Architect module

## Writing Tests

### Test Structure

Use **Google Test** framework. Each test file has one header to test.

**File**: `tests/test_canvas.cpp`
```cpp
#include <gtest/gtest.h>
#include "modules/mod_architect/canvas.h"

using namespace convoy::modules::architect;

class CanvasTest : public ::testing::Test {
 protected:
    Canvas canvas{800, 600};  // SetUp
};

TEST_F(CanvasTest, SetPixelSetsColor) {
    canvas.SetPixel(0, 10, 20, Color::RED);
    ASSERT_EQ(canvas.GetPixel(0, 10, 20), Color::RED);
}

TEST_F(CanvasTest, OutOfBoundsIsClipped) {
    canvas.SetPixel(0, -1, 0, Color::RED);  // Should not crash
    // Behavior: either clipped or ignored
    ASSERT_EQ(canvas.width(), 800);  // Canvas unchanged
}
```

### Test Naming

- `TEST_F(ClassName, DescribesBehavior)` - Descriptive
- Bad: `TEST_F(CanvasTest, Test1)` - Too vague
- Good: `TEST_F(CanvasTest, SetPixelSetsColor)` - Action + outcome

### Assertions

| Assertion | Purpose |
|-----------|---------|
| `ASSERT_EQ(a, b)` | Equals (fatal, stops test) |
| `EXPECT_EQ(a, b)` | Equals (non-fatal, continues) |
| `ASSERT_TRUE(x)` | Truthy |
| `ASSERT_FALSE(x)` | Falsy |
| `ASSERT_THROW(expr, exception)` | Throws specific exception |
| `ASSERT_NO_THROW(expr)` | Doesn't throw |

**Use `ASSERT_` for critical assertions** (test setup), **`EXPECT_`** for checks.

### Mocking (Optional)

For complex dependencies, use Google Mock:
```cpp
class MockEventBus : public EventBus {
 public:
    MOCK_METHOD(void, Publish, (const Event&), (override));
};

TEST_F(MyTest, CallsEventBus) {
    MockEventBus bus;
    EXPECT_CALL(bus, Publish).Times(1);
    // ...
}
```

## Debugging

### CMake Debug Build

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Adds:
- `-g` flag (debug symbols)
- No optimizations (easier stepping)
- Runtime assertion checks (if present)

### GDB (Linux)

```bash
gdb ./bin/test_math
(gdb) run
(gdb) run arg1 arg2              # With arguments
(gdb) break canvas.cpp:50        # Breakpoint at line
(gdb) next / step                # Step over / step into
(gdb) continue                   # Resume
(gdb) print variable_name        # Inspect variable
(gdb) backtrace                  # Call stack
(gdb) quit
```

### MSVC Debugger (Windows Visual Studio)

Open `.sln` generated by CMake:
```bash
start build/Convoy.sln
```

Then:
- Set breakpoints (F9)
- Run (F5)
- Step (F10/F11)
- Inspect (hover over variables)

### Logging

Use `Logger::Info()`, `Logger::Warn()`, `Logger::Error()` (defined in `core/logger.h`):

```cpp
#include "core/logger.h"

Logger::Info("Canvas created: {}x{}", width, height);
Logger::Warn("Layer count exceeds max, clamping to 32");
Logger::Error("Failed to open file: {}", filename);
```

## Git Workflow

### Branching

- Main branch: `main` (stable, release-ready)
- Feature branches: `feat/feature-name`
- Bugfix branches: `fix/issue-name`

### Commits

Keep commits **atomic** (one feature per commit):

```bash
# Good: One feature per commit
git commit -m "feat: add bucket fill tool"
git commit -m "fix: correct a bug"

# Bad: Multiple features in one commit
git commit -m "add bucket fill and fix coordinates and update docs"
```

### Commit Message Format

Use conventional commits:
- `feat: add new feature`
- `fix: correct a bug`
- `docs: add or update documentation`
- `test: add or update tests`
- `refactor: improve code without changing behavior`

### Creating Pull Requests

1. Push feature branch: `git push origin feat/my-feature`
2. Open PR on GitHub with:
   - Clear title: "feat: add pencil tool"
   - Description: Why this change, testing notes
   - Link related issues
3. Wait for code review
4. Address feedback, push updates (same branch)
5. Merge when approved

## Common Tasks

### Adding a New Tool

1. **Create tool file**: `src/modules/mod_architect/tools/my_tool.h/cpp`
2. **Inherit from Tool**:
```cpp
class MyTool : public Tool {
    void OnMouseDown(int x, int y, const Canvas&, Viewport&) override;
    void OnMouseDrag(int x, int y, const Canvas&, Viewport&) override;
    void OnMouseUp(int x, int y, const Canvas&, Viewport&) override;
};
```
3. **Register in UI**: Add button in `ArchitectUI::Render()`
4. **Add tests**: Create `tests/test_my_tool.cpp`
5. **Commit**: `git commit -m "feat: add my tool"`

### Exte
