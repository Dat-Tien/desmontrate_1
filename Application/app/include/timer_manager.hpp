#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "types.hpp"

namespace app {

enum class TimerId {
    RunningTimeout,
    RecoveryTimeout,
    HeartbeatTimeout
};

class ITimerManager {
public:
    virtual ~ITimerManager() = default;

    virtual void StartTimer(TimerId timer_id,
                            std::chrono::milliseconds duration,
                            EventType timeout_event) = 0;

    virtual void CancelTimer(TimerId timer_id) = 0;
    virtual void CancelAll() = 0;
};

class TimerManager final : public ITimerManager {
public:
    explicit TimerManager(EventPoster event_poster);
    ~TimerManager() override;

    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;

    void StartTimer(TimerId timer_id,
                    std::chrono::milliseconds duration,
                    EventType timeout_event) override;

    void CancelTimer(TimerId timer_id) override;
    void CancelAll() override;

private:
    struct SharedState {
        std::mutex mutex;
        std::unordered_map<TimerId, std::uint64_t> generation_map;
        EventPoster event_poster;
        std::atomic<bool> stopped{false};
    };

    std::shared_ptr<SharedState> m_state;
};

} // namespace app
