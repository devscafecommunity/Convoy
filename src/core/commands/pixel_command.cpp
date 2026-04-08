#include "core/commands/pixel_command.h"

namespace convoy {

PixelCommand::PixelCommand(architect::Canvas* canvas, uint32_t x, uint32_t y, Color new_color)
    : canvas_(canvas), x_(x), y_(y), new_color_(new_color), executed_(false) {
    old_color_ = canvas_->get_pixel(x_, y_);
}

void PixelCommand::execute() {
    if (!executed_) {
        canvas_->set_pixel(x_, y_, new_color_);
        executed_ = true;
    }
}

void PixelCommand::undo() {
    canvas_->set_pixel(x_, y_, old_color_);
    executed_ = false;
}

std::string PixelCommand::description() const {
    return "Paint pixel";
}

} // namespace convoy
