#include "window_manager.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

#include "../../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../../third_party/imgui/backends/imgui_impl_opengl3.h"
#include "keybind_manager.h"
#include "logger.h"

namespace convoy
{

WindowManager::WindowManager() : canvas_(64, 64) {}

WindowManager::~WindowManager()
{
    if (initialized_)
        shutdown();
}

void WindowManager::glfw_error_callback(int error, const char* description)
{
    Logger::error("GLFW Error {}: {}", error, description);
}

void WindowManager::glfw_key_callback(GLFWwindow* window, int key, int /*sc*/, int action, int mods)
{
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self)
        self->input_.process_key(key, action, mods);
}

void WindowManager::initialize(const std::string& title, int width, int height)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, glfw_key_callback);
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gladLoadGL())
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "convoy_layout.ini";

    ThemeManager::apply_dark_terminal();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    input_.setup_defaults();
    setup_input_commands();

    initialize_default_keybinds();

    MenuBarCallbacks callbacks;
    callbacks.on_new_project  = [this]() { new_project_dialog_.open(); };
    callbacks.on_open_project = []() {};
    callbacks.on_save_project = []() {};
    callbacks.on_export = [this]() { export_wizard_.open(); };
    callbacks.on_undo   = [this]() { cmd_mgr_.undo(); };
    callbacks.on_redo   = [this]() { cmd_mgr_.redo(); };
    callbacks.on_module_switch = [this](const std::string& module) {
        if      (module == "architect") current_module_ = ActiveModule::Architect;
        else if (module == "forge")     current_module_ = ActiveModule::Forge;
        else if (module == "sequencer") current_module_ = ActiveModule::Sequencer;
        else if (module == "walker")    current_module_ = ActiveModule::Walker;
    };
    callbacks.on_preferences = [this]() { preferences_.open(); };
    callbacks.on_layout_change = [this](int preset) {
        if (preset < 0) {
            dockspace_.reset_to_default();
        } else if (preset < 3) {
            dockspace_.switch_preset(static_cast<LayoutPreset>(preset));
        }
    };
    callbacks.on_toggle_snap_zones = [this]() { dockspace_.toggle_snap_zones(); };
    callbacks.on_toggle_grid       = [this]() { architect_ui_.toggle_grid(); };
    callbacks.on_toggle_collision  = [this]() { architect_ui_.toggle_collision_overlay(); };
    callbacks.on_toggle_dod        = [this]() { architect_ui_.toggle_dod_visualizer(); };
    menubar_.set_callbacks(callbacks);

    pressure_curve_editor_.on_curve_changed = [](const std::vector<SplinePoint>&) {};

    architect_ui_.initialize(&canvas_, &cmd_mgr_);

    initialized_ = true;
    Logger::info("WindowManager initialized {}x{}", width, height);
}

void WindowManager::setup_input_commands()
{
    input_.register_command("edit.undo", [this]() { cmd_mgr_.undo(); });
    input_.register_command("edit.redo", [this]() { cmd_mgr_.redo(); });
    input_.register_command("tool.pencil", [this]() { architect_ui_.set_tool(ToolType::Pencil); });
    input_.register_command("tool.eraser", [this]() { architect_ui_.set_tool(ToolType::Eraser); });
    input_.register_command("tool.bucket", [this]() { architect_ui_.set_tool(ToolType::Bucket); });
    input_.register_command("tool.pivot", [this]() { architect_ui_.set_tool(ToolType::Pivot); });
    input_.register_command("tool.hitbox", [this]() { architect_ui_.set_tool(ToolType::Hitbox); });
}

void WindowManager::render_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    menubar_.render(cmd_mgr_.can_undo(), cmd_mgr_.can_redo(), project_name_, 16.0f, 128.0f);

    dockspace_.begin_frame();
    dockspace_.end_frame();

    switch (current_module_) {
        case ActiveModule::Architect: architect_ui_.render(); break;
        case ActiveModule::Forge:     forge_ui_.render();     break;
        case ActiveModule::Sequencer: sequencer_ui_.render(); break;
        case ActiveModule::Walker:    walker_ui_.render();    break;
    }

    new_project_dialog_.render();
    preferences_.render();
    pressure_curve_editor_.render();
    export_wizard_.render();

    ImGui::Render();

    int fw, fh;
    glfwGetFramebufferSize(window_, &fw, &fh);
    glViewport(0, 0, fw, fh);
    glClearColor(0.039f, 0.039f, 0.039f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void WindowManager::run_loop()
{
    while (!should_close())
    {
        glfwPollEvents();
        render_frame();
        glfwSwapBuffers(window_);
    }
}

bool WindowManager::should_close() const
{
    return !window_ || glfwWindowShouldClose(window_);
}

void WindowManager::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (window_)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
    initialized_ = false;
    Logger::info("WindowManager shutdown");
}

}  // namespace convoy
