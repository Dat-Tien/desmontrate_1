#pragma once

#include <functional>
#include <iostream>
#include <utility>
#include <vector>

#include "types.hpp"
#include "logger.hpp"

namespace app {

using Action = std::function<void()>;

enum class ProcessorState {
    Idle,
    IgnitionOn,
    Running,
    Recovery,
    Stopped
};

std::string ToString(ProcessorState state);

struct StateTransition {
    ProcessorState current_state;
    EventType event;
    ProcessorState next_state;
    Action action;
};

class StateMachine {
public:
    explicit StateMachine(ProcessorState initial_state = ProcessorState::Idle)
        : m_current_state(initial_state) {}

    void AddTransition(ProcessorState current, EventType event, ProcessorState next, Action action);
    void HandleEvent(EventType event);
    ProcessorState GetCurrentState() const;

private:
    ProcessorState m_current_state;
    std::vector<StateTransition> m_transitions;
};

} // namespace app
