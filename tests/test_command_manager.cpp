#include <gtest/gtest.h>
#include <memory>
#include "core/command_manager.h"
#include "core/commands/pixel_command.h"
#include "modules/mod_architect/canvas.h"

using namespace convoy;

class CommandManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        canvas = std::make_unique<architect::Canvas>(10, 10);
        manager = std::make_unique<CommandManager>();
    }

    std::unique_ptr<architect::Canvas> canvas;
    std::unique_ptr<CommandManager> manager;
};

TEST_F(CommandManagerTest, ExecuteAndUndo) {
    // Check initial state
    Color initial = canvas->get_pixel(5, 5);
    EXPECT_EQ(initial.r, 0);
    EXPECT_EQ(initial.g, 0);
    EXPECT_EQ(initial.b, 0);

    // Execute paint command
    Color new_color(255, 0, 0, 255);  // Red
    auto cmd = std::make_unique<PixelCommand>(canvas.get(), 5, 5, new_color);
    manager->execute(std::move(cmd));

    // Verify pixel changed
    Color after_execute = canvas->get_pixel(5, 5);
    EXPECT_EQ(after_execute.r, 255);
    EXPECT_EQ(after_execute.g, 0);
    EXPECT_EQ(after_execute.b, 0);

    // Verify we can undo
    EXPECT_TRUE(manager->can_undo());
    EXPECT_EQ(manager->undo_description(), "Paint pixel");

    // Undo
    manager->undo();

    // Verify pixel reverted
    Color after_undo = canvas->get_pixel(5, 5);
    EXPECT_EQ(after_undo.r, 0);
    EXPECT_EQ(after_undo.g, 0);
    EXPECT_EQ(after_undo.b, 0);
}

TEST_F(CommandManagerTest, UndoRedo) {
    Color new_color(0, 255, 0, 255);  // Green

    // Execute
    auto cmd = std::make_unique<PixelCommand>(canvas.get(), 3, 3, new_color);
    manager->execute(std::move(cmd));

    Color after_execute = canvas->get_pixel(3, 3);
    EXPECT_EQ(after_execute.g, 255);

    // Undo
    manager->undo();
    Color after_undo = canvas->get_pixel(3, 3);
    EXPECT_EQ(after_undo.g, 0);

    // Verify we can redo
    EXPECT_TRUE(manager->can_redo());
    EXPECT_EQ(manager->redo_description(), "Paint pixel");

    // Redo
    manager->redo();

    // Verify state matches original execution
    Color after_redo = canvas->get_pixel(3, 3);
    EXPECT_EQ(after_redo.g, 255);

    // Undo stack should have one command again
    EXPECT_TRUE(manager->can_undo());
    EXPECT_FALSE(manager->can_redo());
}

TEST_F(CommandManagerTest, NewCommandClearsRedo) {
    Color red(255, 0, 0, 255);
    Color blue(0, 0, 255, 255);

    // Execute first command
    auto cmd1 = std::make_unique<PixelCommand>(canvas.get(), 1, 1, red);
    manager->execute(std::move(cmd1));

    // Undo
    manager->undo();
    EXPECT_TRUE(manager->can_redo());

    // Execute new command - should clear redo stack
    auto cmd2 = std::make_unique<PixelCommand>(canvas.get(), 2, 2, blue);
    manager->execute(std::move(cmd2));

    // Redo should no longer be available
    EXPECT_FALSE(manager->can_redo());
    EXPECT_TRUE(manager->can_undo());
}
