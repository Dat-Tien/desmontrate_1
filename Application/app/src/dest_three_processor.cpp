#include "dest_three_processor.hpp"

#include <iostream>

namespace app {

DestThreeProcessor::DestThreeProcessor() {
    InitializeStateMachine();
}

void DestThreeProcessor::InitializeStateMachine() {
    m_state_machine.AddTransition(ProcessorState::Idle,       EventType::PowerIgnitionOn,          ProcessorState::IgnitionOn, [this]() { OnIgnitionOn(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::ApplicationStartRequest,  ProcessorState::Running,    [this]() { OnStartRequest(); });
    m_state_machine.AddTransition(ProcessorState::IgnitionOn, EventType::ApplicationRecovery,      ProcessorState::Recovery,   [this]() { OnRecovery(); });
    m_state_machine.AddTransition(ProcessorState::Running,    EventType::ApplicationStopRequest,   ProcessorState::Stopped,    [this]() { OnStopRequest(); });
    m_state_machine.AddTransition(ProcessorState::Recovery,   EventType::ApplicationStopRequest,   ProcessorState::Stopped,    [this]() { OnStopRequest(); });
    m_state_machine.AddTransition(ProcessorState::Stopped,    EventType::PowerIgnitionOff,         ProcessorState::Idle,       [this]() { OnIgnitionOff(); });
}

void DestThreeProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[DestThreeProcessor] Handling event: %s , payload= %s" ,ToString(message.event).c_str(), message.payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region DestThreeProcessor::GetRegion() const {
    return Region::RegionThree;
}

void DestThreeProcessor::OnIgnitionOn() {
    LOGD("[DestThreeProcessor] Initialize JP-specific ignition flow");
}

void DestThreeProcessor::OnIgnitionOff() {
    LOGD("[DestThreeProcessor] Initialize JP-specific ignition off flow");
}

void DestThreeProcessor::OnStartRequest() {
    LOGD("[DestThreeProcessor] Execute JP start request logic");
}

void DestThreeProcessor::OnRecovery() {
    LOGD("[DestThreeProcessor] Execute JP recovery logic");
}

void DestThreeProcessor::OnStopRequest() {
    LOGD("[DestThreeProcessor] Execute JP stop logic");
}

} // namespace app