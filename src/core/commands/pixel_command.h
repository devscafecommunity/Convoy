#pragma once
#include "core/commands/command.h"
#include "shared/color.h"
#include "modules/mod_architect/canvas.h"

namespace convoy {

class PixelCommand : public Command {
public:
    PixelCommand(architect::Canvas* canvas, uint32_t x, uint32_t y, Color new_color);

    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    architect::Canvas* canvas_;
    uint32_t x_;
    uint32_t y_;
    Color old_color_;
    Color new_color_;
    bool executed_;
};

} // namespace convoy
