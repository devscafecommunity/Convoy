# Convoy Frontend Overhaul - Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Transform the Convoy stub shell (ShowDemoWindow) into the full "Dark Terminal" editor with DockSpace, real canvas FBO rendering, all Architect tools, plugin architecture, DOD Visualizer, Live Preview, and module stubs.

**Architecture:** UIComponent interface as base for all panels; WindowManager orchestrates DockSpaceManager + ThemeManager + MenuBar + modules; OpenGL FBO (RenderTexture) for canvas rendering to support future CRT shaders.

**Tech Stack:** C++17, Dear ImGui (DockingEnable + ViewportsEnable already set), GLFW, OpenGL 4.1, GLAD, GoogleTest

**Phase Order:** A (Shell) → B (FBO) → C (Viewport) → D (Tools) → E (Plugins) → F (DOD+Preview) → G (Stubs)

---

## Phase A: Core Shell Foundation

### Task A.1: Dark Terminal Theme Manager

**Files:**
- Create: `src/core/ui/theme_manager.h`
- Create: `src/core/ui/theme_manager.cpp`

**src/core/ui/theme_manager.h:**
```cpp
#pragma once
#include <imgui.h>

namespace convoy {

struct DarkTerminalPalette {
    // #0A0A0A - deep black background
    static constexpr ImVec4 BG_DEEP     = {0.039f, 0.039f, 0.039f, 1.0f};
    // #151515 - panel background
    static constexpr ImVec4 BG_PANEL    = {0.082f, 0.082f, 0.082f, 1.0f};
    // #1F1F1F - widget background
    static constexpr ImVec4 BG_WIDGET   = {0.122f, 0.122f, 0.122f, 1.0f};
    // #2A2A2A - borders
    static constexpr ImVec4 BORDER      = {0.165f, 0.165f, 0.165f, 1.0f};
    // #00FF41 - Matrix green accent
    static constexpr ImVec4 ACCENT      = {0.0f,   1.0f,   0.255f, 1.0f};
    // #FF9000 - warning orange
    static constexpr ImVec4 WARNING     = {1.0f,   0.565f, 0.0f,   1.0f};
    // #E0E0E0 - primary text
    static constexpr ImVec4 TEXT        = {0.878f, 0.878f, 0.878f, 1.0f};
    // #787878 - dimmed text
    static constexpr ImVec4 TEXT_DIM    = {0.471f, 0.471f, 0.471f, 1.0f};
    // Active element (slightly lighter panel)
    static constexpr ImVec4 BG_ACTIVE   = {0.18f,  0.18f,  0.18f,  1.0f};
    // Hovered element
    static constexpr ImVec4 BG_HOVERED  = {0.14f,  0.14f,  0.14f,  1.0f};
};

class ThemeManager {
public:
    static void apply_dark_terminal();
};

} // namespace convoy
```

**src/core/ui/theme_manager.cpp:**
```cpp
#include "theme_manager.h"

namespace convoy {

void ThemeManager::apply_dark_terminal() {
    ImGuiStyle& s = ImGui::GetStyle();
    using P = DarkTerminalPalette;

    s.WindowRounding    = 2.0f;
    s.ChildRounding     = 2.0f;
    s.FrameRounding     = 2.0f;
    s.PopupRounding     = 2.0f;
    s.ScrollbarRounding = 2.0f;
    s.GrabRounding      = 2.0f;
    s.TabRounding       = 2.0f;

    s.WindowBorderSize  = 1.0f;
    s.FrameBorderSize   = 0.0f;
    s.PopupBorderSize   = 1.0f;

    s.WindowPadding     = {8.0f, 6.0f};
    s.FramePadding      = {6.0f, 3.0f};
    s.ItemSpacing       = {6.0f, 4.0f};
    s.ScrollbarSize     = 10.0f;
    s.GrabMinSize       = 8.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]             = P::BG_PANEL;
    c[ImGuiCol_ChildBg]              = P::BG_PANEL;
    c[ImGuiCol_PopupBg]              = P::BG_PANEL;
    c[ImGuiCol_Border]               = P::BORDER;
    c[ImGuiCol_BorderShadow]         = {0,0,0,0};
    c[ImGuiCol_FrameBg]              = P::BG_WIDGET;
    c[ImGuiCol_FrameBgHovered]       = P::BG_HOVERED;
    c[ImGuiCol_FrameBgActive]        = P::BG_ACTIVE;
    c[ImGuiCol_TitleBg]              = P::BG_DEEP;
    c[ImGuiCol_TitleBgActive]        = P::BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed]     = P::BG_DEEP;
    c[ImGuiCol_MenuBarBg]            = P::BG_DEEP;
    c[ImGuiCol_ScrollbarBg]          = P::BG_DEEP;
    c[ImGuiCol_ScrollbarGrab]        = P::BORDER;
    c[ImGuiCol_ScrollbarGrabHovered] = P::BG_ACTIVE;
    c[ImGuiCol_ScrollbarGrabActive]  = P::ACCENT;
    c[ImGuiCol_CheckMark]            = P::ACCENT;
    c[ImGuiCol_SliderGrab]           = P::ACCENT;
    c[ImGuiCol_SliderGrabActive]     = P::ACCENT;
    c[ImGuiCol_Button]               = P::BG_WIDGET;
    c[ImGuiCol_ButtonHovered]        = P::BG_HOVERED;
    c[ImGuiCol_ButtonActive]         = P::BG_ACTIVE;
    c[ImGuiCol_Header]               = P::BG_WIDGET;
    c[ImGuiCol_HeaderHovered]        = P::BG_HOVERED;
    c[ImGuiCol_HeaderActive]         = P::BG_ACTIVE;
    c[ImGuiCol_Separator]            = P::BORDER;
    c[ImGuiCol_SeparatorHovered]     = P::ACCENT;
    c[ImGuiCol_SeparatorActive]      = P::ACCENT;
    c[ImGuiCol_ResizeGrip]           = P::BG_WIDGET;
    c[ImGuiCol_ResizeGripHovered]    = P::ACCENT;
    c[ImGuiCol_ResizeGripActive]     = P::ACCENT;
    c[ImGuiCol_Tab]                  = P::BG_DEEP;
    c[ImGuiCol_TabHovered]           = P::BG_ACTIVE;
    c[ImGuiCol_TabActive]            = P::BG_PANEL;
    c[ImGuiCol_TabUnfocused]         = P::BG_DEEP;
    c[ImGuiCol_TabUnfocusedActive]   = P::BG_PANEL;
    c[ImGuiCol_DockingPreview]       = {P::ACCENT.x, P::ACCENT.y, P::ACCENT.z, 0.3f};
    c[ImGuiCol_DockingEmptyBg]       = P::BG_DEEP;
    c[ImGuiCol_PlotLines]            = P::ACCENT;
    c[ImGuiCol_PlotHistogram]        = P::ACCENT;
    c[ImGuiCol_TableHeaderBg]        = P::BG_DEEP;
    c[ImGuiCol_TableBorderStrong]    = P::BORDER;
    c[ImGuiCol_TableBorderLight]     = P::BG_WIDGET;
    c[ImGuiCol_Text]                 = P::TEXT;
    c[ImGuiCol_TextDisabled]         = P::TEXT_DIM;
    c[ImGuiCol_NavHighlight]         = P::ACCENT;
    c[ImGuiCol_ModalWindowDimBg]     = {0.0f, 0.0f, 0.0f, 0.6f};

    // Match OS windows to ImGui style when viewports enabled
    s.Colors[ImGuiCol_WindowBg].w = 1.0f;
}

} // namespace convoy
```

**Verify:** Add `#include "core/ui/theme_manager.h"` + `ThemeManager::apply_dark_terminal()` call in window_manager.cpp after `ImGui::CreateContext()`. Build confirms no errors.

---

### Task A.2: DockSpace Manager

**Files:**
- Create: `src/core/ui/dockspace_manager.h`
- Create: `src/core/ui/dockspace_manager.cpp`

**src/core/ui/dockspace_manager.h:**
```cpp
#pragma once
#include <imgui.h>

namespace convoy {

enum class DockZone {
    ToolPalette,  // Left slim bar  (~8% width)
    MainCanvas,   // Center          (~68%)
    Inspector,    // Right panel     (~24%)
};

class DockSpaceManager {
public:
    // Call after ImGui::NewFrame(), before module windows
    void begin_frame();
    // Call after all module ImGui::Begin/End pairs
    void end_frame();

    ImGuiID get_zone_id(DockZone zone) const;
    bool is_initialized() const { return initialized_; }

private:
    void build_initial_layout(ImGuiID root_id);

    ImGuiID root_id_      = 0;
    ImGuiID zone_tools_   = 0;
    ImGuiID zone_canvas_  = 0;
    ImGuiID zone_inspect_ = 0;
    bool    initialized_  = false;
};

} // namespace convoy
```

**src/core/ui/dockspace_manager.cpp:**
```cpp
#include "dockspace_manager.h"
#include <imgui_internal.h>

namespace convoy {

void DockSpaceManager::begin_frame() {
    ImGuiViewport* vp = ImGui::GetMainViewport();

    float menu_h = ImGui::GetFrameHeight();
    ImVec2 pos  = {vp->WorkPos.x,              vp->WorkPos.y + menu_h};
    ImVec2 size = {vp->WorkSize.x,             vp->WorkSize.y - menu_h};

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar       | ImGuiWindowFlags_NoCollapse  |
        ImGuiWindowFlags_NoResize         | ImGuiWindowFlags_NoMove      |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking        | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0, 0));
    ImGui::Begin("##ConvoyShell", nullptr, flags);
    ImGui::PopStyleVar(3);

    root_id_ = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(root_id_, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

    if (!initialized_) {
        build_initial_layout(root_id_);
        initialized_ = true;
    }
}

void DockSpaceManager::end_frame() {
    ImGui::End();
}

ImGuiID DockSpaceManager::get_zone_id(DockZone zone) const {
    switch (zone) {
        case DockZone::ToolPalette: return zone_tools_;
        case DockZone::MainCanvas:  return zone_canvas_;
        case DockZone::Inspector:   return zone_inspect_;
    }
    return root_id_;
}

void DockSpaceManager::build_initial_layout(ImGuiID root_id) {
    ImGui::DockBuilderRemoveNode(root_id);
    ImGui::DockBuilderAddNode(root_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(root_id, ImGui::GetMainViewport()->WorkSize);

    ImGuiID remaining = root_id;

    // Carve left slim panel (Tool Palette)
    ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Left, 0.08f, &zone_tools_, &remaining);
    // Carve right inspector panel
    ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.26f, &zone_inspect_, &remaining);
    // What remains is the main canvas
    zone_canvas_ = remaining;

    ImGui::DockBuilderFinish(root_id);
}

} // namespace convoy
```

