#include "western_processor.hpp"

#include <iostream>

namespace app {

WesternProcessor::WesternProcessor() {
    InitializeStateMachine();
}

void WesternProcessor::InitializeStateMachine() {
    m_state_machine.AddTransition(ProcessorState::Idle,       EventType::IgnitionOn,    ProcessorState::IgnitionOn, [this]() { OnIgnitionOn(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::StartRequest,  ProcessorState::Running,    [this]() { OnStartRequest(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::StartRecovery, ProcessorState::Recovery,   [this]() { OnRecovery(); });
    m_state_machine.AddTransition(ProcessorState::Running,    EventType::StopRequest,   ProcessorState::Stopped,    [this]() { OnStopRequest(); });
}

void WesternProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[WesternProcessor] Handling event: %s, payload= %s", ToString(message.event).c_str(), message.payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

void WesternProcessor::OnIgnitionOn() {
    LOGD("[WesternProcessor] Initialize RegionOne-specific ignition flow");
}

} // namespace app
