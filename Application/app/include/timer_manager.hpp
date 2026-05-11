#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "types.hpp"

namespace app {

enum class TimerId : uint32_t {
    // Testing purpose only
    TestingApplicationStartRequestTimer,
    // Application Timeout events
    ApplicationStartRequestTimer,
    ApplicationRecoveryTimer,
    ApplicationStopRequestTimer,
    ApplicationResetRequestTimeout,
    ApplicatiOnTimeoutEvent,
    ApplicationHeartbeatTimeout,
    RunningTimeout,
    RecoveryTimeout,
    HeartbeatTimeout
};

class ITimerManager {
public:
    virtual ~ITimerManager() = default;

    // Timer details are configured inside TimerManager.
    // Processors only need to know which timer they want to start.
    virtual void StartTimer(TimerId timer_id) = 0;
    virtual void PauseTimer(TimerId timer_id) = 0;
    virtual void ResumeTimer(TimerId timer_id) = 0;
    virtual void CancelTimer(TimerId timer_id) = 0;
    virtual void CancelAll() = 0;
    virtual void Stop() = 0;
};

class TimerManager final : public ITimerManager {
public:
    explicit TimerManager(EventPoster event_poster);
    ~TimerManager() override;

    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;

    void StartTimer(TimerId timer_id) override;
    void PauseTimer(TimerId timer_id) override;
    void ResumeTimer(TimerId timer_id) override;
    void CancelTimer(TimerId timer_id) override;
    void CancelAll() override;
    void Stop() override;

private:
    struct TimerConfig {
        std::chrono::milliseconds duration{0};
        EventType timeout_event{EventType::Unknown};
    };

    struct TimerContext {
        std::mutex mutex;
        std::condition_variable cv;
        std::uint64_t generation{0};
        bool active{false};
        bool paused{false};
        std::chrono::milliseconds remaining{0};
        std::chrono::steady_clock::time_point deadline{};
    };

    struct SharedState {
        std::mutex mutex;
        std::unordered_map<TimerId, TimerConfig> config_map;
        std::unordered_map<TimerId, std::shared_ptr<TimerContext>> timer_map;
        EventPoster event_poster;
        std::atomic<bool> stopped{false};
    };

    void InitializeDefaultTimers();
    std::shared_ptr<TimerContext> GetOrCreateTimerContext(TimerId timer_id);
    bool GetTimerConfig(TimerId timer_id, TimerConfig& out_config) const;
    void StartWorker(TimerId timer_id,
                     std::shared_ptr<TimerContext> context,
                     TimerConfig config,
                     std::uint64_t generation);

private:
    std::shared_ptr<SharedState> m_state;
};

} // namespace app
