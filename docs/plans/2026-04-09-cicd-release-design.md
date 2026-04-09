# CI/CD Release Pipeline Design
**Date**: 2026-04-09  
**Status**: Approved

---

## Goal

Implement a multi-platform build and automated release system for Convoy using GitHub Actions. The pipeline compiles on Windows (MSVC) and Linux (GCC), and automatically publishes a GitHub Release whenever the version in `CMakeLists.txt` is bumped on `main`.

---

## Architecture

Two GitHub Actions workflows:

```
.github/workflows/
├── ci.yml       — Runs on every push/PR (fast feedback, tests only, no graphics)
└── release.yml  — Runs on push to main, detects version bump, auto-tags + releases
```

### CI Workflow (`ci.yml`)
- **Trigger**: push or pull_request to any branch
- **Matrix**: `[windows-latest + MSVC, ubuntu-latest + GCC]`
- **Build mode**: headless (no GLFW/ImGui/GLAD required) — tests compile and run fine
- **Steps**: checkout → CMake configure → build → run tests
- **Purpose**: fast feedback loop, no binary artifacts produced

### Release Workflow (`release.yml`)
- **Trigger**: push to `main`
- **Version detection**: reads `project(Convoy VERSION x.y.z)` from `CMakeLists.txt`, compares against latest git tag
- **If version unchanged**: no-op (workflow exits early)
- **If version bumped**: creates git tag `vX.Y.Z`, then triggers full build matrix
- **Matrix**: `[windows-latest + MSVC + vcpkg, ubuntu-latest + GCC + apt]`
- **Steps**: checkout → install graphics deps → CMake configure (Release, full graphics) → build `convoy` executable → package → create GitHub Release

---

## Dependencies

### Windows (MSVC + vcpkg)
- `vcpkg.json` manifest at repo root declaring `glfw3`
- vcpkg bootstrapped in CI; `-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake` passed to CMake
- vcpkg binary cache stored in GitHub Actions cache (keyed on `vcpkg.json` hash) — fast on cache hit
- GLAD + ImGui: committed directly to `third_party/`

### Linux (GCC + apt)
- `sudo apt-get install -y libglfw3-dev libgl1-mesa-dev xorg-dev`
- GLAD + ImGui: same `third_party/` bundle

### `third_party/` contents (to commit)
- `third_party/glad/include/glad/glad.h`
- `third_party/glad/src/glad.c`
- `third_party/imgui/*.h` + `*.cpp` (core imgui + backends: opengl3, glfw)

---

## Release Artifacts

| File | Platform | Contents |
|---|---|---|
| `convoy-windows-x86_64.zip` | Windows | `convoy.exe` + required DLLs from vcpkg |
| `convoy-linux-x86_64.tar.gz` | Linux | `convoy` binary |

---

## Version Bump Workflow

To cut a new release, a developer only needs to:

```cmake
# CMakeLists.txt — bump this line
project(Convoy VERSION 1.0.1 LANGUAGES CXX)
```

Then push to `main`. The release workflow detects the change, creates tag `v1.0.1`, builds both platforms, and publishes the GitHub Release automatically.

---

## Files to Create / Modify

| File | Action |
|---|---|
| `.github/workflows/ci.yml` | Create |
| `.github/workflows/release.yml` | Create |
| `vcpkg.json` | Create |
| `third_party/glad/...` | Add GLAD sources |
| `third_party/imgui/...` | Add ImGui sources |
