#pragma once
#include <memory>
#include <string>
#include <vector>

#include "plugin_interface.h"

namespace convoy
{

class PluginRegistry
{
   public:
    void register_plugin(std::shared_ptr<IConvoyPlugin> plugin);
    bool unregister_plugin(const std::string& name);
    void render_all();
    void unload_all();

    IConvoyPlugin* find(const std::string& name) const;
    size_t count() const { return plugins_.size(); }
    const std::vector<std::shared_ptr<IConvoyPlugin>>& get_all() const { return plugins_; }

   private:
    std::vector<std::shared_ptr<IConvoyPlugin>> plugins_;
};

}  // namespace convoy
