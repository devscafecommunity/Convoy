#include <gtest/gtest.h>
#include "core/plugins/plugin_registry.h"

using namespace convoy;

struct MockPlugin : IConvoyPlugin {
    bool loaded = false, unloaded = false, rendered = false;
    std::string plugin_name;

    MockPlugin(const std::string& n) : plugin_name(n) {}
    void on_load()   override { loaded   = true; }
    void on_unload() override { unloaded = true; }
    void on_render() override { rendered = true; }
    PluginInfo info() const override { return {plugin_name, "1.0", "test", ""}; }
};

TEST(PluginRegistryTest, RegisterCallsOnLoad) {
    PluginRegistry reg;
    auto p = std::make_shared<MockPlugin>("test");
    reg.register_plugin(p);
    EXPECT_TRUE(p->loaded);
    EXPECT_EQ(reg.count(), 1u);
}

TEST(PluginRegistryTest, UnregisterCallsOnUnload) {
    PluginRegistry reg;
    auto p = std::make_shared<MockPlugin>("test");
    reg.register_plugin(p);
    bool removed = reg.unregister_plugin("test");
    EXPECT_TRUE(removed);
    EXPECT_TRUE(p->unloaded);
    EXPECT_EQ(reg.count(), 0u);
}

TEST(PluginRegistryTest, FindByName) {
    PluginRegistry reg;
    reg.register_plugin(std::make_shared<MockPlugin>("alpha"));
    reg.register_plugin(std::make_shared<MockPlugin>("beta"));
    EXPECT_NE(reg.find("alpha"), nullptr);
    EXPECT_EQ(reg.find("gamma"), nullptr);
}

TEST(PluginRegistryTest, UnloadAllClearsRegistry) {
    PluginRegistry reg;
    reg.register_plugin(std::make_shared<MockPlugin>("a"));
    reg.register_plugin(std::make_shared<MockPlugin>("b"));
    reg.unload_all();
    EXPECT_EQ(reg.count(), 0u);
}
