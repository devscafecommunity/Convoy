#pragma once
#include <string>

#include "command_manager.h"
#include "core/input/input_handler.h"
#include "core/ui/dockspace_manager.h"
#include "core/ui/main_menu_bar.h"
#include "core/ui/preferences_window.h"
#include "core/ui/pressure_curve_editor.h"
#include "core/ui/theme_manager.h"
#include "modules/mod_architect/architect_ui.h"
#include "modules/mod_architect/canvas.h"

struct GLFWwindow;

namespace convoy
{

class WindowManager
{
   public:
    WindowManager();
    ~WindowManager();

    void initialize(const std::string& title, int width, int height);
    void run_loop();
    void shutdown();
    bool should_close() const;

   private:
    static void glfw_error_callback(int error, const char* description);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void render_frame();
    void setup_input_commands();

    GLFWwindow* window_ = nullptr;
    bool initialized_ = false;

    DockSpaceManager dockspace_;
    MainMenuBar menubar_;
    InputHandler input_;
    CommandManager cmd_mgr_;

    PreferencesWindow preferences_;
    PressureCurveEditor pressure_curve_editor_;

    architect::Canvas canvas_;
    architect::ArchitectUI architect_ui_;

    std::string project_name_ = "Untitled";
};

}  // namespace convoy
