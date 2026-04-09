#include "hook_system.h"

namespace convoy {

HookSystem::HookHandle HookSystem::register_hook(HookPoint point, HookFn fn) {
    HookHandle h = next_++;
    hooks_[static_cast<int>(point)].push_back({h, std::move(fn)});
    return h;
}

void HookSystem::unregister_hook(HookPoint point, HookHandle handle) {
    auto& vec = hooks_[static_cast<int>(point)];
    vec.erase(std::remove_if(vec.begin(), vec.end(),
        [handle](const Entry& e) { return e.handle == handle; }), vec.end());
}

bool HookSystem::fire(HookPoint point) const {
    auto it = hooks_.find(static_cast<int>(point));
    if (it == hooks_.end()) return true;
    for (const auto& e : it->second)
        if (e.fn() == HookResult::SuppressOriginal) return false;
    return true;
}

}
