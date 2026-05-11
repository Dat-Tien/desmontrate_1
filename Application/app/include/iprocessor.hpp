#pragma once

#include <chrono>

#include "state_machine.hpp"
#include "timer_manager.hpp"
#include "types.hpp"
#include "logger.hpp"

namespace app {

class IProcessor {
public:
    virtual ~IProcessor() = default;
    virtual void HandleMessage(const AppMessage& message) = 0;
    virtual Region GetRegion() const = 0;

    void SetTimerManager(ITimerManager* timer_manager) {
        m_timer_manager = timer_manager;
    }

protected:
    virtual void InitializeStateMachine() = 0;
    virtual void OnIgnitionOn() = 0;
    virtual void OnIgnitionOff() = 0;
    virtual void OnStartRequest() = 0;
    virtual void OnRecovery() = 0;
    virtual void OnStopRequest() = 0;
    virtual void OnStopCompleted() = 0;
    virtual void OnAudioPlayCompleted() = 0;
    virtual void OnTimeoutEvent() = 0;

    void StartTimer(TimerId timer_id) {
        if (m_timer_manager != nullptr) {
            m_timer_manager->StartTimer(timer_id);
        } else {
            LOGD("[IProcessor] TimerManager is not configured. timer_id=%d", static_cast<int>(timer_id));
        }
    }

    void PauseTimer(TimerId timer_id) {
        if (m_timer_manager != nullptr) {
            m_timer_manager->PauseTimer(timer_id);
        }
    }

    void ResumeTimer(TimerId timer_id) {
        if (m_timer_manager != nullptr) {
            m_timer_manager->ResumeTimer(timer_id);
        }
    }

    void CancelTimer(TimerId timer_id) {
        if (m_timer_manager != nullptr) {
            m_timer_manager->CancelTimer(timer_id);
        }
    }

    void CancelAllTimers() {
        if (m_timer_manager != nullptr) {
            m_timer_manager->CancelAll();
        }
    }

    StateMachine m_state_machine;
    ITimerManager* m_timer_manager {nullptr};
};

} // namespace app
