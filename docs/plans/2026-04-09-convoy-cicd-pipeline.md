# Convoy CI/CD Release Pipeline Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a multi-platform CI/CD pipeline that builds and tests on Windows + Linux on every push, and automatically releases binaries when the `VERSION` in `CMakeLists.txt` is bumped on `main`.

**Architecture:** Two GitHub Actions workflows — `ci.yml` (fast headless build + tests, no graphics deps) and `release.yml` (full graphics build, version-bump detection, GitHub Release). GLAD and ImGui committed to `third_party/`. GLFW installed via vcpkg on Windows, built from source on Linux.

**Tech Stack:** GitHub Actions, CMake 3.20+, MSVC (Windows), GCC (Linux), vcpkg (Windows GLFW), GLAD 3.3 Core, ImGui v1.91.9

---

## Pre-flight checks (before starting any task)

1. Confirm repo remote: `git remote get-url origin` → should be `https://github.com/devscafecommunity/Convoy.git`
2. Confirm current branch: `git branch --show-current` → work on `main`
3. Confirm clean working tree: `git status`

---

### Task 1: Populate `third_party/glad/`

**Files:**
- Create: `third_party/glad/include/glad/glad.h`
- Create: `third_party/glad/include/KHR/khrplatform.h`
- Create: `third_party/glad/src/glad.c`

**Step 1: Install glad Python package**
```bash
pip install glad
```
Expected output: `Successfully installed glad-x.x.x`

**Step 2: Generate GLAD for OpenGL 3.3 Core with loader**
Run from repo root:
```bash
glad --out-path third_party/glad --api gl:core=3.3 c
```
Expected: creates `third_party/glad/include/glad/glad.h`, `third_party/glad/include/KHR/khrplatform.h`, `third_party/glad/src/glad.c`

**Step 3: Verify files exist**
```bash
ls third_party/glad/include/glad/glad.h
ls third_party/glad/include/KHR/khrplatform.h
ls third_party/glad/src/glad.c
```
Expected: all three files present, no errors.

**Step 4: Commit**
```bash
git add third_party/glad/
git commit -m "chore: add GLAD OpenGL 3.3 core sources"
```

---

### Task 2: Populate `third_party/imgui/`

**Files:**
- Create: `third_party/imgui/imgui.h`, `imgui.cpp`, `imgui_demo.cpp`, `imgui_draw.cpp`, `imgui_widgets.cpp`, `imgui_tables.cpp`
- Create: `third_party/imgui/imgui_internal.h`, `imconfig.h`, `imstb_rectpack.h`, `imstb_textedit.h`, `imstb_truetype.h`
- Create: `third_party/imgui/backends/imgui_impl_opengl3.h`, `imgui_impl_opengl3.cpp`
- Create: `third_party/imgui/backends/imgui_impl_glfw.h`, `imgui_impl_glfw.cpp`

**Step 1: Clone ImGui v1.91.9 into a temp directory**
```bash
git clone --depth=1 --branch v1.91.9 https://github.com/ocornut/imgui tmp_imgui
```

**Step 2: Create destination directories**
```bash
mkdir -p third_party/imgui/backends
```

**Step 3: Copy core ImGui files**
```bash
cp tmp_imgui/imgui.h tmp_imgui/imgui.cpp third_party/imgui/
cp tmp_imgui/imgui_demo.cpp tmp_imgui/imgui_draw.cpp third_party/imgui/
cp tmp_imgui/imgui_widgets.cpp tmp_imgui/imgui_tables.cpp third_party/imgui/
cp tmp_imgui/imgui_internal.h tmp_imgui/imconfig.h third_party/imgui/
cp tmp_imgui/imstb_rectpack.h tmp_imgui/imstb_textedit.h tmp_imgui/imstb_truetype.h third_party/imgui/
```

**Step 4: Copy backend files**
```bash
cp tmp_imgui/backends/imgui_impl_opengl3.h tmp_imgui/backends/imgui_impl_opengl3.cpp third_party/imgui/backends/
cp tmp_imgui/backends/imgui_impl_glfw.h tmp_imgui/backends/imgui_impl_glfw.cpp third_party/imgui/backends/
```

**Step 5: Clean up temp clone**
```bash
rm -rf tmp_imgui
```

