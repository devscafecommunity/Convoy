#include <gtest/gtest.h>
#include "core/plugins/hook_system.h"

using namespace convoy;

TEST(HookSystemTest, DefaultFireReturnsTrue) {
    HookSystem hs;
    EXPECT_TRUE(hs.fire(HookPoint::OnProjectSave));
}

TEST(HookSystemTest, ContinueHookAllowsOriginal) {
    HookSystem hs;
    hs.register_hook(HookPoint::OnProjectSave,
        []() { return HookResult::Continue; });
    EXPECT_TRUE(hs.fire(HookPoint::OnProjectSave));
}

TEST(HookSystemTest, SuppressHookBlocksOriginal) {
    HookSystem hs;
    hs.register_hook(HookPoint::OnProjectSave,
        []() { return HookResult::SuppressOriginal; });
    EXPECT_FALSE(hs.fire(HookPoint::OnProjectSave));
}

TEST(HookSystemTest, UnregisterRemovesHook) {
    HookSystem hs;
    auto h = hs.register_hook(HookPoint::OnExport,
        []() { return HookResult::SuppressOriginal; });
    hs.unregister_hook(HookPoint::OnExport, h);
    EXPECT_TRUE(hs.fire(HookPoint::OnExport));
}

TEST(HookSystemTest, MultipleHooksAllMustPassToContinue) {
    HookSystem hs;
    hs.register_hook(HookPoint::PreCanvasRender,
        []() { return HookResult::Continue; });
    hs.register_hook(HookPoint::PreCanvasRender,
        []() { return HookResult::SuppressOriginal; });
    EXPECT_FALSE(hs.fire(HookPoint::PreCanvasRender));
}