---

### Task A.3: Main Menu Bar

**Files:**
- Create: `src/core/ui/main_menu_bar.h`
- Create: `src/core/ui/main_menu_bar.cpp`

**src/core/ui/main_menu_bar.h:**
```cpp
#pragma once
#include <functional>
#include <string>

namespace convoy {

struct MenuBarCallbacks {
    std::function<void()> on_new_project;
    std::function<void()> on_open_project;
    std::function<void()> on_save_project;
    std::function<void()> on_export;
    std::function<void()> on_undo;
    std::function<void()> on_redo;
    std::function<void(const std::string& module)> on_module_switch;
    std::function<void()> on_preferences;
};

class MainMenuBar {
public:
    void set_callbacks(const MenuBarCallbacks& cb) { cb_ = cb; }
    void render(bool can_undo, bool can_redo,
                const std::string& project_name,
                float memory_mb, float memory_max_mb);

private:
    MenuBarCallbacks cb_;

    void render_file_menu();
    void render_edit_menu(bool can_undo, bool can_redo);
    void render_modules_menu();
    void render_view_menu();
    void render_status_right(const std::string& project_name,
                             float mem_mb, float mem_max);
};

} // namespace convoy
```

**src/core/ui/main_menu_bar.cpp:**
```cpp
#include "main_menu_bar.h"
#include "theme_manager.h"
#include <imgui.h>
#include <cstdio>

namespace convoy {

void MainMenuBar::render(bool can_undo, bool can_redo,
                         const std::string& project_name,
                         float memory_mb, float memory_max_mb) {
    if (ImGui::BeginMainMenuBar()) {
        render_file_menu();
        render_edit_menu(can_undo, can_redo);
        render_modules_menu();
        render_view_menu();
        render_status_right(project_name, memory_mb, memory_max_mb);
        ImGui::EndMainMenuBar();
    }
}

void MainMenuBar::render_file_menu() {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Project",  "Ctrl+N") && cb_.on_new_project)  cb_.on_new_project();
        if (ImGui::MenuItem("Open...",      "Ctrl+O") && cb_.on_open_project) cb_.on_open_project();
        ImGui::Separator();
        if (ImGui::MenuItem("Save",         "Ctrl+S") && cb_.on_save_project) cb_.on_save_project();
        ImGui::Separator();
        if (ImGui::MenuItem("Deploy (.caf)","Ctrl+D") && cb_.on_export)       cb_.on_export();
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_edit_menu(bool can_undo, bool can_redo) {
    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, can_undo) && cb_.on_undo) cb_.on_undo();
        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, can_redo) && cb_.on_redo) cb_.on_redo();
        ImGui::Separator();
        if (ImGui::MenuItem("Preferences") && cb_.on_preferences) cb_.on_preferences();
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_modules_menu() {
    if (ImGui::BeginMenu("Modules")) {
        if (ImGui::MenuItem("Architect (Pixel)",    "F1") && cb_.on_module_switch) cb_.on_module_switch("architect");
        if (ImGui::MenuItem("Forge (Atlas)",        "F2") && cb_.on_module_switch) cb_.on_module_switch("forge");
        if (ImGui::MenuItem("Sequencer (Anim)",     "F3") && cb_.on_module_switch) cb_.on_module_switch("sequencer");
        if (ImGui::MenuItem("Walker (World)",       "F4") && cb_.on_module_switch) cb_.on_module_switch("walker");
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_view_menu() {
    if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Toggle Grid",          "G");
        ImGui::MenuItem("Collision Overlay",    "C");
        ImGui::MenuItem("DOD Visualizer",       nullptr);
        ImGui::EndMenu();
    }
}

void MainMenuBar::render_status_right(const std::string& project_name,
                                      float mem_mb, float mem_max) {
    // Right-align: project name + memory indicator
    char buf[128];
    snprintf(buf, sizeof(buf), "[ %s ]   Mem: %.0f / %.0f MB",
             project_name.c_str(), mem_mb, mem_max);

    float text_w = ImGui::CalcTextSize(buf).x;
    float bar_w  = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + bar_w - text_w - 8.0f);

    // Warn in orange if > 80% memory
    if (mem_mb / mem_max > 0.8f)
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::WARNING);
    else
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::TEXT_DIM);

    ImGui::Text("%s", buf);
    ImGui::PopStyleColor();
}

} // namespace convoy
```

---

### Task A.4: Input Handler

**Files:**
- Create: `src/core/input/input_handler.h`
- Create: `src/core/input/input_handler.cpp`

**src/core/input/input_handler.h:**
```cpp
#pragma once
#include <functional>
#include <unordered_map>
#include <string>

namespace convoy {

struct KeyBinding {
    int key;
    int mods; // GLFW_MOD_CONTROL | GLFW_MOD_SHIFT etc.
};

class InputHandler {
public:
    using CommandFn = std::function<void()>;

    void bind(const std::string& command_id, int key, int mods = 0);
    void register_command(const std::string& command_id, CommandFn fn);
    void process_key(int key, int action, int mods);
    void setup_defaults();

private:
    std::string find_command(int key, int mods) const;

    std::unordered_map<std::string, KeyBinding>  bindings_;
    std::unordered_map<std::string, CommandFn>   commands_;
};

} // namespace convoy
```

**src/core/input/input_handler.cpp:**
```cpp
#include "input_handler.h"
#include <GLFW/glfw3.h>

namespace convoy {

void InputHandler::bind(const std::string& id, int key, int mods) {
    bindings_[id] = {key, mods};
}

void InputHandler::register_command(const std::string& id, CommandFn fn) {
    commands_[id] = std::move(fn);
}

void InputHandler::process_key(int key, int action, int mods) {
    if (action != GLFW_PRESS) return;
    std::string id = find_command(key, mods);
    if (id.empty()) return;
    auto it = commands_.find(id);
    if (it != commands_.end()) it->second();
}

void InputHandler::setup_defaults() {
    // Tool shortcuts
    bind("tool.pencil",   GLFW_KEY_B);
    bind("tool.eraser",   GLFW_KEY_E);
    bind("tool.bucket",   GLFW_KEY_G);
    bind("tool.pivot",    GLFW_KEY_V);
    bind("tool.hitbox",   GLFW_KEY_H);
    // Module shortcuts
    bind("module.architect",  GLFW_KEY_F1);
    bind("module.forge",      GLFW_KEY_F2);
    bind("module.sequencer",  GLFW_KEY_F3);
    bind("module.walker",     GLFW_KEY_F4);
    // Project shortcuts
    bind("project.save",   GLFW_KEY_S, GLFW_MOD_CONTROL);
    bind("project.new",    GLFW_KEY_N, GLFW_MOD_CONTROL);
    bind("project.open",   GLFW_KEY_O, GLFW_MOD_CONTROL);
    bind("project.export", GLFW_KEY_D, GLFW_MOD_CONTROL);
    bind("edit.undo",      GLFW_KEY_Z, GLFW_MOD_CONTROL);
    bind("edit.redo",      GLFW_KEY_Y, GLFW_MOD_CONTROL);
}

std::string InputHandler::find_command(int key, int mods) const {
    for (const auto& [id, binding] : bindings_) {
        if (binding.key == key && binding.mods == mods)
            return id;
    }
    return {};
}

} // namespace convoy
```

**Test (tests/test_input_handler.cpp):**
```cpp
#include <gtest/gtest.h>
#include "core/input/input_handler.h"
#include <GLFW/glfw3.h>

using namespace convoy;

TEST(InputHandlerTest, BindAndFire) {
    InputHandler handler;
    bool fired = false;
    handler.bind("tool.pencil", GLFW_KEY_B);
    handler.register_command("tool.pencil", [&]() { fired = true; });
    handler.process_key(GLFW_KEY_B, GLFW_PRESS, 0);
    EXPECT_TRUE(fired);
}

TEST(InputHandlerTest, ModifierRequirement) {
    InputHandler handler;
    bool fired = false;
    handler.bind("project.save", GLFW_KEY_S, GLFW_MOD_CONTROL);
    handler.register_command("project.save", [&]() { fired = true; });
    handler.process_key(GLFW_KEY_S, GLFW_PRESS, 0);
    EXPECT_FALSE(fired);
    handler.process_key(GLFW_KEY_S, GLFW_PRESS, GLFW_MOD_CONTROL);
    EXPECT_TRUE(fired);
}

TEST(InputHandlerTest, ReleaseIgnored) {
    InputHandler handler;
    bool fired = false;
    handler.bind("tool.eraser", GLFW_KEY_E);
    handler.register_command("tool.eraser", [&]() { fired = true; });
    handler.process_key(GLFW_KEY_E, GLFW_RELEASE, 0);
    EXPECT_FALSE(fired);
}

TEST(InputHandlerTest, SetupDefaults) {
    InputHandler handler;
    handler.setup_defaults();
    // defaults registered without crashing
    SUCCEED();
}
```

---

### Task A.5: Integrate Shell into WindowManager

**Files:**
- Modify: `src/core/window_manager.h`
- Modify: `src/core/window_manager.cpp`
- Modify: `CMakeLists.txt`

**Replace window_manager.h with:**
```cpp
#pragma once
#include "core/ui/theme_manager.h"
#include "core/ui/dockspace_manager.h"
#include "core/ui/main_menu_bar.h"
#include "core/input/input_handler.h"
#include "command_manager.h"
#include "modules/mod_architect/architect_ui.h"
#include "modules/mod_architect/canvas.h"
#include <string>

struct GLFWwindow;

namespace convoy {

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    void initialize(const std::string& title, int width, int height);
    void run_loop();
    void shutdown();
    bool should_close() const;

private:
    static void glfw_error_callback(int error, const char* description);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode,
                                  int action, int mods);

    void render_frame();
    void setup_input_commands();

    GLFWwindow*        window_      = nullptr;
    bool               initialized_ = false;

    DockSpaceManager   dockspace_;
    MainMenuBar        menubar_;
    InputHandler       input_;
    CommandManager     cmd_mgr_;

    // Architect module (Fase 1)
    architect::Canvas    canvas_;
    architect::ArchitectUI architect_ui_;

    std::string project_name_ = "Untitled";
};

} // namespace convoy
```

