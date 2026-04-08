#pragma once
#include "core/commands/command.h"
#include <memory>
#include <stack>
#include <string>

namespace convoy {

class CommandManager {
public:
    CommandManager() = default;

    void execute(std::unique_ptr<Command> command);
    void undo();
    void redo();
    void clear();

    bool can_undo() const;
    bool can_redo() const;

    std::string undo_description() const;
    std::string redo_description() const;

private:
    std::stack<std::unique_ptr<Command>> undo_stack_;
    std::stack<std::unique_ptr<Command>> redo_stack_;

    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
};

} // namespace convoy
