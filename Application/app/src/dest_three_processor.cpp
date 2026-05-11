#include "dest_three_processor.hpp"
#include "audio_wrapper.hpp"

namespace app {

DestThreeProcessor::DestThreeProcessor() {
    InitializeStateMachine();
}

void DestThreeProcessor::InitializeStateMachine() {
    m_state_machine.AddTransition(ProcessorState::Idle,
                                  EventType::PowerIgnitionOn,
                                  ProcessorState::IgnitionOn,
                                  [this]() { OnIgnitionOn(); });

    m_state_machine.AddTransition(ProcessorState::IgnitionOn,
                                  EventType::ApplicationStartRequest,
                                  ProcessorState::Running,
                                  [this]() { OnStartRequest(); });

    m_state_machine.AddTransition(ProcessorState::IgnitionOn,
                                  EventType::ApplicationRecovery,
                                  ProcessorState::Recovery,
                                  [this]() { OnRecovery(); });

    m_state_machine.AddTransition(ProcessorState::Running,
                                  EventType::ApplicationStopRequest,
                                  ProcessorState::Stopping,
                                  [this]() { OnStopRequest(); });

    m_state_machine.AddTransition(ProcessorState::Recovery,
                                  EventType::ApplicationStopRequest,
                                  ProcessorState::Stopping,
                                  [this]() { OnStopRequest(); });

    m_state_machine.AddTransition(ProcessorState::Stopping,
                                  EventType::ApplicationStopRequestTimeout,
                                  ProcessorState::WaitingAudioComplete,
                                  [this]() { OnStopCompleted(); });

    m_state_machine.AddTransition(ProcessorState::WaitingAudioComplete,
                                  EventType::AudioPlayCompleted,
                                  ProcessorState::Idle,
                                  [this]() { OnAudioPlayCompleted(); });

    // Allow IgnitionOff from any state to return to Idle.
    m_state_machine.AddIgnitionOffTransition(EventType::PowerIgnitionOff,
                                             ProcessorState::Idle,
                                             [this]() { OnIgnitionOff(); });
}

void DestThreeProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[DestThreeProcessor] Handling event=%s, raw_payload=%s",
         ToString(message.event).c_str(),
         message.raw_payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region DestThreeProcessor::GetRegion() const {
    return Region::RegionThree;
}

void DestThreeProcessor::OnIgnitionOn() {
    LOGD("[DestThreeProcessor] IG_ON received. State moved to IgnitionOn. Waiting for HMI StartRequest.");
}

void DestThreeProcessor::OnIgnitionOff() {
    LOGD("[DestThreeProcessor] IG_OFF received. Cancel timers and reset to Idle.");
    CancelAllTimers();
}

void DestThreeProcessor::OnStartRequest() {
    LOGD("[DestThreeProcessor] HMI StartRequest handled. Application is now Running.");
}

void DestThreeProcessor::OnRecovery() {
    LOGD("[DestThreeProcessor] Execute RegionThree recovery logic");
    StartTimer(TimerId::ApplicationRecoveryTimer);
}

void DestThreeProcessor::OnStopRequest() {
    LOGD("[DestThreeProcessor] HMI StopRequest handled. Start 5s stop-completion timer.");
    StartTimer(TimerId::ApplicationStopRequestTimer);
}

void DestThreeProcessor::OnStopCompleted() {
    LOGD("[DestThreeProcessor] Stop operation completed. Request AudioService to play completion sound.");
    AudioWrapper::GetInstance().RequestPlayStopCompletedSound();
}

void DestThreeProcessor::OnAudioPlayCompleted() {
    LOGD("[DestThreeProcessor] Audio completed callback received. State returned to Idle.");
}

void DestThreeProcessor::OnTimeoutEvent() {
    LOGD("[DestThreeProcessor] OnTimeoutEvent");
}

} // namespace app
