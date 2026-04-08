# Convoy - The Codex Art Station

A modular, cross-platform pixel art editor written in modern C++. Convoy provides a robust architecture for interactive graphics applications with plugin-based extensibility.

## Features

- **Multi-Layer Canvas**: Up to 32 layers per project with full compositing support
- **Essential Tools**: Pencil (freehand), Eraser (transparency), and Bucket Fill (flood) tools
- **Undo/Redo System**: Full command-based history with unlimited undo/redo
- **Binary Format**: Proprietary .cvp format for efficient project storage
- **Cross-Platform**: Builds on Windows (MSVC) and Linux (GCC/Clang)
- **Modern UI**: ImGui-based interface with real-time viewport

## Architecture

\`\`\`
┌─────────────────────────────────────────────────────┐
│                   Convoy Core                       │
│  (Window Manager, Event Bus, Filesystem, Commands)  │
└────────────────────┬────────────────────────────────┘
                     │
        ┌────────────┴────────────┐
        │                         │
    ┌───▼────────────┐    ┌──────▼────────────┐
    │  mod_architect │    │  Future Modules   │
    │  (Canvas, UI,  │    │  (Walker, Forge)  │
    │   Tools)       │    │  (Phase 2+)       │
    └────────────────┘    └───────────────────┘
\`\`\`

## Quick Start

### Prerequisites

- **Windows**: Visual Studio 2019+ or MSVC 141+
- **Linux**: GCC 9+, CMake 3.18+
- **Common**: OpenGL 4.1 compatible GPU

### Build from Source

\`\`\`bash
# Clone and enter worktree
cd .worktrees/convoy-fase1

# Configure CMake
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build . --config Release -j\$(nproc)

# Run tests
ctest --output-on-failure

# Run application
./bin/convoy
\`\`\`

### On Linux

\`\`\`bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install libgl1-mesa-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev

# Build as above
\`\`\`

## Usage

1. **Create Project**: Launch Convoy, creates 800×600 canvas
2. **Select Tool**: Use toolbar buttons (Pencil, Eraser, Bucket)
3. **Draw**: Click to place pixels, drag to draw lines
4. **Layers**: Add/remove/reorder layers in left panel
5. **Save**: File → Save creates \`project.cvp\` binary
6. **Load**: File → Open reopens project with full fidelity

## Project Structure

\`\`\`
src/
  core/              # Window, events, filesystem, commands
  shared/            # Math, types, colors (shared across modules)
  modules/
    mod_architect/   # Canvas, tools, UI (Fase 1)
  convoy/            # Main entry point
tests/               # Comprehensive test suite
docs/
  plans/             # Implementation planning docs
  README.md          # This file
  ARCHITECTURE.md    # Detailed architecture
  DEVELOPMENT.md     # Contributing guide
  API.md             # Public API reference
\`\`\`

## Testing

All code is covered by unit and integration tests.

\`\`\`bash
cd build
ctest -V                    # Run all tests with verbose output
ctest -R pencil_tool        # Run specific test
ctest --output-on-failure   # Show failures only
\`\`\`

See [DEVELOPMENT.md](DEVELOPMENT.md) for test writing conventions.

## Architecture & Design

See [ARCHITECTURE.md](ARCHITECTURE.md) for module breakdown, data flow, and design patterns.

## Contributing

See [DEVELOPMENT.md](DEVELOPMENT.md) for code style, testing requirements, and PR guidelines.

## API Reference

See [API.md](API.md) for public API documentation of all modules.

## License

[TBD - Add your license here]

## Roadmap

**Fase 1** (Complete): Core canvas, layers, basic tools, binary format, undo/redo
**Fase 2** (Planned): Walker module (animation), Forge module (effects)
**Fase 3+** (Future): Plugin system, hot-reload, advanced tools

---

**Last Updated**: 2026-04-08
