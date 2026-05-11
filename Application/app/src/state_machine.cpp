#include "state_machine.hpp"

namespace app {

void StateMachine::AddTransition(ProcessorState current, EventType event, ProcessorState next, Action action)
{
    m_transitions.push_back({current, event, next, std::move(action)});
}

void StateMachine::AddIgnitionOffTransition(EventType event, ProcessorState next, Action action)
{
    m_transitions.push_back({std::nullopt, event, next, std::move(action)});
}

void StateMachine::HandleEvent(EventType event)
{
    // Exact state-specific transitions have priority over global transitions.
    for (const auto& transition : m_transitions) {
        if (transition.current_state.has_value() &&
            transition.current_state.value() == m_current_state &&
            transition.event == event) {
            ExecuteTransition(transition);
            return;
        }
    }

    // Global transition fallback. Used for high-priority events like IG_OFF.
    for (const auto& transition : m_transitions) {
        if (!transition.current_state.has_value() && transition.event == event) {
            ExecuteTransition(transition);
            return;
        }
    }

    LOGE("[StateMachine] No transition found for state=%s, event=%s",
         ToString(m_current_state).c_str(),
         ToString(event).c_str());
}

void StateMachine::ExecuteTransition(const StateTransition& transition)
{
    LOGD("[StateMachine] Transition %s -> %s by event=%s",
         ToString(m_current_state).c_str(),
         ToString(transition.next_state).c_str(),
         ToString(transition.event).c_str());

    if (transition.action) {
        transition.action();
    }

    m_current_state = transition.next_state;
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
        case ProcessorState::Stopping: return "Stopping";
        case ProcessorState::WaitingAudioComplete: return "WaitingAudioComplete";
        case ProcessorState::Stopped: return "Stopped";
        default: return "Unknown";
    }
}

} // namespace app