**Replace window_manager.cpp with:**
```cpp
#include "window_manager.h"
#include "logger.h"
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdexcept>

namespace convoy {

WindowManager::WindowManager() : canvas_(64, 64) {}

WindowManager::~WindowManager() {
    if (initialized_) shutdown();
}

void WindowManager::glfw_error_callback(int error, const char* description) {
    Logger::error("GLFW Error {}: {}", error, description);
}

void WindowManager::glfw_key_callback(GLFWwindow* window, int key, int /*sc*/,
                                      int action, int mods) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self) self->input_.process_key(key, action, mods);
}

void WindowManager::initialize(const std::string& title, int width, int height) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) { glfwTerminate(); throw std::runtime_error("Failed to create window"); }

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, glfw_key_callback);
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = "convoy_layout.ini";

    ThemeManager::apply_dark_terminal();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // Wire up input commands
    input_.setup_defaults();
    setup_input_commands();

    // Initialize Architect module
    architect_ui_.initialize(&canvas_, &cmd_mgr_);

    initialized_ = true;
    Logger::info("WindowManager initialized {}x{}", width, height);
}

void WindowManager::setup_input_commands() {
    input_.register_command("edit.undo", [this]() { cmd_mgr_.undo(); });
    input_.register_command("edit.redo", [this]() { cmd_mgr_.redo(); });
    // Tool commands delegated to ArchitectUI via tool type
    input_.register_command("tool.pencil",  [this]() { architect_ui_.set_tool(ToolType::Pencil); });
    input_.register_command("tool.eraser",  [this]() { architect_ui_.set_tool(ToolType::Eraser); });
    input_.register_command("tool.bucket",  [this]() { architect_ui_.set_tool(ToolType::Bucket); });
    input_.register_command("tool.pivot",   [this]() { architect_ui_.set_tool(ToolType::Pivot); });
    input_.register_command("tool.hitbox",  [this]() { architect_ui_.set_tool(ToolType::Hitbox); });
}

void WindowManager::render_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Menu bar (fixed top)
    menubar_.render(cmd_mgr_.can_undo(), cmd_mgr_.can_redo(),
                    project_name_, 16.0f, 128.0f);

    // DockSpace shell (fills rest of window)
    dockspace_.begin_frame();
    dockspace_.end_frame();

    // Module panels (dock into zones automatically on first run)
    architect_ui_.render();

    ImGui::Render();

    int fw, fh;
    glfwGetFramebufferSize(window_, &fw, &fh);
    glViewport(0, 0, fw, fh);
    glClearColor(0.039f, 0.039f, 0.039f, 1.0f); // BG_DEEP
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup);
    }
}

void WindowManager::run_loop() {
    while (!should_close()) {
        glfwPollEvents();
        render_frame();
        glfwSwapBuffers(window_);
    }
}

bool WindowManager::should_close() const {
    return !window_ || glfwWindowShouldClose(window_);
}

void WindowManager::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (window_) { glfwDestroyWindow(window_); window_ = nullptr; }
    glfwTerminate();
    initialized_ = false;
    Logger::info("WindowManager shutdown");
}

} // namespace convoy
```

**Update CMakeLists.txt - add new UI sources to graphics conditional block:**
```cmake
if(GLFW3_FOUND AND OpenGL_FOUND AND GLAD_FOUND AND ImGui_FOUND)
  target_sources(convoy_core PRIVATE
    src/core/window_manager.cpp
    src/core/event_bus.cpp
    src/core/ui/theme_manager.cpp
    src/core/ui/dockspace_manager.cpp
    src/core/ui/main_menu_bar.cpp
    src/core/input/input_handler.cpp
  )
  ...
```

**Also add ToolType::Pivot and ToolType::Hitbox to shared/types.h:**
```cpp
enum class ToolType {
    Pencil,
    Eraser,
    Bucket,
    ColorPicker,
    Move,
    Pivot,   // NEW
    Hitbox,  // NEW
};
```

**Verify:** `cmake --build build` exits 0. Running convoy shows Dark Terminal UI with menu bar and empty docked panels.

---

## Phase B: Canvas FBO Renderer

### Task B.1: RenderTexture (OpenGL FBO Wrapper)

**Files:**
- Create: `src/core/gfx/render_texture.h`
- Create: `src/core/gfx/render_texture.cpp`

**src/core/gfx/render_texture.h:**
```cpp
#pragma once
#include <cstdint>

namespace convoy {

class RenderTexture {
public:
    RenderTexture() = default;
    ~RenderTexture() { destroy(); }

    RenderTexture(const RenderTexture&)            = delete;
    RenderTexture& operator=(const RenderTexture&) = delete;

    void create(int width, int height);
    void resize(int width, int height);
    void destroy();

    void bind();    // Render INTO this texture
    void unbind();  // Restore default framebuffer

    uint32_t texture_id() const { return texture_id_; }
    int width()    const { return width_; }
    int height()   const { return height_; }
    bool is_valid() const { return fbo_id_ != 0; }

private:
    uint32_t fbo_id_     = 0;
    uint32_t texture_id_ = 0;
    uint32_t rbo_id_     = 0;
    int      width_      = 0;
    int      height_     = 0;
};

} // namespace convoy
```

**src/core/gfx/render_texture.cpp:**
```cpp
#include "render_texture.h"
#include <glad/glad.h>

namespace convoy {

void RenderTexture::create(int width, int height) {
    destroy();
    width_  = width;
    height_ = height;

    glGenFramebuffers(1, &fbo_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

    // Color attachment (RGBA8)
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture_id_, 0);

    // Depth/stencil renderbuffer
    glGenRenderbuffers(1, &rbo_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_id_);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::resize(int width, int height) {
    if (width == width_ && height == height_) return;
    create(width, height);
}

void RenderTexture::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
    glViewport(0, 0, width_, height_);
}

void RenderTexture::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::destroy() {
    if (rbo_id_)     { glDeleteRenderbuffers(1, &rbo_id_);    rbo_id_     = 0; }
    if (texture_id_) { glDeleteTextures(1, &texture_id_);     texture_id_ = 0; }
    if (fbo_id_)     { glDeleteFramebuffers(1, &fbo_id_);     fbo_id_     = 0; }
    width_ = height_ = 0;
}

} // namespace convoy
```

---

### Task B.2: Real Canvas Rendering in ArchitectUI

The canvas needs a GPU texture that is updated when pixels change, displayed via `ImGui::Image`.

**Files:**
- Modify: `src/modules/mod_architect/architect_ui.h`
- Modify: `src/modules/mod_architect/architect_ui.cpp`
- Modify: `CMakeLists.txt` (add render_texture.cpp to convoy_core graphics block)

**Updated architect_ui.h:**
```cpp
#pragma once
#include "viewport.h"
#include "canvas.h"
#include "tools/tool_base.h"
#include "../../core/command_manager.h"
#include "../../core/gfx/render_texture.h"
#include "../../shared/types.h"
#include <memory>
#include <cstdint>

struct GLFWwindow;

namespace convoy {
namespace architect {

class ArchitectUI {
public:
    ArchitectUI();
    ~ArchitectUI();

    void initialize(Canvas* canvas, CommandManager* cmd_mgr);
    void render();
    void set_tool(ToolType type);
    Tool* get_current_tool() const { return current_tool_.get(); }

private:
    void render_toolbar();
    void render_canvas_viewport();
    void render_layers_panel();
    void render_properties_panel();
    void handle_canvas_input(ImVec2 canvas_origin, ImVec2 canvas_size);

    void upload_canvas_texture();
    void ensure_canvas_texture();

    Canvas*         canvas_{nullptr};
    CommandManager* cmd_mgr_{nullptr};
    Viewport        viewport_;
    std::unique_ptr<Tool> current_tool_;
    ToolType        current_tool_type_{ToolType::Pencil};
    Color           current_color_{255, 255, 255, 255};

    // GPU canvas texture (updated on pixel change)
    uint32_t canvas_tex_id_  = 0;
    bool     canvas_dirty_   = true;

    // Interaction state
    bool is_painting_    = false;
    bool is_panning_     = false;
    int  last_mouse_x_   = 0;
    int  last_mouse_y_   = 0;
    float pan_start_x_   = 0.0f;
    float pan_start_y_   = 0.0f;

    // Grid
    bool show_grid_ = true;

    // Pivot + hitbox state
    Vec2 pivot_point_{0.0f, 0.0f};
    Rect hitbox_{0.0f, 0.0f, 0.0f, 0.0f};
    bool hitbox_defined_ = false;
};

} // namespace architect
} // namespace convoy
```