**Step 6: Verify**
```bash
ls third_party/imgui/imgui.h
ls third_party/imgui/backends/imgui_impl_glfw.h
```
Expected: both files present.

**Step 7: Commit**
```bash
git add third_party/imgui/
git commit -m "chore: add ImGui v1.91.9 sources (core + opengl3/glfw backends)"
```

---

### Task 3: Fix `src/cmake/FindGLAD.cmake`

**Problem:** The current file never sets `GLAD_FOUND`, so `if(GLAD_FOUND)` in CMakeLists.txt always evaluates to false even when glad sources are present.

**Files:**
- Modify: `src/cmake/FindGLAD.cmake`

**Step 1: Replace the entire file with this content:**

```cmake
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include")
set(GLAD_SOURCE      "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/glad.c")

if(EXISTS "${GLAD_INCLUDE_DIR}/glad/glad.h" AND EXISTS "${GLAD_SOURCE}")
  find_package(OpenGL QUIET)
  if(OpenGL_FOUND)
    if(NOT TARGET GLAD)
      add_library(GLAD INTERFACE)
      target_include_directories(GLAD INTERFACE "${GLAD_INCLUDE_DIR}")
      target_sources(GLAD INTERFACE "${GLAD_SOURCE}")
      add_library(GLAD::GLAD ALIAS GLAD)
    endif()
    set(GLAD_FOUND TRUE)
  else()
    set(GLAD_FOUND FALSE)
  endif()
else()
  set(GLAD_FOUND FALSE)
endif()
```

**Step 2: Commit**
```bash
git add src/cmake/FindGLAD.cmake
git commit -m "fix: FindGLAD sets GLAD_FOUND based on third_party file existence"
```

---

### Task 4: Fix `src/cmake/FindImGui.cmake`

**Problem:** The current file never sets `ImGui_FOUND`, and unconditionally references `GLFW::GLFW` which may not exist in headless mode.

**Files:**
- Modify: `src/cmake/FindImGui.cmake`

**Step 1: Replace the entire file with this content:**

```cmake
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

# Guard: ImGui depends on GLFW and OpenGL already being found
if(NOT GLFW3_FOUND OR NOT OpenGL_FOUND)
  set(ImGui_FOUND FALSE)
  return()
endif()

if(EXISTS "${IMGUI_DIR}/imgui.h" AND
   EXISTS "${IMGUI_DIR}/backends/imgui_impl_opengl3.h" AND
   EXISTS "${IMGUI_DIR}/backends/imgui_impl_glfw.h")
  if(NOT TARGET ImGui)
    add_library(ImGui INTERFACE)
    target_include_directories(ImGui INTERFACE "${IMGUI_DIR}")
    target_sources(ImGui INTERFACE
      "${IMGUI_DIR}/imgui.cpp"
      "${IMGUI_DIR}/imgui_demo.cpp"
      "${IMGUI_DIR}/imgui_draw.cpp"
      "${IMGUI_DIR}/imgui_widgets.cpp"
      "${IMGUI_DIR}/imgui_tables.cpp"
      "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
      "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    )
    target_link_libraries(ImGui INTERFACE OpenGL::OpenGL GLFW::GLFW)
    add_library(ImGui::ImGui ALIAS ImGui)
  endif()
  set(ImGui_FOUND TRUE)
else()
  set(ImGui_FOUND FALSE)
endif()
```

**Step 2: Commit**
```bash
git add src/cmake/FindImGui.cmake
git commit -m "fix: FindImGui sets ImGui_FOUND and guards against missing GLFW/OpenGL"
```

---

### Task 5: Create `vcpkg.json`

**Files:**
- Create: `vcpkg.json` (repo root)

**Step 1: Create the file with this content:**

```json
{
  "name": "convoy",
  "version-string": "1.0.0",
  "dependencies": [
    "glfw3"
  ]
}
```

**Step 2: Commit**
```bash
git add vcpkg.json
git commit -m "chore: add vcpkg manifest for Windows GLFW3 dependency"
```

---

### Task 6: Create `.github/workflows/ci.yml`

**Files:**
- Create: `.github/workflows/ci.yml`

**Step 1: Create directories**
```bash
mkdir -p .github/workflows
```

**Step 2: Create file with this content:**

