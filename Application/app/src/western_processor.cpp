#include "western_processor.hpp"

#include <iostream>

namespace app {

WesternProcessor::WesternProcessor() {
    InitializeStateMachine();
}

void WesternProcessor::InitializeStateMachine() {
    m_state_machine.AddTransition(ProcessorState::Idle,       EventType::PowerIgnitionOn,          ProcessorState::IgnitionOn, [this]() { OnIgnitionOn(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::ApplicationStartRequest,  ProcessorState::Running,    [this]() { OnStartRequest(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::ApplicationRecovery,      ProcessorState::Recovery,   [this]() { OnRecovery(); });
    m_state_machine.AddTransition(ProcessorState::Running,    EventType::ApplicationStopRequest,   ProcessorState::Stopped,    [this]() { OnStopRequest(); });
    m_state_machine.AddTransition(ProcessorState::Recovery,   EventType::ApplicationStopRequest,   ProcessorState::Stopped,    [this]() { OnStopRequest(); });

    // // Allow IgnitionOff from any state to return to Idle
    m_state_machine.AddIgnitionOffTransition(EventType::PowerIgnitionOff, ProcessorState::Idle,       [this]() { OnIgnitionOff(); });
}

void WesternProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[WesternProcessor] Handling event: %s, raw_payload= %s", ToString(message.event).c_str(), message.raw_payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

void WesternProcessor::OnIgnitionOn() {
    LOGD("[WesternProcessor] Initialize RegionOne-specific ignition flow");
}

void WesternProcessor::OnIgnitionOff() {
    LOGD("[WesternProcessor] Initialize RegionOne-specific ignition flow");
}

} // namespace app
