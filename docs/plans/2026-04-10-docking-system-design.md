# Convoy Docking System Design

> **Feature:** Snap System, Default Layout, Layout Switcher, Persistence  
> **Status:** Design  
> **Target:** ImGui-based docking for Convoy workspace

---

## 1. Overview

The Convoy docking system provides a flexible, user-configurable workspace with:
- **Snap Zones** - Visual drop targets when dragging windows
- **Default Layout** - "Artisan" preset for first launch
- **Layout Switcher** - Quick presets (Drawing, Level Design, Debugging)
- **Layout Persistence** - Save/restore from `convoy_layout.ini`

---

## 2. Architecture

### 2.1 Core Components

```
DockSpaceManager
├── ImGuiID root_id_           // Main dock space node
├── ImGuiID zones_[N]          // Pre-allocated zone IDs
├── LayoutPreset current_      // Active preset
├── std::string layout_path_   // Path to .ini file
│
├── build_initial_layout()     // Creates default "Artisan" layout
├── switch_preset(LayoutPreset) // Switch between layouts
├── save_layout()             // Persist to INI
└── load_layout()             // Restore from INI
```

### 2.2 Preset Definitions

| Preset | Description | Zone Distribution |
|--------|-------------|------------------|
| **Artisan** (Default) | Pixel art workflow | Tools: 10%, Canvas: 70%, Inspector: 20%, Bottom: 15% |
| **Level Design** | World map editing | Layers: 25%, Canvas: 50%, Tiles: 25% |
| **Debugging** | Diagnostics focus | Console: 30%, Canvas: 40%, Props: 30% |

---

## 3. Snap Zones Implementation

### 3.1 Drop Target Visualization

When a window is being dragged, ImGui's built-in dock preview shows drop targets. Additional visual enhancement:
- **Blue zones** - Valid drop locations (edges)
- **Green zones** - Suggested/snap locations (corners)
- **Preview rect** - Shows final position before drop

### 3.2 DockBuilder API Usage

```cpp
// Split node - creates new dock zone
ImGuiID right_zone = ImGui::DockBuilderSplitNode(
    main_id,
    ImGuiDir_Right,    // Direction
    0.25f,             // Size ratio (25%)
    nullptr,           // Direction perpendicular (auto)
    &main_id           // Remaining space out-parameter
);

// Dock window to zone
ImGui::DockBuilderDockWindow("Inspector", right_zone);

// Finalize - apply all docks
ImGui::DockBuilderFinish(main_id);
```

### 3.3 Zone Hierarchy

```
Root DockSpace (full viewport)
├── Tools (Left, 10%)
│   └── "Toolbar" window
├── Main (Center, 70%)
│   └── "Architect" / "Walker" / "Sequencer"
├── Inspector (Right, 20%)
│   └── "Properties" + "Layers" stacked
└── Bottom (Down, 15%)
    └── "Console" + "Timeline" stacked
```

---

## 4. Default Layout - "Artisan" Preset

### 4.1 Layout Structure

```text
__________________________________________________________________
| [File][Edit][View][Plugins][Help]                [ Memory: OK ] |
|----------------------------------------------------------------|
| [T] |                                           | [ Inspector ]|
| [O] |          [ Architect: Hero.cvp ]          | Name: Player |
| [O] |-------------------------------------------| Pos: 0,0     |
| [L] |                                           |--------------|
| [S] |              ( CANVAS AREA )              | [ Layers ]   |
| [ ] |                                           | [x] Layer 1  |
| [ ] |             Zoom: 800%  Grid: On          | [ ] Layer 2  |
|-------------------------------------------------|--------------|
| [ Log: Plugin 'CRT_Filter' loaded successfully.              ] |
|________________________________________________________________|
```

### 4.2 Implementation

```cpp
void DockSpaceManager::build_initial_layout(ImGuiID main_dock_id) {
    // Clear existing
    ImGui::DockBuilderRemoveNode(main_dock_id);
    ImGui::DockBuilderAddNode(main_dock_id, ImGuiDockNodeFlags_DockSpace);

    ImGuiID remaining = main_dock_id;

    // Right: Inspector (25%)
    ImGuiID id_right = ImGui::DockBuilderSplitNode(
        remaining, ImGuiDir_Right, 0.25f, nullptr, &remaining);

    // Left: Toolbar (10%)
    ImGuiID id_left = ImGui::DockBuilderSplitNode(
        remaining, ImGuiDir_Left, 0.10f, nullptr, &remaining);

    // Bottom: Console (15%)
    ImGuiID id_bottom = ImGui::DockBuilderSplitNode(
        remaining, ImGuiDir_Down, 0.15f, nullptr, &remaining);

    // Dock windows
    ImGui::DockBuilderDockWindow("Architect", remaining);
    ImGui::DockBuilderDockWindow("Inspector", id_right);
    ImGui::DockBuilderDockWindow("Toolbar", id_left);
    ImGui::DockBuilderDockWindow("Console", id_bottom);

    // Finish
    ImGui::DockBuilderFinish(main_dock_id);

    // Store zone IDs for get_zone_id()
    zone_canvas_ = remaining;
    zone_inspect_ = id_right;
    zone_tools_ = id_left;
    zone_bottom_ = id_bottom;
}
```

---

## 5. Layout Switcher

### 5.1 Menu Integration

Located in menu bar: `View > Layout > [Preset Name]`

Keyboard shortcuts: `F1` (Artisan), `F2` (Level Design), `F3` (Debugging)

### 5.2 Preset Configurations

```cpp
enum class LayoutPreset {
    Artisan,       // Drawing default
    LevelDesign,  // World editor
    Debugging,    // Diagnostics
};

struct LayoutConfig {
    std::vector<std::pair<std::string, ZoneConfig>> windows;
    // ZoneConfig: direction, ratio, split before/after
};
```

---

## 6. Layout Persistence

### 6.1 File Format

ImGui's native `IniFilename` handles most persistence. Additional custom data stored in same file:

```ini
[Window][##ConvoyShell]
Pos=0,0
Size=1600,900
Collapsed=0

[Window][Toolbar]
DockId=1
...
```

### 6.2 Save/Load Functions

```cpp
void DockSpaceManager::save_layout() {
    // ImGui saves automatically to convoy_layout.ini on shutdown
    // Custom: save current preset selection
    save_preset_config(current_preset_);
}

void DockSpaceManager::load_layout() {
    // ImGui loads automatically on startup
    // Custom: restore preset from config
    load_preset_config();
}
```

---

## 7. Module Window Registration

All module windows register on render:

```cpp
void ArchitectUI::render() {
    ImGui::Begin("Tools##Architect");  // Will dock to Tools zone
    // ...
    ImGui::End();

    ImGui::Begin("Canvas##Architect"); // Will dock to Main zone
    // ...
    ImGui::End();
}
```

---

## 8. Edge Cases & Recovery

1. **First launch (no ini):** Build default Artisan layout
2. **Corrupted ini:** Reset to default, warn user
3. **Window not found on restore:** Re-dock to main area
4. **User manually changed layout:** Persist as-is, no override

---

## 9. Success Criteria

- [ ] Windows snap to zones when dragged near edges
- [ ] Artisan preset displays on first launch
- [ ] Layout switcher changes workspace in < 100ms
- [ ] Layout persists across application restarts
- [ ] Keyboard shortcuts F1/F2/F3 switch presets