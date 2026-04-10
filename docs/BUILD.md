# Building Convoy

> Complete build instructions for the Convoy project.

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Dependencies](#dependencies)
3. [Build Steps](#build-steps)
4. [Troubleshooting](#troubleshooting)

---

## System Requirements

- **OS**: Linux (x86-64)
- **C++ Standard**: C++17 or later
- **Compiler**: g++ 11+ or clang++ 13+
- **CMake**: Version 3.18 to 3.28 (versions 4.x have issues with this project)
- **RAM**: Minimum 4GB (for linking)

---

## Dependencies

### Required System Packages

```bash
sudo apt-get install -y \
  build-essential \
  cmake \
  libglfw3-dev \
  libgl1-mesa-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxcursor-dev \
  libxi-dev
```

### Included Dependencies (via Conan or Git Submodules)

- **GLFW3** - Window and input management
- **OpenGL** - 3D graphics API (via GLAD loader)
- **ImGui** - Immediate mode UI framework
- **GoogleTest** - Testing framework
- **GLAD** - OpenGL function loader

---

## Build Steps

### 1. Clone the Repository

```bash
git clone https://github.com/your-org/Convoy.git
cd Convoy
```

### 2. Create Build Directory

```bash
mkdir -p build
cd build
```

### 3. Configure with CMake

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Options:**
- `-DCMAKE_BUILD_TYPE=Debug` - Debug build with symbols
- `-DCMAKE_BUILD_TYPE=Release` - Optimized release build
- `-DCMAKE_BUILD_TYPE=RelWithDebInfo` - Release with debug symbols

### 4. Build the Project

```bash
cmake --build . --config Release --parallel 4
```

**Or using make directly:**

```bash
make -j4
```

**Expected output:**
```
[  2%] Built target convoy_glad
[ 34%] Built target convoy_core
[ 67%] Built target mod_architect
[ 78%] Built target mod_walker
[ 89%] Built target mod_forge
[100%] Built target convoy
```

### 5. Verify Build Success

```bash
ls -lh ./convoy
file ./convoy
ldd ./convoy
```

**Expected:**
- Binary size: ~1.7MB
- Type: ELF 64-bit LSB pie executable
- All dependencies resolved (libglfw.so.3, libOpenGL.so.0, libc.so.6, etc.)

---

## Running the Application

### Execute the Binary

```bash
./convoy
```

**Expected output:**
```
[INFO] WindowManager initialized 1600x900
```

The window should open and display the ImGui interface.

---

## Build Artifacts

After successful build:

```
Convoy/build/
├── convoy                    # Main executable (1.7MB)
├── libconvoy_core.a         # Core library
├── libmod_architect.a       # Architect module
├── libmod_walker.a          # Walker module
├── libmod_forge.a           # Forge module
├── libmod_sequencer.a       # Sequencer module
├── libconvoy_glad.a         # OpenGL loader
└── CMakeFiles/              # Build configuration
```

---

## Development Build

For faster iteration during development:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug --parallel 4
```

Then run with debugger:

```bash
gdb ./convoy
(gdb) run
```

---

## Clean Build

To remove all build artifacts and start fresh:

```bash
cd Convoy
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

---

## Build on macOS

macOS build has different dependency paths. Install via Homebrew:

```bash
brew install cmake glfw3 glad
```

Then follow the standard build steps above. Note: OpenGL version may differ (use `-DGLFW_INCLUDE_NONE`).

---

## Build on Windows

For Windows (experimental):

1. Install Visual Studio 2019 or newer
2. Install CMake 3.18+
3. Install GLFW3 and dependencies via vcpkg:
   ```bash
   vcpkg install glfw3 glad imgui
   ```
4. Configure with Visual Studio generator:
   ```bash
   cmake .. -G "Visual Studio 17 2022"
   cmake --build . --config Release
   ```

---

## Troubleshooting

### CMake Configuration Fails

**Error:** `CMake Error at CMakeLists.txt:1 (cmake_minimum_required): CMake 3.28 or higher is required.`

**Solution:** Update CMake or explicitly use supported version:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release  # Version 3.18-3.28 only
```

### OpenGL/GLAD Initialization Error

**Error:** `error: OpenGL header already included`

**Solution:** Ensure `#define GLFW_INCLUDE_NONE` comes BEFORE `#include <GLFW/glfw3.h>` in all .cpp files.

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for more issues.

---

## Next Steps

- **Run the application**: `./convoy`
- **Read development guide**: See [DEVELOPMENT.md](DEVELOPMENT.md)
- **Understand architecture**: See [ARCHITECTURE.md](ARCHITECTURE.md)
- **Get started coding**: See [QUICK_START.md](QUICK_START.md)