**Updated architect_ui.cpp - key sections (render_canvas_viewport and helpers):**
```cpp
#include "architect_ui.h"
#include "tools/pencil_tool.h"
#include "tools/eraser_tool.h"
#include "tools/bucket_tool.h"
#include "tools/pivot_tool.h"
#include "tools/hitbox_tool.h"
#include <imgui.h>
#include <glad/glad.h>
#include <string>
#include <vector>

namespace convoy {
namespace architect {

ArchitectUI::ArchitectUI() : viewport_(800, 600) {
    set_tool(ToolType::Pencil);
}

ArchitectUI::~ArchitectUI() {
    if (canvas_tex_id_) glDeleteTextures(1, &canvas_tex_id_);
}

void ArchitectUI::initialize(Canvas* canvas, CommandManager* cmd_mgr) {
    canvas_   = canvas;
    cmd_mgr_  = cmd_mgr;
    viewport_.fit_to_canvas(canvas_);
    canvas_dirty_ = true;
}

void ArchitectUI::set_tool(ToolType type) {
    current_tool_type_ = type;
    switch (type) {
        case ToolType::Pencil:  current_tool_ = std::make_unique<PencilTool>();  break;
        case ToolType::Eraser:  current_tool_ = std::make_unique<EraserTool>();  break;
        case ToolType::Bucket:  current_tool_ = std::make_unique<BucketTool>();  break;
        case ToolType::Pivot:   current_tool_ = std::make_unique<PivotTool>();   break;
        case ToolType::Hitbox:  current_tool_ = std::make_unique<HitboxTool>();  break;
        default:                current_tool_ = std::make_unique<PencilTool>();  break;
    }
    if (current_tool_) current_tool_->set_foreground_color(current_color_);
}

void ArchitectUI::ensure_canvas_texture() {
    if (canvas_tex_id_ != 0) return;
    glGenTextures(1, &canvas_tex_id_);
    glBindTexture(GL_TEXTURE_2D, canvas_tex_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ArchitectUI::upload_canvas_texture() {
    if (!canvas_ || !canvas_dirty_) return;
    ensure_canvas_texture();

    std::vector<uint32_t> pixels;
    canvas_->composite_to_texture(pixels);

    glBindTexture(GL_TEXTURE_2D, canvas_tex_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<int>(canvas_->width()),
                 static_cast<int>(canvas_->height()),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    canvas_dirty_ = false;
}

void ArchitectUI::render() {
    upload_canvas_texture();
    render_toolbar();
    render_canvas_viewport();
    render_layers_panel();
    render_properties_panel();
}

void ArchitectUI::render_toolbar() {
    ImGui::Begin("Tools##Architect");

    float col[4] = {
        current_color_.r / 255.0f, current_color_.g / 255.0f,
        current_color_.b / 255.0f, current_color_.a / 255.0f
    };
    if (ImGui::ColorEdit4("##Color", col,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        current_color_ = Color(
            static_cast<uint8_t>(col[0] * 255),
            static_cast<uint8_t>(col[1] * 255),
            static_cast<uint8_t>(col[2] * 255),
            static_cast<uint8_t>(col[3] * 255));
        if (current_tool_) current_tool_->set_foreground_color(current_color_);
    }
    ImGui::Separator();

    auto tool_btn = [&](const char* label, ToolType type) {
        bool active = (current_tool_type_ == type);
        if (active) ImGui::PushStyleColor(ImGuiCol_Button,
                        {0.0f, 1.0f, 0.255f, 0.3f}); // accent tint
        if (ImGui::Button(label, {-1, 0})) set_tool(type);
        if (active) ImGui::PopStyleColor();
    };

    tool_btn("[P] Pencil",  ToolType::Pencil);
    tool_btn("[E] Eraser",  ToolType::Eraser);
    tool_btn("[B] Bucket",  ToolType::Bucket);
    ImGui::Separator();
    tool_btn("[V] Pivot",   ToolType::Pivot);
    tool_btn("[H] Hitbox",  ToolType::Hitbox);
    ImGui::Separator();
    ImGui::Checkbox("Grid", &show_grid_);

    ImGui::End();
}

void ArchitectUI::render_canvas_viewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Canvas##Architect");
    ImGui::PopStyleVar();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    viewport_.resize(avail.x, avail.y);

    ImVec2 origin = ImGui::GetCursorScreenPos();

    // ---- Draw canvas image ----
    if (canvas_tex_id_ && canvas_) {
        float cw = static_cast<float>(canvas_->width());
        float ch = static_cast<float>(canvas_->height());
        Vec2 tl = viewport_.canvas_to_screen(0.0f, 0.0f);
        Vec2 br = viewport_.canvas_to_screen(cw, ch);

        ImVec2 img_tl = {origin.x + tl.x, origin.y + tl.y};
        ImVec2 img_br = {origin.x + br.x, origin.y + br.y};

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(intptr_t)canvas_tex_id_,
            img_tl, img_br);

        // ---- Grid overlay ----
        if (show_grid_ && viewport_.get_zoom() >= 4.0f) {
            float zoom = viewport_.get_zoom();
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImU32 grid_col = IM_COL32(42, 42, 42, 200);

            // Determine step: 1px at 4-8x, 2px at 2-4x etc.
            float step_canvas = 1.0f;
            if (zoom < 8.0f) step_canvas = 2.0f;
            if (zoom < 4.0f) step_canvas = 4.0f;

            float step_screen = step_canvas * zoom;
            float start_x = origin.x + tl.x;
            float start_y = origin.y + tl.y;
            float end_x   = origin.x + br.x;
            float end_y   = origin.y + br.y;

            for (float x = start_x; x <= end_x; x += step_screen)
                dl->AddLine({x, start_y}, {x, end_y}, grid_col);
            for (float y = start_y; y <= end_y; y += step_screen)
                dl->AddLine({start_x, y}, {end_x, y}, grid_col);
        }

        // ---- Pivot crosshair ----
        if (current_tool_type_ == ToolType::Pivot || hitbox_defined_) {
            Vec2 pv = viewport_.canvas_to_screen(pivot_point_.x, pivot_point_.y);
            ImVec2 pscreen = {origin.x + pv.x, origin.y + pv.y};
            float sz = 8.0f;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddLine({pscreen.x - sz, pscreen.y}, {pscreen.x + sz, pscreen.y},
                        IM_COL32(0, 255, 65, 255), 1.5f);
            dl->AddLine({pscreen.x, pscreen.y - sz}, {pscreen.x, pscreen.y + sz},
                        IM_COL32(0, 255, 65, 255), 1.5f);
            dl->AddCircle(pscreen, 4.0f, IM_COL32(0, 255, 65, 255));
        }

        // ---- Hitbox rect ----
        if (hitbox_defined_) {
            Vec2 htl = viewport_.canvas_to_screen(hitbox_.x, hitbox_.y);
            Vec2 hbr = viewport_.canvas_to_screen(hitbox_.x + hitbox_.w,
                                                   hitbox_.y + hitbox_.h);
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRect({origin.x + htl.x, origin.y + htl.y},
                        {origin.x + hbr.x, origin.y + hbr.y},
                        IM_COL32(255, 144, 0, 200), 0.0f, 0, 1.5f);
        }
    }

    // Invisible button to capture mouse events on the canvas area
    ImGui::SetCursorScreenPos(origin);
    ImGui::InvisibleButton("##canvas_input", avail);
    handle_canvas_input(origin, avail);

    ImGui::End();
}

void ArchitectUI::handle_canvas_input(ImVec2 canvas_origin, ImVec2 /*canvas_size*/) {
    if (!canvas_ || !current_tool_) return;

    ImGuiIO& io = ImGui::GetIO();

    // ---- Zoom (scroll wheel, zoom towards cursor) ----
    if (ImGui::IsItemHovered()) {
        if (io.MouseWheel != 0.0f) {
            ImVec2 mouse = ImGui::GetMousePos();
            float local_x = mouse.x - canvas_origin.x;
            float local_y = mouse.y - canvas_origin.y;
            float factor = (io.MouseWheel > 0) ? 1.15f : (1.0f / 1.15f);
            viewport_.zoom_to_point(local_x, local_y, factor);
        }
    }

    // ---- Pan (middle mouse or Alt+LMB) ----
    bool alt_held = (io.KeyAlt);
    bool mid_down = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    bool pan_active = mid_down || (alt_held && ImGui::IsMouseDown(ImGuiMouseButton_Left));

    if (ImGui::IsItemHovered() && pan_active && !is_panning_) {
        is_panning_   = true;
        pan_start_x_  = io.MousePos.x;
        pan_start_y_  = io.MousePos.y;
    }
    if (is_panning_) {
        if (pan_active) {
            viewport_.pan(io.MouseDelta.x, io.MouseDelta.y);
        } else {
            is_panning_ = false;
        }
        return; // Skip draw tools while panning
    }

    // ---- Drawing tools ----
    if (!ImGui::IsItemHovered() && !is_painting_) return;
    if (alt_held) return;

    ImVec2 mouse = ImGui::GetMousePos();
    float local_x = mouse.x - canvas_origin.x;
    float local_y = mouse.y - canvas_origin.y;
    Vec2 cc = viewport_.screen_to_canvas(local_x, local_y);
    int cx = static_cast<int>(cc.x);
    int cy = static_cast<int>(cc.y);

    bool lmb = ImGui::IsMouseDown(ImGuiMouseButton_Left);

    if (lmb && !is_painting_) {
        is_painting_  = true;
        last_mouse_x_ = cx;
        last_mouse_y_ = cy;
        current_tool_->on_mouse_down(canvas_, cx, cy);
        canvas_dirty_ = true;

        // Capture pivot/hitbox after tool actions
        if (current_tool_type_ == ToolType::Pivot) {
            pivot_point_ = {static_cast<float>(cx), static_cast<float>(cy)};
        }
    } else if (lmb && is_painting_) {
        if (cx != last_mouse_x_ || cy != last_mouse_y_) {
            current_tool_->on_mouse_drag(canvas_, last_mouse_x_, last_mouse_y_, cx, cy);
            last_mouse_x_ = cx;
            last_mouse_y_ = cy;
            canvas_dirty_ = true;

            if (current_tool_type_ == ToolType::Hitbox) {
                if (auto* ht = dynamic_cast<HitboxTool*>(current_tool_.get()))
                    hitbox_ = ht->get_hitbox();
            }
        }
    } else if (!lmb && is_painting_) {
        is_painting_ = false;
        current_tool_->on_mouse_up(canvas_, cx, cy);
        canvas_dirty_ = true;

        if (current_tool_type_ == ToolType::Hitbox) {
            if (auto* ht = dynamic_cast<HitboxTool*>(current_tool_.get())) {
                hitbox_         = ht->get_hitbox();
                hitbox_defined_ = (hitbox_.w > 0 && hitbox_.h > 0);
            }
        }
    }
}

void ArchitectUI::render_layers_panel() {
    ImGui::Begin("Layers##Architect");
    if (canvas_) {
        auto& layers = canvas_->get_layers();
        for (size_t i = 0; i < layers.size(); ++i) {
            auto& layer    = layers[i];
            bool is_active = (canvas_->get_active_layer() == &layer);
            if (ImGui::Selectable(layer.name.c_str(), is_active))
                canvas_->set_active_layer(static_cast<int>(i));
            ImGui::SameLine();
            bool vis = layer.visible;
            if (ImGui::Checkbox(("##V" + std::to_string(i)).c_str(), &vis))
                layer.visible = vis;
        }
        ImGui::Separator();
        if (ImGui::SmallButton("+##addlayer")) {
            canvas_->add_layer("Layer " + std::to_string(layers.size() + 1));
        }
    }
    ImGui::End();
}

void ArchitectUI::render_properties_panel() {
    ImGui::Begin("Properties##Architect");
    if (canvas_) {
        ImGui::TextDisabled("Canvas");
        ImGui::Text("%d x %d px", canvas_->width(), canvas_->height());
        ImGui::Separator();
        ImGui::TextDisabled("Viewport");
        float zoom = viewport_.get_zoom();
        if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 32.0f, "%.1fx"))
            viewport_.set_zoom(zoom);
        ImGui::Separator();
        ImGui::TextDisabled("Metadata");
        ImGui::Text("Pivot: (%.0f, %.0f)", pivot_point_.x, pivot_point_.y);
        if (hitbox_defined_)
            ImGui::Text("Hitbox: %.0f,%.0f  %.0fx%.0f",
                        hitbox_.x, hitbox_.y, hitbox_.w, hitbox_.h);
        else
            ImGui::TextDisabled("No hitbox defined");
    }
    ImGui::End();
}

} // namespace architect
} // namespace convoy
```

