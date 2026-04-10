#pragma once
#include <functional>
#include <string>
#include <vector>

namespace convoy
{

using SettingsChangeCallback = std::function<void()>;

class ISettingsSection
{
   public:
    virtual ~ISettingsSection() = default;
    virtual void render() = 0;
    virtual std::string get_name() const = 0;
};

class PreferencesWindow
{
   public:
    void render();
    bool is_open() const { return is_open_; }
    void open() { is_open_ = true; }
    void close() { is_open_ = false; }
    void toggle() { is_open_ = !is_open_; }

    void register_plugin_section(ISettingsSection* section);

    void set_on_apply(SettingsChangeCallback cb) { on_apply_ = cb; }
    void set_on_reset(SettingsChangeCallback cb) { on_reset_ = cb; }

   private:
    void render_general_tab();
    void render_input_tab();
    void render_graphics_tab();
    void render_engine_tab();
    void render_plugins_tab();
    void render_keybinds_tab();

    bool is_open_ = false;
    int selected_category_ = 0;
    char search_buffer_[256] = {};

    std::vector<ISettingsSection*> plugin_sections_;
    SettingsChangeCallback on_apply_;
    SettingsChangeCallback on_reset_;
};

}  // namespace convoy
