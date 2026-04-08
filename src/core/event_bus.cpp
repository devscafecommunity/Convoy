#include "event_bus.h"

namespace convoy {

void EventBus::subscribe(const std::string& event_type, EventCallback callback) {
    subscribers_[event_type].push_back(callback);
}

void EventBus::publish(const Event& event) {
    auto it = subscribers_.find(event.type());
    if (it != subscribers_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
}

void EventBus::clear_subscriptions() {
    subscribers_.clear();
}

}