---

## Phase C: Viewport Interactivity

### Task C.1: zoom_to_point in Viewport

The scroll zoom must zoom towards the cursor, not the canvas center.

**Files:**
- Modify: `src/modules/mod_architect/viewport.h`
- Modify: `src/modules/mod_architect/viewport.cpp`

**Add to viewport.h:**
```cpp
void resize(float width, float height);
void zoom_to_point(float screen_x, float screen_y, float factor);
```

**Add to viewport.cpp:**
```cpp
void Viewport::resize(float width, float height) {
    width_  = width;
    height_ = height;
}

void Viewport::zoom_to_point(float screen_x, float screen_y, float factor) {
    // Convert screen point to canvas before zoom
    Vec2 pre = screen_to_canvas(screen_x, screen_y);
    set_zoom(zoom_ * factor);
    // Recalculate offset so canvas point stays under cursor
    offset_.x = screen_x - pre.x * zoom_;
    offset_.y = screen_y - pre.y * zoom_;
}
```

**Test (tests/test_viewport_zoom.cpp):**
```cpp
#include <gtest/gtest.h>
#include "modules/mod_architect/viewport.h"

using namespace convoy::architect;

TEST(ViewportTest, ZoomToPoint_PreservesCanvasPoint) {
    Viewport vp(800, 600);
    vp.set_zoom(1.0f);

    // Canvas point at screen (100, 100) before zoom
    Vec2 before = vp.screen_to_canvas(100.0f, 100.0f);
    vp.zoom_to_point(100.0f, 100.0f, 2.0f);
    // After zoom, same screen point should map to same canvas point
    Vec2 after = vp.screen_to_canvas(100.0f, 100.0f);

    EXPECT_NEAR(before.x, after.x, 0.01f);
    EXPECT_NEAR(before.y, after.y, 0.01f);
    EXPECT_NEAR(vp.get_zoom(), 2.0f, 0.001f);
}

TEST(ViewportTest, ZoomClampsToLimits) {
    Viewport vp(800, 600);
    vp.set_zoom(1.0f);
    vp.zoom_to_point(400, 300, 1000.0f);
    EXPECT_EQ(vp.get_zoom(), 32.0f);
}
```

---

## Phase D: Architect Tools Completion

### Task D.1: Pivot Tool

**Files:**
- Create: `src/modules/mod_architect/tools/pivot_tool.h`
- Create: `src/modules/mod_architect/tools/pivot_tool.cpp`

**pivot_tool.h:**
```cpp
#pragma once
#include "tool_base.h"
#include "shared/math.h"

namespace convoy { namespace architect {

class PivotTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
    Vec2 get_pivot() const { return pivot_; }

private:
    Vec2 pivot_{0.0f, 0.0f};
};

}} // namespace convoy::architect
```

**pivot_tool.cpp:**
```cpp
#include "pivot_tool.h"

namespace convoy { namespace architect {

void PivotTool::on_mouse_down(Canvas* /*canvas*/, int x, int y) {
    pivot_ = {static_cast<float>(x), static_cast<float>(y)};
}
void PivotTool::on_mouse_drag(Canvas* /*c*/, int, int, int to_x, int to_y) {
    pivot_ = {static_cast<float>(to_x), static_cast<float>(to_y)};
}
void PivotTool::on_mouse_up(Canvas* /*c*/, int x, int y) {
    pivot_ = {static_cast<float>(x), static_cast<float>(y)};
}

}} // namespace
```

### Task D.2: Hitbox Tool

**Files:**
- Create: `src/modules/mod_architect/tools/hitbox_tool.h`
- Create: `src/modules/mod_architect/tools/hitbox_tool.cpp`

**hitbox_tool.h:**
```cpp
#pragma once
#include "tool_base.h"
#include "shared/math.h"

namespace convoy { namespace architect {

class HitboxTool : public Tool {
public:
    void on_mouse_down(Canvas* canvas, int x, int y) override;
    void on_mouse_drag(Canvas* canvas, int from_x, int from_y, int to_x, int to_y) override;
    void on_mouse_up(Canvas* canvas, int x, int y) override;
    const Rect& get_hitbox() const { return hitbox_; }
    bool is_defining() const { return defining_; }

private:
    Rect hitbox_{0, 0, 0, 0};
    int  start_x_ = 0, start_y_ = 0;
    bool defining_ = false;
};

}} // namespace
```

**hitbox_tool.cpp:**
```cpp
#include "hitbox_tool.h"
#include <algorithm>
#include <cmath>

namespace convoy { namespace architect {

void HitboxTool::on_mouse_down(Canvas*, int x, int y) {
    start_x_ = x; start_y_ = y;
    hitbox_  = {static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f};
    defining_ = true;
}

void HitboxTool::on_mouse_drag(Canvas*, int, int, int to_x, int to_y) {
    float x1 = static_cast<float>(std::min(start_x_, to_x));
    float y1 = static_cast<float>(std::min(start_y_, to_y));
    float x2 = static_cast<float>(std::max(start_x_, to_x));
    float y2 = static_cast<float>(std::max(start_y_, to_y));
    hitbox_ = {x1, y1, x2 - x1, y2 - y1};
}

void HitboxTool::on_mouse_up(Canvas*, int to_x, int to_y) {
    on_mouse_drag(nullptr, start_x_, start_y_, to_x, to_y);
    defining_ = false;
}

}} // namespace
```

**Tests (tests/test_tools.cpp):**
```cpp
#include <gtest/gtest.h>
#include "modules/mod_architect/tools/pivot_tool.h"
#include "modules/mod_architect/tools/hitbox_tool.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;
using namespace convoy::architect;

TEST(PivotToolTest, SetsPositionOnClick) {
    PivotTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 10, 20);
    EXPECT_FLOAT_EQ(tool.get_pivot().x, 10.0f);
    EXPECT_FLOAT_EQ(tool.get_pivot().y, 20.0f);
}

TEST(PivotToolTest, UpdatesOnDrag) {
    PivotTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 0, 0);
    tool.on_mouse_drag(&c, 0, 0, 15, 25);
    EXPECT_FLOAT_EQ(tool.get_pivot().x, 15.0f);
    EXPECT_FLOAT_EQ(tool.get_pivot().y, 25.0f);
}

TEST(HitboxToolTest, BuildsCorrectRect) {
    HitboxTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 5, 5);
    tool.on_mouse_drag(&c, 5, 5, 20, 30);
    tool.on_mouse_up(&c, 20, 30);
    const Rect& hb = tool.get_hitbox();
    EXPECT_FLOAT_EQ(hb.x, 5.0f);
    EXPECT_FLOAT_EQ(hb.y, 5.0f);
    EXPECT_FLOAT_EQ(hb.w, 15.0f);
    EXPECT_FLOAT_EQ(hb.h, 25.0f);
    EXPECT_FALSE(tool.is_defining());
}

TEST(HitboxToolTest, HandlesReverseDirection) {
    HitboxTool tool;
    Canvas c(64, 64);
    tool.on_mouse_down(&c, 20, 30);
    tool.on_mouse_up(&c, 5, 5);
    const Rect& hb = tool.get_hitbox();
    EXPECT_FLOAT_EQ(hb.x, 5.0f);
    EXPECT_FLOAT_EQ(hb.y, 5.0f);
    EXPECT_FLOAT_EQ(hb.w, 15.0f);
    EXPECT_FLOAT_EQ(hb.h, 25.0f);
}
```

### Task D.3: Color Palette Window

**Files:**
- Create: `src/modules/mod_architect/ui/color_palette.h`
- Create: `src/modules/mod_architect/ui/color_palette.cpp`

**color_palette.h:**
```cpp
#pragma once
#include "shared/color.h"
#include <vector>
#include <string>
#include <functional>

namespace convoy { namespace architect {

struct IndexedPalette {
    std::string       name;
    std::vector<Color> colors;
    static IndexedPalette make_default_16();
    static IndexedPalette make_db32();
};

class ColorPalette {
public:
    using SelectFn = std::function<void(Color)>;

    void render(bool* open = nullptr);
    void load_palette(const IndexedPalette& pal) { palette_ = pal; }
    void set_callback(SelectFn fn) { on_select_ = std::move(fn); }
    Color selected() const { return selected_; }

private:
    IndexedPalette palette_ = IndexedPalette::make_default_16();
    Color          selected_{255, 255, 255, 255};
    int            sel_idx_ = -1;
    SelectFn       on_select_;
};

}} // namespace
```

