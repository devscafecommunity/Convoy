#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace convoy {

struct Event {
    virtual ~Event() = default;
    virtual std::string type() const = 0;
};

struct CanvasModifiedEvent : Event {
    int layer_id;
    std::string type() const override { return "CanvasModified"; }
};

struct ToolSelectedEvent : Event {
    int tool_id;
    std::string type() const override { return "ToolSelected"; }
};

class EventBus {
public:
    using EventCallback = std::function<void(const Event&)>;
    
    void subscribe(const std::string& event_type, EventCallback callback);
    void publish(const Event& event);
    void clear_subscriptions();
    
private:
    std::unordered_map<std::string, std::vector<EventCallback>> subscribers_;
};

}
