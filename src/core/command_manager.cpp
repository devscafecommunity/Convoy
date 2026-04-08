#include "core/command_manager.h"

namespace convoy {

void CommandManager::execute(std::unique_ptr<Command> command) {
    command->execute();
    undo_stack_.push(std::move(command));
    while (!redo_stack_.empty()) {
        redo_stack_.pop();
    }
}

void CommandManager::undo() {
    if (can_undo()) {
        auto command = std::move(undo_stack_.top());
        undo_stack_.pop();
        command->undo();
        redo_stack_.push(std::move(command));
    }
}

void CommandManager::redo() {
    if (can_redo()) {
        auto command = std::move(redo_stack_.top());
        redo_stack_.pop();
        command->execute();
        undo_stack_.push(std::move(command));
    }
}

void CommandManager::clear() {
    while (!undo_stack_.empty()) {
        undo_stack_.pop();
    }
    while (!redo_stack_.empty()) {
        redo_stack_.pop();
    }
}

bool CommandManager::can_undo() const {
    return !undo_stack_.empty();
}

bool CommandManager::can_redo() const {
    return !redo_stack_.empty();
}

std::string CommandManager::undo_description() const {
    if (can_undo()) {
        return undo_stack_.top()->description();
    }
    return "";
}

std::string CommandManager::redo_description() const {
    if (can_redo()) {
        return redo_stack_.top()->description();
    }
    return "";
}

} // namespace convoy