**color_palette.cpp:**
```cpp
#include "color_palette.h"
#include <imgui.h>
#include <cmath>

namespace convoy { namespace architect {

IndexedPalette IndexedPalette::make_default_16() {
    IndexedPalette p;
    p.name = "Default 16";
    p.colors = {
        {0,0,0,255},     {255,255,255,255}, {127,127,127,255}, {64,64,64,255},
        {255,0,0,255},   {0,255,0,255},     {0,0,255,255},     {255,255,0,255},
        {255,0,255,255}, {0,255,255,255},   {255,127,0,255},   {127,0,255,255},
        {0,127,255,255}, {255,0,127,255},   {0,255,127,255},   {127,255,0,255},
    };
    return p;
}

IndexedPalette IndexedPalette::make_db32() {
    // DB32 - DawnBringer 32-color palette (standard for pixel art)
    IndexedPalette p;
    p.name = "DB32";
    p.colors = {
        {0,0,0,255},       {34,32,52,255},    {69,40,60,255},    {102,57,49,255},
        {143,86,59,255},   {223,113,38,255},  {217,160,102,255}, {238,195,154,255},
        {251,242,54,255},  {153,229,80,255},  {106,190,48,255},  {55,148,110,255},
        {75,105,47,255},   {82,75,36,255},    {50,60,57,255},    {63,63,116,255},
        {48,96,130,255},   {91,110,225,255},  {99,155,255,255},  {95,205,228,255},
        {203,219,252,255}, {255,255,255,255}, {155,173,183,255}, {132,126,135,255},
        {105,106,106,255}, {89,86,82,255},    {118,66,138,255},  {172,50,50,255},
        {217,87,99,255},   {215,123,186,255}, {143,151,74,255},  {138,111,48,255},
    };
    return p;
}

void ColorPalette::render(bool* open) {
    if (!ImGui::Begin("Palette##Architect", open)) { ImGui::End(); return; }

    ImGui::TextDisabled("%s", palette_.name.c_str());
    ImGui::Separator();

    float swatch = 18.0f;
    float pad    = 2.0f;
    float region = ImGui::GetContentRegionAvail().x;
    int   cols   = std::max(1, static_cast<int>((region + pad) / (swatch + pad)));

    for (int i = 0; i < static_cast<int>(palette_.colors.size()); ++i) {
        const Color& c = palette_.colors[i];
        ImVec4 col4 = {c.r/255.0f, c.g/255.0f, c.b/255.0f, 1.0f};

        ImGui::PushID(i);
        bool selected = (i == sel_idx_);

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            cursor, {cursor.x + swatch, cursor.y + swatch},
            IM_COL32(c.r, c.g, c.b, 255));
        if (selected) {
            ImGui::GetWindowDrawList()->AddRect(
                cursor, {cursor.x + swatch, cursor.y + swatch},
                IM_COL32(0, 255, 65, 255), 0, 0, 2.0f);
        }

        ImGui::InvisibleButton("##swatch", {swatch, swatch});
        if (ImGui::IsItemClicked()) {
            selected_  = c;
            sel_idx_   = i;
            if (on_select_) on_select_(c);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("R:%d G:%d B:%d", c.r, c.g, c.b);
        }

        ImGui::PopID();
        if ((i + 1) % cols != 0)
            ImGui::SameLine(0.0f, pad);
    }

    ImGui::End();
}

}} // namespace
```

---

## Phase E: Plugin Architecture

### Task E.1: UIComponent Base Interface

**Files:**
- Create: `src/core/plugins/ui_component.h`

```cpp
#pragma once

namespace convoy {

/// Base interface for all renderable UI panels.
class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void on_render() = 0;
    virtual bool is_open() const { return true; }
};

} // namespace convoy
```

### Task E.2: IConvoyPlugin + PluginRegistry

**Files:**
- Create: `src/core/plugins/plugin_interface.h`
- Create: `src/core/plugins/plugin_registry.h`
- Create: `src/core/plugins/plugin_registry.cpp`

**plugin_interface.h:**
```cpp
#pragma once
#include "ui_component.h"
#include <string>

namespace convoy {

struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
};

class IConvoyPlugin : public UIComponent {
public:
    virtual ~IConvoyPlugin() = default;
    virtual void on_load()   = 0;
    virtual void on_unload() = 0;
    virtual PluginInfo info() const = 0;
};

} // namespace convoy
```

**plugin_registry.h:**
```cpp
#pragma once
#include "plugin_interface.h"
#include <memory>
#include <vector>
#include <string>

namespace convoy {

class PluginRegistry {
public:
    void register_plugin(std::shared_ptr<IConvoyPlugin> plugin);
    bool unregister_plugin(const std::string& name);
    void render_all();
    void unload_all();

    IConvoyPlugin* find(const std::string& name) const;
    size_t count() const { return plugins_.size(); }

private:
    std::vector<std::shared_ptr<IConvoyPlugin>> plugins_;
};

} // namespace convoy
```

**plugin_registry.cpp:**
```cpp
#include "plugin_registry.h"
#include <algorithm>

namespace convoy {

void PluginRegistry::register_plugin(std::shared_ptr<IConvoyPlugin> plugin) {
    plugin->on_load();
    plugins_.push_back(std::move(plugin));
}

bool PluginRegistry::unregister_plugin(const std::string& name) {
    auto it = std::find_if(plugins_.begin(), plugins_.end(),
        [&](const auto& p) { return p->info().name == name; });
    if (it == plugins_.end()) return false;
    (*it)->on_unload();
    plugins_.erase(it);
    return true;
}

void PluginRegistry::render_all() {
    for (auto& p : plugins_)
        if (p->is_open()) p->on_render();
}

void PluginRegistry::unload_all() {
    for (auto& p : plugins_) p->on_unload();
    plugins_.clear();
}

IConvoyPlugin* PluginRegistry::find(const std::string& name) const {
    for (auto& p : plugins_)
        if (p->info().name == name) return p.get();
    return nullptr;
}

} // namespace convoy
```

**Test (tests/test_plugin_registry.cpp):**
```cpp
#include <gtest/gtest.h>
#include "core/plugins/plugin_registry.h"

using namespace convoy;

struct MockPlugin : IConvoyPlugin {
    bool loaded = false, unloaded = false, rendered = false;
    std::string plugin_name;

    MockPlugin(const std::string& n) : plugin_name(n) {}
    void on_load()   override { loaded   = true; }
    void on_unload() override { unloaded = true; }
    void on_render() override { rendered = true; }
    PluginInfo info() const override { return {plugin_name, "1.0", "test", ""}; }
};

TEST(PluginRegistryTest, RegisterCallsOnLoad) {
    PluginRegistry reg;
    auto p = std::make_shared<MockPlugin>("test");
    reg.register_plugin(p);
    EXPECT_TRUE(p->loaded);
    EXPECT_EQ(reg.count(), 1u);
}

TEST(PluginRegistryTest, UnregisterCallsOnUnload) {
    PluginRegistry reg;
    auto p = std::make_shared<MockPlugin>("test");
    reg.register_plugin(p);
    bool removed = reg.unregister_plugin("test");
    EXPECT_TRUE(removed);
    EXPECT_TRUE(p->unloaded);
    EXPECT_EQ(reg.count(), 0u);
}

TEST(PluginRegistryTest, FindByName) {
    PluginRegistry reg;
    reg.register_plugin(std::make_shared<MockPlugin>("alpha"));
    reg.register_plugin(std::make_shared<MockPlugin>("beta"));
    EXPECT_NE(reg.find("alpha"), nullptr);
    EXPECT_EQ(reg.find("gamma"), nullptr);
}

TEST(PluginRegistryTest, UnloadAllClearsRegistry) {
    PluginRegistry reg;
    reg.register_plugin(std::make_shared<MockPlugin>("a"));
    reg.register_plugin(std::make_shared<MockPlugin>("b"));
    reg.unload_all();
    EXPECT_EQ(reg.count(), 0u);
}
```

### Task E.3: HookSystem

**Files:**
- Create: `src/core/plugins/hook_system.h`
- Create: `src/core/plugins/hook_system.cpp`

**hook_system.h:**
```cpp
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace convoy {

enum class HookResult  { Continue, SuppressOriginal };
enum class HookPoint : int {
    PreCanvasRender  = 0,
    PostCanvasRender = 1,
    PreWindowRender  = 2,
    OnProjectSave    = 3,
    OnExport         = 4,
};

class HookSystem {
public:
    using HookFn     = std::function<HookResult()>;
    using HookHandle = uint64_t;

    HookHandle register_hook(HookPoint point, HookFn fn);
    void       unregister_hook(HookPoint point, HookHandle handle);

    // Returns true if original behavior should run
    bool fire(HookPoint point) const;

private:
    struct Entry { HookHandle handle; HookFn fn; };
    std::unordered_map<int, std::vector<Entry>> hooks_;
    HookHandle next_ = 1;
};

} // namespace convoy
```

**hook_system.cpp:**
```cpp
#include "hook_system.h"

namespace convoy {

HookSystem::HookHandle HookSystem::register_hook(HookPoint point, HookFn fn) {
    HookHandle h = next_++;
    hooks_[static_cast<int>(point)].push_back({h, std::move(fn)});
    return h;
}

void HookSystem::unregister_hook(HookPoint point, HookHandle handle) {
    auto& vec = hooks_[static_cast<int>(point)];
    vec.erase(std::remove_if(vec.begin(), vec.end(),
        [handle](const Entry& e) { return e.handle == handle; }), vec.end());
}

bool HookSystem::fire(HookPoint point) const {
    auto it = hooks_.find(static_cast<int>(point));
    if (it == hooks_.end()) return true;
    for (const auto& e : it->second)
        if (e.fn() == HookResult::SuppressOriginal) return false;
    return true;
}

} // namespace convoy
```

**Test (tests/test_hook_system.cpp):**
```cpp
#include <gtest/gtest.h>
#include "core/plugins/hook_system.h"

using namespace convoy;

TEST(HookSystemTest, DefaultFireReturnsTrue) {
    HookSystem hs;
    EXPECT_TRUE(hs.fire(HookPoint::OnProjectSave));
}

TEST(HookSystemTest, ContinueHookAllowsOriginal) {
    HookSystem hs;
    hs.register_hook(HookPoint::OnProjectSave,
        []() { return HookResult::Continue; });
    EXPECT_TRUE(hs.fire(HookPoint::OnProjectSave));
}

TEST(HookSystemTest, SuppressHookBlocksOriginal) {
    HookSystem hs;
    hs.register_hook(HookPoint::OnProjectSave,
        []() { return HookResult::SuppressOriginal; });
    EXPECT_FALSE(hs.fire(HookPoint::OnProjectSave));
}

TEST(HookSystemTest, UnregisterRemovesHook) {
    HookSystem hs;
    auto h = hs.register_hook(HookPoint::OnExport,
        []() { return HookResult::SuppressOriginal; });
    hs.unregister_hook(HookPoint::OnExport, h);
    EXPECT_TRUE(hs.fire(HookPoint::OnExport));
}

TEST(HookSystemTest, MultipleHooksAllMustPassToContinue) {
    HookSystem hs;
    hs.register_hook(HookPoint::PreCanvasRender,
        []() { return HookResult::Continue; });
    hs.register_hook(HookPoint::PreCanvasRender,
        []() { return HookResult::SuppressOriginal; });
    EXPECT_FALSE(hs.fire(HookPoint::PreCanvasRender));
}
```

---

## Phase F: DOD Visualizer + Live Preview

### Task F.1: DOD Visualizer Window

**Files:**
- Create: `src/modules/mod_architect/ui/dod_visualizer.h`
- Create: `src/modules/mod_architect/ui/dod_visualizer.cpp`

