#include "keybind_manager.h"

#include <algorithm>

namespace convoy
{

KeybindManager& KeybindManager::instance()
{
    static KeybindManager instance;
    return instance;
}

KeybindManager::KeybindManager() {}

void KeybindManager::register_keybind(const Keybind& bind)
{
    for (auto& existing : keybinds_)
    {
        if (existing.id == bind.id)
        {
            existing = bind;
            return;
        }
    }
    keybinds_.push_back(bind);
}

void KeybindManager::unregister_keybind(const std::string& id)
{
    keybinds_.erase(std::remove_if(keybinds_.begin(), keybinds_.end(), [&id](const Keybind& k) { return k.id == id; }),
                    keybinds_.end());
}

std::vector<Keybind> KeybindManager::get_keybinds_by_category(const std::string& category) const
{
    std::vector<Keybind> result;
    for (const auto& k : keybinds_)
    {
        if (k.category == category)
            result.push_back(k);
    }
    return result;
}

const Keybind* KeybindManager::find_keybind(const std::string& id) const
{
    for (const auto& k : keybinds_)
    {
        if (k.id == id)
            return &k;
    }
    return nullptr;
}

bool KeybindManager::set_keybind(const std::string& id, const std::string& key, const std::string& modifiers)
{
    for (auto& k : keybinds_)
    {
        if (k.id == id)
        {
            k.current_key = key;
            k.current_modifiers = modifiers;
            if (on_keybind_changed_)
                on_keybind_changed_(id);
            return true;
        }
    }
    return false;
}

bool KeybindManager::reset_keybind(const std::string& id)
{
    for (auto& k : keybinds_)
    {
        if (k.id == id)
        {
            k.current_key = k.default_key;
            k.current_modifiers = k.default_modifiers;
            if (on_keybind_changed_)
                on_keybind_changed_(id);
            return true;
        }
    }
    return false;
}

void KeybindManager::reset_all_keybinds()
{
    for (auto& k : keybinds_)
    {
        k.current_key = k.default_key;
        k.current_modifiers = k.default_modifiers;
    }
    if (on_keybind_changed_)
        on_keybind_changed_("");
}

void initialize_default_keybinds()
{
    auto& km = KeybindManager::instance();

    km.register_keybind({"file.new", "File", "New Project", "Ctrl+N", "Ctrl", "Ctrl+N", "Ctrl"});
    km.register_keybind({"file.open", "File", "Open Project", "Ctrl+O", "Ctrl", "Ctrl+O", "Ctrl"});
    km.register_keybind({"file.save", "File", "Save Project", "Ctrl+S", "Ctrl", "Ctrl+S", "Ctrl"});
    km.register_keybind({"file.export", "File", "Export CAF", "Ctrl+D", "Ctrl", "Ctrl+D", "Ctrl"});

    km.register_keybind({"edit.undo", "Edit", "Undo", "Ctrl+Z", "Ctrl", "Ctrl+Z", "Ctrl"});
    km.register_keybind({"edit.redo", "Edit", "Redo", "Ctrl+Y", "Ctrl", "Ctrl+Y", "Ctrl"});
    km.register_keybind({"edit.preferences", "Edit", "Preferences", ",", "", ",", ""});

    km.register_keybind({"view.grid", "View", "Toggle Grid", "G", "", "G", ""});
    km.register_keybind({"view.collision", "View", "Toggle Collision", "C", "", "C", ""});
    km.register_keybind({"view.dod", "View", "DOD Visualizer", "D", "Ctrl", "D", "Ctrl"});
    km.register_keybind({"view.rulers", "View", "Toggle Rulers", "R", "", "R", ""});

    km.register_keybind({"tool.pencil", "Tools", "Pencil", "P", "", "P", ""});
    km.register_keybind({"tool.eraser", "Tools", "Eraser", "E", "", "E", ""});
    km.register_keybind({"tool.fill", "Tools", "Bucket Fill", "F", "", "F", ""});
    km.register_keybind({"tool.pivot", "Tools", "Pivot Point", "I", "", "I", ""});
    km.register_keybind({"tool.hitbox", "Tools", "Hitbox Editor", "H", "", "H", ""});

    km.register_keybind({"module.architect", "Modules", "Architect", "F1", "", "F1", ""});
    km.register_keybind({"module.forge", "Modules", "Forge", "F2", "", "F2", ""});
    km.register_keybind({"module.sequencer", "Modules", "Sequencer", "F3", "", "F3", ""});
    km.register_keybind({"module.walker", "Modules", "Walker", "F4", "", "F4", ""});

    km.register_keybind({"zoom.in", "Zoom", "Zoom In", "+", "", "+", ""});
    km.register_keybind({"zoom.out", "Zoom", "Zoom Out", "-", "", "-", ""});
    km.register_keybind({"zoom.fit", "Zoom", "Fit to Canvas", "0", "", "0", ""});
}

}  // namespace convoy
