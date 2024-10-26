//
// Created by robko on 10/26/24.
//

#pragma once

#include <vector>
#include <algorithm>
#include "assertions.h"

template<typename Action>
class Observable {
public:
    virtual void update(const Action &action) = 0;

    virtual ~Observable() = default;
};

template<typename Action>
class Subject {
private:
    std::vector<std::shared_ptr<Observable<Action>>> observers;
    Action lastAction;
public:
    explicit Subject(Action lastAction) : lastAction(lastAction) {
        observers = std::vector<std::shared_ptr<Observable<Action>>>();
    }

    void attach(const std::shared_ptr<Observable<Action>> &observable) {
        DEBUG_ASSERT(nullptr != observable.get())
        DEBUG_ASSERT(observable.use_count() > 1)
        observable->update(lastAction);
        observers.emplace_back(observable);
    }

    void detach(const std::shared_ptr<Observable<Action>> &observable) {
        auto it = std::find(observers.begin(), observers.end(), observable);
        DEBUG_ASSERTF(it != observers.end(), "This observer was never registered")
        observers.erase(it);
    }

    void notify(const Action &action) {
        lastAction = action;
        for (const std::shared_ptr<Observable<Action>> &item: observers) {
#ifdef DEBUG_ASSERTIONS
            DEBUG_ASSERT(nullptr != item.get())
            DEBUG_ASSERTF(item.use_count() > 1,
                          "Use count is 1, which means that we are the only one holding the reference and someone forgot to unregister this observer")
#endif
            item->update(action);
        }
    }

    virtual ~Subject() = default;
};