```yaml
name: CI

on:
  push:
    branches: ['**']
  pull_request:
    branches: ['**']

jobs:
  build-and-test:
    name: ${{ matrix.config.name }}
    runs-on: ${{ matrix.config.os }}

    strategy:
      fail-fast: false
      matrix:
        config:
          - name: "Windows MSVC"
            os: windows-latest
          - name: "Ubuntu GCC"
            os: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - name: Configure CMake (headless — graphics deps not required)
        run: cmake -B build -DCMAKE_BUILD_TYPE=Debug

      - name: Build
        run: cmake --build build --config Debug

      - name: Test
        run: ctest --test-dir build --build-config Debug --output-on-failure
```

**Step 3: Commit**
```bash
git add .github/workflows/ci.yml
git commit -m "ci: add headless build+test workflow for Windows and Linux"
```

---

### Task 7: Create `.github/workflows/release.yml`

**Files:**
- Create: `.github/workflows/release.yml`

**Step 1: Create file with this content:**

```yaml
name: Release

on:
  push:
    branches: [main]

jobs:
  # ── Step 1: Check if the version in CMakeLists.txt is new ───────────────────
  check-version:
    runs-on: ubuntu-latest
    outputs:
      version: ${{ steps.version.outputs.version }}
      should_release: ${{ steps.tag_check.outputs.should_release }}

    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0  # required to see all tags

      - name: Extract version from CMakeLists.txt
        id: version
        run: |
          VERSION=$(grep -oP '(?<=project\(Convoy VERSION )\d+\.\d+\.\d+' CMakeLists.txt)
          echo "version=$VERSION" >> $GITHUB_OUTPUT
          echo "Detected version: $VERSION"

      - name: Check if tag already exists
        id: tag_check
        run: |
          if git rev-parse "v${{ steps.version.outputs.version }}" >/dev/null 2>&1; then
            echo "Tag v${{ steps.version.outputs.version }} already exists — skipping release."
            echo "should_release=false" >> $GITHUB_OUTPUT
          else
            echo "New version v${{ steps.version.outputs.version }} — will create release."
            echo "should_release=true" >> $GITHUB_OUTPUT
          fi

  # ── Step 2: Build full graphics executable on both platforms ────────────────
  build-release:
    needs: check-version
    if: needs.check-version.outputs.should_release == 'true'
    name: Build ${{ matrix.config.name }}
    runs-on: ${{ matrix.config.os }}

    strategy:
      fail-fast: false
      matrix:
        config:
          - name: "Windows"
            os: windows-latest
            artifact_name: convoy-windows-x86_64
            artifact_ext: zip
          - name: "Linux"
            os: ubuntu-latest
            artifact_name: convoy-linux-x86_64
            artifact_ext: tar.gz

    steps:
      - uses: actions/checkout@v4

      # ── Windows: GLFW via vcpkg (static-md = static libs + dynamic CRT) ─────
      - name: Bootstrap vcpkg (Windows)
        if: runner.os == 'Windows'
        run: |
          git clone --depth=1 https://github.com/microsoft/vcpkg.git C:/vcpkg
          C:/vcpkg/bootstrap-vcpkg.bat -disableMetrics
        shell: bash

      - name: Cache vcpkg packages (Windows)
        if: runner.os == 'Windows'
        uses: actions/cache@v4
        with:
          path: C:/vcpkg/installed
          key: vcpkg-windows-${{ hashFiles('vcpkg.json') }}
          restore-keys: vcpkg-windows-

      - name: Configure CMake (Windows)
        if: runner.os == 'Windows'
        run: |
          cmake -B build \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" \
            -DVCPKG_TARGET_TRIPLET=x64-windows-static-md
        shell: bash

      # ── Linux: build GLFW 3.4 from source (Ubuntu apt ships 3.3.x) ──────────
      - name: Install build dependencies (Linux)
        if: runner.os == 'Linux'
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake libgl1-mesa-dev xorg-dev wget unzip

      - name: Build and install GLFW 3.4 from source (Linux)
        if: runner.os == 'Linux'
        run: |
          wget -q https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
          unzip -q glfw-3.4.zip
          cmake -B glfw-build -S glfw-3.4 \
            -DGLFW_BUILD_DOCS=OFF \
            -DGLFW_BUILD_TESTS=OFF \
            -DGLFW_BUILD_EXAMPLES=OFF \
            -DCMAKE_BUILD_TYPE=Release
          cmake --build glfw-build
          sudo cmake --install glfw-build
          rm -rf glfw-3.4 glfw-3.4.zip glfw-build

      - name: Configure CMake (Linux)
        if: runner.os == 'Linux'
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release

      # ── Build ────────────────────────────────────────────────────────────────
      - name: Build
        run: cmake --build build --config Release

      # ── Verify convoy executable was produced ────────────────────────────────
      - name: Verify executable (Windows)
        if: runner.os == 'Windows'
        run: test -f build/Release/convoy.exe
        shell: bash

      - name: Verify executable (Linux)
        if: runner.os == 'Linux'
        run: test -f build/convoy

      # ── Package ──────────────────────────────────────────────────────────────
      - name: Package (Windows)
        if: runner.os == 'Windows'
        run: |
          mkdir dist
          cp build/Release/convoy.exe dist/
          7z a convoy-windows-x86_64.zip ./dist/convoy.exe
        shell: bash

      - name: Package (Linux)
        if: runner.os == 'Linux'
        run: |
          mkdir dist
          cp build/convoy dist/
          tar -czf convoy-linux-x86_64.tar.gz -C dist convoy

      - name: Upload artifact
        uses: actions/upload-artifact@v4
        with:
          name: ${{ matrix.config.artifact_name }}
          path: ${{ matrix.config.artifact_name }}.${{ matrix.config.artifact_ext }}

  # ── Step 3: Create git tag and GitHub Release ────────────────────────────────
  create-release:
    needs: [check-version, build-release]
    if: needs.check-version.outputs.should_release == 'true'
    runs-on: ubuntu-latest
    permissions:
      contents: write

    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0
          token: ${{ secrets.GITHUB_TOKEN }}

      - name: Create and push git tag
        run: |
          git config user.name "github-actions[bot]"
          git config user.email "github-actions[bot]@users.noreply.github.com"
          git tag "v${{ needs.check-version.outputs.version }}"
          git push origin "v${{ needs.check-version.outputs.version }}"

      - name: Download all build artifacts
        uses: actions/download-artifact@v4
        with:
          path: artifacts

      - name: Create GitHub Release
        uses: softprops/action-gh-release@v2
        with:
          tag_name: v${{ needs.check-version.outputs.version }}
          name: Convoy v${{ needs.check-version.outputs.version }}
          generate_release_notes: true
          files: |
            artifacts/convoy-windows-x86_64/convoy-windows-x86_64.zip
            artifacts/convoy-linux-x86_64/convoy-linux-x86_64.tar.gz
```

