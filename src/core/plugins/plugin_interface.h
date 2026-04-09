#pragma once
#include "ui_component.h"
#include <string>

namespace convoy {

struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
};

class IConvoyPlugin : public UIComponent {
public:
    virtual ~IConvoyPlugin() = default;
    virtual void on_load()   = 0;
    virtual void on_unload() = 0;
    virtual PluginInfo info() const = 0;
};

}
