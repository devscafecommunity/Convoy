#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

namespace convoy
{

enum class HookResult
{
    Continue,
    SuppressOriginal
};
enum class HookPoint : int
{
    PreCanvasRender = 0,
    PostCanvasRender = 1,
    PreWindowRender = 2,
    OnProjectSave = 3,
    OnExport = 4,
};

class HookSystem
{
   public:
    using HookFn = std::function<HookResult()>;
    using HookHandle = uint64_t;

    HookHandle register_hook(HookPoint point, HookFn fn);
    void unregister_hook(HookPoint point, HookHandle handle);
    bool fire(HookPoint point) const;

   private:
    struct Entry
    {
        HookHandle handle;
        HookFn fn;
    };
    std::unordered_map<int, std::vector<Entry>> hooks_;
    HookHandle next_ = 1;
};

}  // namespace convoy
