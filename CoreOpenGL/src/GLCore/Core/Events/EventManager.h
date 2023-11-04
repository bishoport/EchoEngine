#pragma once
#include <functional>
#include <vector>
#include <algorithm>

template <typename... Args>
class Event {
public:
    using CallbackType = std::function<void(Args...)>;

    void subscribe(const CallbackType& callback) {
        callbacks.push_back(callback);
    }

    void unsubscribe(const CallbackType& callback) {
        auto it = std::find(callbacks.begin(), callbacks.end(), callback);
        if (it != callbacks.end()) {
            callbacks.erase(it);
        }
    }

    void trigger(Args... args) const {
        for (const auto& callback : callbacks) {
            callback(args...);
        }
    }

private:
    std::vector<CallbackType> callbacks;
};




class EventManager {
public:

    static Event<int, int>& getWindowResizeEvent() {
        static Event<int, int> event;
        return event;
    }

    static Event<const std::string&, const ImVec2&>& getOnPanelResizedEvent() {
        static Event<const std::string&, const ImVec2&> event;
        return event;
    }


    static Event<const bool&>& getOnEndDeleteEntity() {
        static Event<const bool&> event;
        return event;
    }

};
