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
    bind("tool.pencil",   GLFW_KEY_B);
    bind("tool.eraser",   GLFW_KEY_E);
    bind("tool.bucket",   GLFW_KEY_G);
    bind("tool.pivot",    GLFW_KEY_V);
    bind("tool.hitbox",   GLFW_KEY_H);
    bind("module.architect",  GLFW_KEY_F1);
    bind("module.forge",      GLFW_KEY_F2);
    bind("module.sequencer",  GLFW_KEY_F3);
    bind("module.walker",     GLFW_KEY_F4);
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

}
