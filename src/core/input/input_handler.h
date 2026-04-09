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

}
