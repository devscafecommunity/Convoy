#pragma once
#include <string>

namespace convoy {

class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string description() const = 0;

    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    Command(Command&&) = default;
    Command& operator=(Command&&) = default;

protected:
    Command() = default;
};

} // namespace convoy
