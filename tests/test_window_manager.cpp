#include <gtest/gtest.h>

#include "core/event_bus.h"
#include "core/window_manager.h"

using namespace convoy;

// ===== Event Bus Tests =====

TEST(EventBusTest, SubscribeAndPublish)
{
    EventBus bus;
    int callback_count = 0;

    bus.subscribe("TestEvent", [&callback_count](const Event& e) { callback_count++; });

    // Create a simple test event
    struct TestEvent : Event
    {
        std::string type() const override { return "TestEvent"; }
    };

    TestEvent event;
    bus.publish(event);

    EXPECT_EQ(callback_count, 1);
}

TEST(EventBusTest, MultipleSubscribers)
{
    EventBus bus;
    int count1 = 0, count2 = 0;

    bus.subscribe("Event1", [&count1](const Event& e) { count1++; });
    bus.subscribe("Event1", [&count2](const Event& e) { count2++; });

    struct Event1 : Event
    {
        std::string type() const override { return "Event1"; }
    };

    Event1 event;
    bus.publish(event);

    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
}

TEST(EventBusTest, CanvasModifiedEvent)
{
    EventBus bus;
    int layer_id_received = -1;

    bus.subscribe("CanvasModified",
                  [&layer_id_received](const Event& e)
                  {
                      const CanvasModifiedEvent& evt = static_cast<const CanvasModifiedEvent&>(e);
                      layer_id_received = evt.layer_id;
                  });

    CanvasModifiedEvent event;
    event.layer_id = 42;
    bus.publish(event);

    EXPECT_EQ(layer_id_received, 42);
}

TEST(EventBusTest, ToolSelectedEvent)
{
    EventBus bus;
    int tool_id_received = -1;

    bus.subscribe("ToolSelected",
                  [&tool_id_received](const Event& e)
                  {
                      const ToolSelectedEvent& evt = static_cast<const ToolSelectedEvent&>(e);
                      tool_id_received = evt.tool_id;
                  });

    ToolSelectedEvent event;
    event.tool_id = 3;
    bus.publish(event);

    EXPECT_EQ(tool_id_received, 3);
}

TEST(EventBusTest, ClearSubscriptions)
{
    EventBus bus;
    int callback_count = 0;

    bus.subscribe("Event1", [&callback_count](const Event& e) { callback_count++; });
    bus.subscribe("Event2", [&callback_count](const Event& e) { callback_count++; });

    bus.clear_subscriptions();

    struct Event1 : Event
    {
        std::string type() const override { return "Event1"; }
    };

    Event1 event;
    bus.publish(event);

    EXPECT_EQ(callback_count, 0);
}

// ===== Window Manager Tests =====

TEST(WindowManagerTest, InitializationBasics)
{
    WindowManager wm;
    // Just verify object construction doesn't crash
    EXPECT_TRUE(true);
}

TEST(WindowManagerTest, Create)
{
    WindowManager wm;
    // Just verify object construction doesn't crash
    EXPECT_TRUE(true);
}

TEST(WindowManagerTest, ShouldCloseBeforeInit)
{
    WindowManager wm;
    // Window not initialized, should_close should handle gracefully
    bool should_close = wm.should_close();
    EXPECT_TRUE(should_close);
}