**Step 2: Commit**
```bash
git add .github/workflows/release.yml
git commit -m "ci: add release workflow with version-bump detection and multi-platform builds"
```

---

### Task 8: Push and verify

**Step 1: Push all commits to main**
```bash
git push origin main
```

**Step 2: Watch the CI run**
Navigate to `https://github.com/devscafecommunity/Convoy/actions`.
- The `CI` workflow should trigger and pass on both Windows and Linux.
- The `Release` workflow will also trigger. Since version is `1.0.0` and no `v1.0.0` tag exists yet, it will build both platforms and create the first release.

**⚠️ If you do NOT want an immediate v1.0.0 release on this push:**
Before pushing, manually create the tag so the workflow sees it already exists:
```bash
git tag v1.0.0
git push origin v1.0.0
git push origin main
```
The release workflow will then detect `v1.0.0` already exists and skip — no release produced.

**Step 3: Verify CI passes**
- Both matrix jobs (Windows + Linux) should show green checkmarks.
- If any job fails, check the logs and fix the issue before the next push.

**Step 4: Verify release (if triggered)**
- Go to `https://github.com/devscafecommunity/Convoy/releases`
- Confirm `Convoy v1.0.0` exists with two attachments:
  - `convoy-windows-x86_64.zip`
  - `convoy-linux-x86_64.tar.gz`

---

## Cutting a future release

To release version `1.2.0`, only do this:

```cmake
# CMakeLists.txt — change line 2
project(Convoy VERSION 1.2.0 LANGUAGES CXX)
```

```bash
git add CMakeLists.txt
git commit -m "chore: bump version to 1.2.0"
git push origin main
```

The release workflow detects no `v1.2.0` tag → builds both platforms → creates GitHub Release automatically.