**dod_visualizer.h:**
```cpp
#pragma once
#include "modules/mod_architect/canvas.h"
#include <cstdint>

namespace convoy { namespace architect {

struct AlignmentInfo {
    size_t data_bytes;
    size_t aligned_bytes;  // padded to next 32-byte boundary
    bool   is_aligned;     // data_bytes % 32 == 0
    int    waste_bytes;    // aligned_bytes - data_bytes
};

class DODVisualizer {
public:
    void render(bool* open, const Canvas* canvas);
    static AlignmentInfo compute_alignment(const Canvas* canvas);
    static size_t        compute_vram_estimate(const Canvas* canvas);

private:
    void render_memory_bar(size_t used_bytes, size_t total_bytes);
    void render_alignment_badge(const AlignmentInfo& info);
};

}} // namespace
```

**dod_visualizer.cpp:**
```cpp
#include "dod_visualizer.h"
#include "core/ui/theme_manager.h"
#include <imgui.h>
#include <cstdio>

namespace convoy { namespace architect {

AlignmentInfo DODVisualizer::compute_alignment(const Canvas* canvas) {
    if (!canvas) return {};
    size_t bytes = static_cast<size_t>(canvas->width()) *
                   static_cast<size_t>(canvas->height()) * 4;
    AlignmentInfo info;
    info.data_bytes    = bytes;
    info.is_aligned    = (bytes % 32 == 0);
    info.aligned_bytes = info.is_aligned ? bytes : bytes + (32 - bytes % 32);
    info.waste_bytes   = static_cast<int>(info.aligned_bytes - bytes);
    return info;
}

size_t DODVisualizer::compute_vram_estimate(const Canvas* canvas) {
    if (!canvas) return 0;
    // RGBA8 uncompressed; all layers
    size_t per_layer = static_cast<size_t>(canvas->width()) *
                       static_cast<size_t>(canvas->height()) * 4;
    return per_layer * canvas->get_layers().size();
}

void DODVisualizer::render_memory_bar(size_t used_bytes, size_t total_bytes) {
    float fraction = total_bytes > 0
        ? static_cast<float>(used_bytes) / static_cast<float>(total_bytes)
        : 0.0f;

    ImVec4 color = fraction > 0.8f
        ? DarkTerminalPalette::WARNING
        : DarkTerminalPalette::ACCENT;

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
    char overlay[64];
    snprintf(overlay, sizeof(overlay), "%zu KB / %zu KB",
             used_bytes / 1024, total_bytes / 1024);
    ImGui::ProgressBar(fraction, {-1, 0}, overlay);
    ImGui::PopStyleColor();
}

void DODVisualizer::render_alignment_badge(const AlignmentInfo& info) {
    if (info.is_aligned) {
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::ACCENT);
        ImGui::Text("ALIGNED [32-byte OK]  +0 bytes waste");
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, DarkTerminalPalette::WARNING);
        ImGui::Text("UNALIGNED! Needs +%d bytes to align", info.waste_bytes);
        ImGui::PopStyleColor();
    }
}

void DODVisualizer::render(bool* open, const Canvas* canvas) {
    if (!ImGui::Begin("DOD Visualizer##Architect", open)) { ImGui::End(); return; }

    if (!canvas) {
        ImGui::TextDisabled("No canvas loaded");
        ImGui::End();
        return;
    }

    ImGui::TextDisabled("CANVAS DATA DIAGNOSTICS");
    ImGui::Separator();

    size_t vram = compute_vram_estimate(canvas);
    size_t vram_max = 8 * 1024 * 1024; // 8 MB budget example

    ImGui::Text("Layers: %zu", canvas->get_layers().size());
    ImGui::Text("VRAM Estimate:");
    render_memory_bar(vram, vram_max);

    ImGui::Separator();
    ImGui::TextDisabled("GPU ALIGNMENT CHECK (32-byte)");
    AlignmentInfo info = compute_alignment(canvas);
    ImGui::Text("Raw size: %zu bytes", info.data_bytes);
    render_alignment_badge(info);
    if (!info.is_aligned) {
        ImGui::TextDisabled("Aligned size would be: %zu bytes", info.aligned_bytes);
    }

    ImGui::End();
}

}} // namespace
```

**Test (tests/test_dod_visualizer.cpp):**
```cpp
#include <gtest/gtest.h>
#include "modules/mod_architect/ui/dod_visualizer.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;
using namespace convoy::architect;

TEST(DODVisualizerTest, ComputeAlignment_AlignedCanvas) {
    // 8x4 canvas = 128 bytes = exactly 4 * 32 -> aligned
    Canvas c(8, 4);
    auto info = DODVisualizer::compute_alignment(&c);
    EXPECT_EQ(info.data_bytes, 128u);
    EXPECT_TRUE(info.is_aligned);
    EXPECT_EQ(info.waste_bytes, 0);
}

TEST(DODVisualizerTest, ComputeAlignment_UnalignedCanvas) {
    // 5x5 = 100 bytes, next multiple of 32 = 128
    Canvas c(5, 5);
    auto info = DODVisualizer::compute_alignment(&c);
    EXPECT_EQ(info.data_bytes, 100u);
    EXPECT_FALSE(info.is_aligned);
    EXPECT_EQ(info.aligned_bytes, 128u);
    EXPECT_EQ(info.waste_bytes, 28);
}

TEST(DODVisualizerTest, VRAMEstimateScalesWithLayers) {
    Canvas c(64, 64);
    size_t single = DODVisualizer::compute_vram_estimate(&c);
    c.add_layer("L2");
    size_t two = DODVisualizer::compute_vram_estimate(&c);
    EXPECT_EQ(two, single * 2);
}
```

### Task F.2: Live Preview Window + CRT Shader

**Files:**
- Create: `src/modules/mod_architect/ui/live_preview.h`
- Create: `src/modules/mod_architect/ui/live_preview.cpp`

**live_preview.h:**
```cpp
#pragma once
#include "core/gfx/render_texture.h"
#include "modules/mod_architect/canvas.h"

namespace convoy { namespace architect {

class LivePreview {
public:
    ~LivePreview() { shutdown(); }
    void initialize();
    void shutdown();
    void render(bool* open, const Canvas* canvas);
    void set_enable_crt(bool v) { crt_enabled_ = v; }
    void set_native_res(int w, int h) { native_w_ = w; native_h_ = h; }

private:
    void update_source_texture(const Canvas* canvas);
    void apply_crt_pass();
    void compile_crt_shader();

    RenderTexture source_rt_;     // Canvas pixels at native res
    RenderTexture crt_rt_;        // CRT post-process output
    uint32_t      crt_program_ = 0;
    uint32_t      quad_vao_    = 0;
    uint32_t      quad_vbo_    = 0;
    int           native_w_    = 320;
    int           native_h_    = 180;
    bool          crt_enabled_ = true;
    bool          initialized_ = false;
};

}} // namespace
```

**live_preview.cpp:**
```cpp
#include "live_preview.h"
#include <imgui.h>
#include <glad/glad.h>
#include <vector>
#include <cstring>

namespace convoy { namespace architect {

// Minimal full-screen quad for post-processing
static const char* kCRTVert = R"(
#version 410 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
out vec2 vUV;
void main() { vUV = aUV; gl_Position = vec4(aPos, 0.0, 1.0); }
)";

static const char* kCRTFrag = R"(
#version 410 core
in  vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
uniform vec2      uResolution;

void main() {
    vec4 col = texture(uTex, vUV);

    // Scanline: darken every other pixel row
    float scanline = sin(vUV.y * uResolution.y * 3.14159) * 0.5 + 0.5;
    scanline = mix(1.0, scanline, 0.18);

    // Slight green tint for CRT phosphor feel
    col.rgb *= vec3(0.92, 1.0, 0.88);
    col.rgb *= scanline;

    // Vignette
    vec2 uv2  = vUV * 2.0 - 1.0;
    float vig = 1.0 - dot(uv2, uv2) * 0.25;
    col.rgb  *= vig;

    FragColor = col;
}
)";

static uint32_t compile_shader(GLenum type, const char* src) {
    uint32_t s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

void LivePreview::compile_crt_shader() {
    uint32_t vert = compile_shader(GL_VERTEX_SHADER,   kCRTVert);
    uint32_t frag = compile_shader(GL_FRAGMENT_SHADER, kCRTFrag);
    crt_program_ = glCreateProgram();
    glAttachShader(crt_program_, vert);
    glAttachShader(crt_program_, frag);
    glLinkProgram(crt_program_);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

void LivePreview::initialize() {
    if (initialized_) return;

    source_rt_.create(native_w_, native_h_);
    crt_rt_.create(native_w_, native_h_);

    compile_crt_shader();

    // Full-screen quad
    float verts[] = {
        -1,-1, 0,0,   1,-1, 1,0,   1,1, 1,1,
        -1,-1, 0,0,   1,1,  1,1,  -1,1, 0,1
    };
    glGenVertexArrays(1, &quad_vao_);
    glGenBuffers(1, &quad_vbo_);
    glBindVertexArray(quad_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),
                          (void*)(2*sizeof(float)));
    glBindVertexArray(0);

    initialized_ = true;
}

void LivePreview::shutdown() {
    if (!initialized_) return;
    source_rt_.destroy();
    crt_rt_.destroy();
    if (crt_program_) { glDeleteProgram(crt_program_); crt_program_ = 0; }
    if (quad_vao_)    { glDeleteVertexArrays(1, &quad_vao_); quad_vao_ = 0; }
    if (quad_vbo_)    { glDeleteBuffers(1, &quad_vbo_);      quad_vbo_ = 0; }
    initialized_ = false;
}

void LivePreview::update_source_texture(const Canvas* canvas) {
    if (!canvas) return;
    std::vector<uint32_t> pixels;
    const_cast<Canvas*>(canvas)->composite_to_texture(pixels);

    glBindTexture(GL_TEXTURE_2D, source_rt_.texture_id());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<int>(canvas->width()),
                 static_cast<int>(canvas->height()),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void LivePreview::apply_crt_pass() {
    crt_rt_.bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(crt_program_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source_rt_.texture_id());
    glUniform1i(glGetUniformLocation(crt_program_, "uTex"), 0);
    glUniform2f(glGetUniformLocation(crt_program_, "uResolution"),
                static_cast<float>(native_w_),
                static_cast<float>(native_h_));

    glBindVertexArray(quad_vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    crt_rt_.unbind();
}

void LivePreview::render(bool* open, const Canvas* canvas) {
    if (!initialized_) initialize();
    if (!ImGui::Begin("Live Preview##Architect", open)) { ImGui::End(); return; }

    ImGui::Checkbox("CRT Filter", &crt_enabled_);
    ImGui::SameLine();
    ImGui::TextDisabled("(%dx%d native)", native_w_, native_h_);

    if (canvas) {
        update_source_texture(canvas);
        if (crt_enabled_) apply_crt_pass();

        uint32_t display_tex = crt_enabled_
            ? crt_rt_.texture_id()
            : source_rt_.texture_id();

        ImVec2 avail = ImGui::GetContentRegionAvail();
        float  ar    = static_cast<float>(native_w_) / static_cast<float>(native_h_);
        float  w     = avail.x;
        float  h     = w / ar;
        if (h > avail.y) { h = avail.y; w = h * ar; }

        ImGui::Image((ImTextureID)(intptr_t)display_tex, {w, h},
                     {0,1}, {1,0}); // Flip Y for OpenGL
    } else {
        ImGui::TextDisabled("No canvas");
    }

    ImGui::End();
}

}} // namespace
```

