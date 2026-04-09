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

}
