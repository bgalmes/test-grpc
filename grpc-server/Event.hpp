#pragma once
#include <vector>
#include <functional>

template<typename... Args>
class Event {
public:
    using Callback = std::function<void(Args...)>;

    void subscribe(const Callback& cb) {
        listeners.push_back(cb);
    }

    void trigger(Args... args) const {
        for (const auto& listener : listeners) {
            listener(args...);
        }
    }

private:
    std::vector<Callback> listeners;
};
