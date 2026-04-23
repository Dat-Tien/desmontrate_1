#include "state_machine.hpp"

namespace app {

void StateMachine::AddTransition(ProcessorState current, EventType event, ProcessorState next, Action action)
{
    m_transitions.push_back({current, event, next, std::move(action)});
}

void StateMachine::HandleEvent(EventType event)
{
    for (const auto& transition : m_transitions) {
        if (transition.current_state == m_current_state && transition.event == event) {
            LOGD("[StateMachine] Transition %s -> %s", ToString(m_current_state).c_str(), ToString(transition.next_state).c_str());
            if (transition.action) {
                transition.action();
            }

            m_current_state = transition.next_state;
            return;
        }
    }

    LOGE("[StateMachine] No transition found for state= %s, event= %s", ToString(m_current_state).c_str(), ToString(event).c_str());
}

ProcessorState StateMachine::GetCurrentState() const {
    return m_current_state;
}

std::string ToString(ProcessorState state) {
    switch (state) {
        case ProcessorState::Idle: return "Idle";
        case ProcessorState::IgnitionOn: return "IgnitionOn";
        case ProcessorState::Running: return "Running";
        case ProcessorState::Recovery: return "Recovery";
        case ProcessorState::Stopped: return "Stopped";
        default: return "Unknown";
    }
}

} // namespace app
