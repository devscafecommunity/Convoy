#include <gtest/gtest.h>
#include "core/input/input_handler.h"
#include <GLFW/glfw3.h>

using namespace convoy;

TEST(InputHandlerTest, BindAndFire) {
    InputHandler handler;
    bool fired = false;
    handler.bind("tool.pencil", GLFW_KEY_B);
    handler.register_command("tool.pencil", [&]() { fired = true; });
    handler.process_key(GLFW_KEY_B, GLFW_PRESS, 0);
    EXPECT_TRUE(fired);
}

TEST(InputHandlerTest, ModifierRequirement) {
    InputHandler handler;
    bool fired = false;
    handler.bind("project.save", GLFW_KEY_S, GLFW_MOD_CONTROL);
    handler.register_command("project.save", [&]() { fired = true; });
    handler.process_key(GLFW_KEY_S, GLFW_PRESS, 0);
    EXPECT_FALSE(fired);
    handler.process_key(GLFW_KEY_S, GLFW_PRESS, GLFW_MOD_CONTROL);
    EXPECT_TRUE(fired);
}

TEST(InputHandlerTest, ReleaseIgnored) {
    InputHandler handler;
    bool fired = false;
    handler.bind("tool.eraser", GLFW_KEY_E);
    handler.register_command("tool.eraser", [&]() { fired = true; });
    handler.process_key(GLFW_KEY_E, GLFW_RELEASE, 0);
    EXPECT_FALSE(fired);
}

TEST(InputHandlerTest, SetupDefaults) {
    InputHandler handler;
    handler.setup_defaults();
    // defaults registered without crashing
    SUCCEED();
}
