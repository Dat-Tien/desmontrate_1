#include "jp_processor.hpp"

#include <iostream>

namespace app {

JPProcessor::JPProcessor() {
    InitializeStateMachine();
}

void JPProcessor::InitializeStateMachine() {
    m_state_machine.AddTransition(ProcessorState::Idle,       EventType::IgnitionOn,    ProcessorState::IgnitionOn, [this]() { OnIgnitionOn(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::StartRequest,  ProcessorState::Running,    [this]() { OnStartRequest(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::StartRecovery, ProcessorState::Recovery,   [this]() { OnRecovery(); });
    m_state_machine.AddTransition(ProcessorState::Running,    EventType::StopRequest,   ProcessorState::Stopped,    [this]() { OnStopRequest(); });
}

void JPProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[JPProcessor] Handling event: %s , payload= %s" ,ToString(message.event).c_str(), message.payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region JPProcessor::GetRegion() const {
    return Region::JP;
}

void JPProcessor::OnIgnitionOn() {
    LOGD("[JPProcessor] Initialize JP-specific ignition flow");
}

void JPProcessor::OnStartRequest() {
    LOGD("[JPProcessor] Execute JP start request logic");
}

void JPProcessor::OnRecovery() {
    LOGD("[JPProcessor] Execute JP recovery logic");
}

void JPProcessor::OnStopRequest() {
    LOGD("[JPProcessor] Execute JP stop logic");
}

} // namespace app