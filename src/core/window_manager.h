#pragma once
#include "event_bus.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace convoy {

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    
    void initialize(const std::string& title, int width, int height);
    void run_loop();
    void shutdown();
    
    GLFWwindow* get_window() const { return window_; }
    EventBus& get_event_bus() { return event_bus_; }
    
    bool should_close() const;
    
private:
    GLFWwindow* window_;
    EventBus event_bus_;
    bool initialized_;
    
    static void glfw_error_callback(int error, const char* description);
};

}
