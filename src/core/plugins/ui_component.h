#pragma once

namespace convoy {

class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void on_render() = 0;
    virtual bool is_open() const { return true; }
};

}
