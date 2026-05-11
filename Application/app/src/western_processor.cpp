#include "western_processor.hpp"
#include "audio_wrapper.hpp"

namespace app {

WesternProcessor::WesternProcessor() {
    InitializeStateMachine();
}

void WesternProcessor::InitializeStateMachine() {
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

    m_state_machine.AddTransition(ProcessorState::Running,
                                  EventType::ApplicatiOnTimeoutEvent,
                                  ProcessorState::Recovery,
                                  [this]() { OnTimeoutEvent(); });

    // Global transition: IG_OFF is valid from any current state.
    m_state_machine.AddIgnitionOffTransition(EventType::PowerIgnitionOff,
                                             ProcessorState::Idle,
                                             [this]() { OnIgnitionOff(); });
}

void WesternProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[WesternProcessor] Handling event=%s, raw_payload=%s",
         ToString(message.event).c_str(),
         message.raw_payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

void WesternProcessor::OnIgnitionOn() {
    LOGD("[WesternProcessor] Common ignition ON flow");
}

void WesternProcessor::OnIgnitionOff() {
    LOGD("[WesternProcessor] Common ignition OFF cleanup");
    CancelAllTimers();
}

void WesternProcessor::OnStopCompleted() {
    LOGD("[WesternProcessor] Stop request completed. Request AudioService to play completion sound");
    AudioWrapper::GetInstance().RequestPlayStopCompletedSound();
}

void WesternProcessor::OnAudioPlayCompleted() {
    LOGD("[WesternProcessor] Audio play completed. Final state is Idle");
}

void WesternProcessor::OnTimeoutEvent() {
    LOGD("[WesternProcessor] Running timeout occurred. Move to recovery flow");
}

} // namespace app