---

## Phase G: Module Stubs (Walker, Forge, Sequencer)

### Task G.1: Walker Module Stub

**Files:**
- Create: `src/modules/mod_walker/walker_ui.h`
- Create: `src/modules/mod_walker/walker_ui.cpp`

**walker_ui.h:**
```cpp
#pragma once

namespace convoy { namespace walker {

class WalkerUI {
public:
    void render();

private:
    void render_tile_grid_placeholder();
    void render_tile_selector();
    void render_collision_toggle();

    bool show_collision_ = false;
    int  grid_size_      = 16;
};

}} // namespace
```

**walker_ui.cpp:**
```cpp
#include "walker_ui.h"
#include <imgui.h>

namespace convoy { namespace walker {

void WalkerUI::render() {
    ImGui::Begin("Walker: World Builder##Walker");
    ImGui::TextDisabled("Walker module - Fase 3 (Set/Nov 2026)");
    ImGui::Separator();

    ImGui::Text("Grid Size:");
    ImGui::SameLine();
    ImGui::RadioButton("16x16", &grid_size_, 16);
    ImGui::SameLine();
    ImGui::RadioButton("32x32", &grid_size_, 32);

    ImGui::Checkbox("Debug Physics (Collision Overlay)", &show_collision_);
    ImGui::Separator();

    render_tile_grid_placeholder();
    ImGui::End();

    render_tile_selector();
}

void WalkerUI::render_tile_grid_placeholder() {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    float step = static_cast<float>(grid_size_);
    ImU32 col  = show_collision_
        ? IM_COL32(255, 144, 0, 60)
        : IM_COL32(42, 42, 42, 120);

    for (float x = 0; x < avail.x; x += step)
        dl->AddLine({origin.x + x, origin.y},
                    {origin.x + x, origin.y + avail.y}, col);
    for (float y = 0; y < avail.y; y += step)
        dl->AddLine({origin.x, origin.y + y},
                    {origin.x + avail.x, origin.y + y}, col);

    ImGui::Dummy(avail);
}

void WalkerUI::render_tile_selector() {
    ImGui::Begin("Tile Selector##Walker");
    ImGui::TextDisabled("Tile assets will appear here");
    ImGui::End();
}

}} // namespace
```

### Task G.2: Forge Module Stub

**Files:**
- Create: `src/modules/mod_forge/forge_ui.h`
- Create: `src/modules/mod_forge/forge_ui.cpp`

**forge_ui.h:**
```cpp
#pragma once

namespace convoy { namespace forge {

class ForgeUI {
public:
    void render();
};

}} // namespace
```

**forge_ui.cpp:**
```cpp
#include "forge_ui.h"
#include <imgui.h>

namespace convoy { namespace forge {

void ForgeUI::render() {
    ImGui::Begin("Forge: Atlas Packer##Forge");
    ImGui::TextDisabled("Forge module - Fase 2 (Jun/Ago 2026)");
    ImGui::Separator();
    ImGui::Text("Sprites loaded: 0");
    ImGui::Text("Atlas size:     not packed");
    ImGui::Separator();
    if (ImGui::Button("Pack Atlas")) {
        // TODO: invoke bin-packing algorithm
    }
    ImGui::End();
}

}} // namespace
```

### Task G.3: Sequencer Module Stub

**Files:**
- Create: `src/modules/mod_sequencer/sequencer_ui.h`
- Create: `src/modules/mod_sequencer/sequencer_ui.cpp`

**sequencer_ui.h:**
```cpp
#pragma once

namespace convoy { namespace sequencer {

class SequencerUI {
public:
    void render();

private:
    int  fps_         = 12;
    int  frame_count_ = 4;
    int  current_frame_ = 0;
    bool playing_    = false;
};

}} // namespace
```

**sequencer_ui.cpp:**
```cpp
#include "sequencer_ui.h"
#include <imgui.h>
#include <string>

namespace convoy { namespace sequencer {

void SequencerUI::render() {
    ImGui::Begin("Sequencer: Animation##Seq");
    ImGui::TextDisabled("Sequencer module - Fase 2 (Jun/Ago 2026)");
    ImGui::Separator();

    ImGui::SliderInt("FPS",    &fps_,         1, 60);
    ImGui::SliderInt("Frames", &frame_count_, 1, 64);
    ImGui::Separator();

    // Minimal timeline placeholder
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 orig = ImGui::GetCursorScreenPos();
    float avail_w = ImGui::GetContentRegionAvail().x;
    float frame_w = (frame_count_ > 0) ? avail_w / frame_count_ : avail_w;
    float height  = 32.0f;

    for (int i = 0; i < frame_count_; ++i) {
        float x = orig.x + i * frame_w;
        ImU32 col = (i == current_frame_)
            ? IM_COL32(0, 255, 65, 200)
            : IM_COL32(42, 42, 42, 255);
        dl->AddRectFilled({x, orig.y}, {x + frame_w - 2, orig.y + height}, col);
        dl->AddRect      ({x, orig.y}, {x + frame_w - 2, orig.y + height},
                          IM_COL32(42, 42, 42, 255));
        std::string label = std::to_string(i + 1);
        dl->AddText({x + 4, orig.y + 8}, IM_COL32(200, 200, 200, 255), label.c_str());
    }
    ImGui::Dummy({avail_w, height});
    ImGui::Separator();

    if (ImGui::Button(playing_ ? "|| Pause" : "> Play"))
        playing_ = !playing_;
    ImGui::SameLine();
    if (ImGui::Button("|< Reset")) { current_frame_ = 0; playing_ = false; }

    ImGui::End();
}

}} // namespace
```

---

## Final CMakeLists.txt Update

Add all new sources:

```cmake
# In the GLFW3/OpenGL conditional block for convoy_core:
target_sources(convoy_core PRIVATE
    src/core/window_manager.cpp
    src/core/event_bus.cpp
    src/core/ui/theme_manager.cpp
    src/core/ui/dockspace_manager.cpp
    src/core/ui/main_menu_bar.cpp
    src/core/input/input_handler.cpp
    src/core/gfx/render_texture.cpp
    src/core/plugins/plugin_registry.cpp
    src/core/plugins/hook_system.cpp
)

# Architect module gets new sources:
add_library(mod_architect
    src/modules/mod_architect/canvas.cpp
    src/modules/mod_architect/viewport.cpp
    src/modules/mod_architect/architect_ui.cpp
    src/modules/mod_architect/tools/pencil_tool.cpp
    src/modules/mod_architect/tools/eraser_tool.cpp
    src/modules/mod_architect/tools/bucket_tool.cpp
    src/modules/mod_architect/tools/pivot_tool.cpp
    src/modules/mod_architect/tools/hitbox_tool.cpp
    src/modules/mod_architect/ui/color_palette.cpp
    src/modules/mod_architect/ui/dod_visualizer.cpp
    src/modules/mod_architect/ui/live_preview.cpp
)

# New module stubs:
add_library(mod_walker
    src/modules/mod_walker/walker_ui.cpp
)
target_include_directories(mod_walker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mod_walker PUBLIC convoy_core)

add_library(mod_forge
    src/modules/mod_forge/forge_ui.cpp
)
target_include_directories(mod_forge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mod_forge PUBLIC convoy_core)

add_library(mod_sequencer
    src/modules/mod_sequencer/sequencer_ui.cpp
)
target_include_directories(mod_sequencer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mod_sequencer PUBLIC convoy_core)

# Main executable links all modules
add_executable(convoy src/convoy/main.cpp)
target_link_libraries(convoy PRIVATE convoy_core mod_architect mod_walker mod_forge mod_sequencer)
```

**Test CMakeLists.txt - add new test files:**
```cmake
set(TEST_SOURCES
    test_math.cpp
    test_canvas.cpp
    test_cvp_format.cpp
    test_command_manager.cpp
    test_architect_ui.cpp
    test_input_handler.cpp
    test_tools.cpp
    test_viewport_zoom.cpp
    test_plugin_registry.cpp
    test_hook_system.cpp
    test_dod_visualizer.cpp
    integration_test.cpp
)
```

---

## Final Verification Checklist

After all phases:
- [ ] `cmake --build build` exits 0 (no warnings treated as errors)
- [ ] `./build/tests/convoy_tests` all tests pass
- [ ] Running `convoy` shows Dark Terminal theme (green accents, #0A0A0A background)
- [ ] Menu bar visible at top with File/Edit/Modules/View
- [ ] Canvas panel renders pixel data via `ImGui::Image` (not stub text)
- [ ] Scroll wheel zooms towards cursor position
- [ ] Middle mouse / Alt+LMB pans canvas
- [ ] Grid appears when zoom >= 4x
- [ ] [V] key activates Pivot tool, click sets crosshair on canvas
- [ ] [H] key activates Hitbox tool, drag draws orange rectangle
- [ ] Tools panel shows active tool with green accent highlight
- [ ] Layers panel shows layer list with visibility toggle
- [ ] Properties panel shows canvas size, zoom, pivot coordinates, hitbox
- [ ] DOD Visualizer shows memory bar and alignment badge (green/orange)
- [ ] Live Preview shows canvas with CRT scanline filter when enabled
- [ ] Walker, Forge, Sequencer stubs open from Modules menu
- [ ] F1-F4 key shortcuts open module panels
- [ ] Ctrl+Z/Y trigger undo/redo
