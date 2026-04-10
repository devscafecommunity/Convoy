# Troubleshooting Guide

> Common issues and solutions when building and running Convoy.

---

## Build Issues

### CMake Configuration Fails: "CMake 3.28 or higher required"

**Error Message:**
```
CMake Error at CMakeLists.txt:1 (cmake_minimum_required):
CMake 3.28 or higher is required
```

**Cause**: CMake version is too new (4.x) or too old (<3.18)

**Solution**:
```bash
# Check CMake version
cmake --version

# Use supported version (3.18-3.28)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Or install specific version
sudo apt-get install cmake=3.28.0-1  # Ubuntu/Debian
brew install cmake@3.28              # macOS
```

---

### OpenGL Header Conflict: "OpenGL header already included"

**Error Message:**
```
error: #error OpenGL header already included, remove this include, glad already provides it
```

**Cause**: GLFW is including GL.h before GLAD, causing conflicts

**Solution**: Ensure this in all .cpp files with OpenGL:

```cpp
// CORRECT ORDER:
#define GLFW_INCLUDE_NONE        // Tell GLFW not to include GL.h
#include <GLFW/glfw3.h>
#include <glad/glad.h>           // GLAD includes GL.h properly
#include <imgui.h>
```

**Files to check**:
- `src/core/window_manager.cpp`
- Any file using GLFW + OpenGL

---

### Linker Error: "undefined reference to glfwInit"

**Error Message:**
```
undefined reference to `glfwInit'
```

**Cause**: GLFW3 library not linked or not installed

**Solution**:
```bash
# Install GLFW3
sudo apt-get install libglfw3-dev      # Ubuntu/Debian
brew install glfw3                      # macOS

# Verify CMakeLists.txt has:
find_package(glfw3 REQUIRED)
target_link_libraries(convoy_core glfw)
```

---

### Compilation Error: "Cannot find imgui.h"

**Error Message:**
```
fatal error: imgui.h: No such file or directory
```

**Cause**: ImGui not in include path or third_party directory missing

**Solution**:
```bash
# Verify third_party exists
ls -la third_party/imgui/

# Check CMakeLists.txt has include directories
include_directories(third_party/imgui)
include_directories(third_party/imgui/backends)
```

---

## Runtime Issues

### SIGSEGV: Segmentation Fault on Startup

**Error Message:**
```
Falta de segmentação (núcleo despejado)
Segmentation fault (core dumped)
```

**Cause**: Most common - GLAD not initialized, OpenGL pointers are NULL

**Solution**:

1. **Check window_manager.cpp** has GLAD initialization:
```cpp
glfwMakeContextCurrent(window_);
glfwSwapInterval(1);

if (!gladLoadGL()) {  // MUST be here
    throw std::runtime_error("Failed to initialize GLAD");
}
```

2. **If still crashing**, debug with GDB:
```bash
gdb ./convoy
(gdb) run
(gdb) bt full          # Get stack trace
(gdb) print variable   # Check values
```

---

### GLFW Error: "Failed to create window"

**Error Message:**
```
std::runtime_error: Failed to create window
```

**Cause**: 
- No X11 display server
- OpenGL not available
- Window size too large

**Solution**:
```bash
# For headless systems, set display
export DISPLAY=:0

# Check if X11 is running
ps aux | grep Xvfb    # Or Xvnc, Xwayland, etc.

# For testing without display, use virtual framebuffer
Xvfb :99 -screen 0 1024x768x24 &
export DISPLAY=:99
./convoy
```

---

### ImGui Rendering Nothing

**Symptom**: Window opens but no UI visible

**Causes & Solutions**:

1. **OpenGL not initialized**:
   - Check GLAD is initialized before ImGui
   - Verify `glClearColor()` works (OpenGL call)

2. **ImGui context not created**:
   ```cpp
   ImGui::CreateContext();        // Must come before backends
   ImGui_ImplGlfw_InitForOpenGL(window_, true);
   ImGui_ImplOpenGL3_Init("#version 410");
   ```

3. **Render loop not implemented**:
   - `run_loop()` must call ImGui render commands
   - Need glClear, ImGui NewFrame/Render, SwapBuffers

---

## Development Issues

### Code Changes Don't Take Effect

**Issue**: Rebuild but changes not applied

**Solution**:
```bash
cd build

# Full clean rebuild
rm -rf .
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Or use make clean
make clean
make -j4
```

---

### Tests Won't Compile

**Error**: References to undefined methods like `get_event_bus()`

**Status**: Known issue - tests ahead of implementation

**Workaround**:
```bash
# Build only executable, skip tests
cmake --build . --target convoy
```

---

### Performance Issues

**Symptom**: Application runs but is slow/stuttering

**Causes & Solutions**:

1. **Debug build** - Use Release:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build .
   ```

2. **VSync enabled** - Can cause 60 FPS cap:
   ```cpp
   glfwSwapInterval(0);  // Disable VSync
   glfwSwapInterval(1);  // Enable VSync (60 FPS)
   ```

3. **CPU throttling** - Check system:
   ```bash
   watch -n 1 "cat /proc/cpuinfo | grep MHz"
   ```

---

## Platform-Specific Issues

### macOS: Vulkan/Metal Instead of OpenGL

**Note**: macOS deprecated OpenGL. Use MoltenVK or Metal bindings.

**For now, OpenGL 4.1 still works**:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./convoy
```

---

### Windows: MSVC Linker Errors

**Issue**: Unresolved external symbols

**Solution**: Use vcpkg for dependencies:
```bash
vcpkg install glfw3:x64-windows glad:x64-windows imgui:x64-windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

---

### WSL: Display Server Issues

**Issue**: No X11 display in WSL

**Solution**: Use WSLg (Windows Subsystem for Linux GUI):
```bash
# WSL2 automatically uses WSLg
./convoy
# Should open window on Windows desktop
```

---

## Getting Help

When reporting issues, include:

1. **System info**:
   ```bash
   lsb_release -a
   uname -a
   cmake --version
   g++ --version
   ```

2. **Build log**:
   ```bash
   cmake --build . 2>&1 | tee build.log
   ```

3. **Runtime output**:
   ```bash
   ./convoy 2>&1 | tee runtime.log
   ```

4. **Stack trace** (if crashing):
   ```bash
   gdb ./convoy -ex run -ex "bt full" -batch 2>&1
   ```

---

## Still Stuck?

1. Check [BUILD.md](BUILD.md) for build instructions
2. Check [DEVELOPMENT.md](DEVELOPMENT.md) for workflow
3. Review [ARCHITECTURE.md](ARCHITECTURE.md) for design
4. Search existing issues on GitHub

**Or ask for help** with:
- What you tried
- What error you got
- What you expected
