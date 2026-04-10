#pragma once
#include <functional>
#include <string>
#include <vector>

namespace convoy
{

struct Keybind
{
    std::string id;
    std::string category;
    std::string display_name;
    std::string current_key;
    std::string current_modifiers;
    std::string default_key;
    std::string default_modifiers;
};

class KeybindManager
{
   public:
    static KeybindManager& instance();

    void register_keybind(const Keybind& bind);
    void unregister_keybind(const std::string& id);

    const std::vector<Keybind>& get_all_keybinds() const { return keybinds_; }
    std::vector<Keybind> get_keybinds_by_category(const std::string& category) const;
    const Keybind* find_keybind(const std::string& id) const;

    bool set_keybind(const std::string& id, const std::string& key, const std::string& modifiers);
    bool reset_keybind(const std::string& id);
    void reset_all_keybinds();

    void set_on_keybind_changed(std::function<void(const std::string& id)> callback) { on_keybind_changed_ = callback; }

   private:
    KeybindManager();
    std::vector<Keybind> keybinds_;
    std::function<void(const std::string& id)> on_keybind_changed_;
};

void initialize_default_keybinds();

}  // namespace convoy
