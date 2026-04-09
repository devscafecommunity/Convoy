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

}
