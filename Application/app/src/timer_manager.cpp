#include "timer_manager.hpp"

#include "logger.hpp"

#include <thread>

namespace app {

TimerManager::TimerManager(EventPoster event_poster)
    : m_state(std::make_shared<SharedState>())
{
    m_state->event_poster = std::move(event_poster);
}

TimerManager::~TimerManager()
{
    CancelAll();
}

void TimerManager::StartTimer(TimerId timer_id,
                              std::chrono::milliseconds duration,
                              EventType timeout_event)
{
    const auto state = m_state;

    std::uint64_t generation = 0;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        generation = ++state->generation_map[timer_id];
    }

    LOGD("[TimerManager] Start timer_id=%d, duration_ms=%lld, timeout_event=%s",
         static_cast<int>(timer_id),
         static_cast<long long>(duration.count()),
         ToString(timeout_event).c_str());

    std::thread([state, timer_id, generation, duration, timeout_event]() {
        std::this_thread::sleep_for(duration);

        if (state->stopped.load()) {
            return;
        }

        bool current_generation = false;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            const auto it = state->generation_map.find(timer_id);
            current_generation = (it != state->generation_map.end() && it->second == generation);
        }

        if (!current_generation) {
            LOGD("[TimerManager] Timer ignored. timer_id=%d was cancelled or restarted",
                 static_cast<int>(timer_id));
            return;
        }

        LOGD("[TimerManager] Timer expired. timer_id=%d, event=%s",
             static_cast<int>(timer_id),
             ToString(timeout_event).c_str());

        if (state->event_poster) {
            AppMessage message;
            message.service = ServiceType::Internal;
            message.event = timeout_event;
            message.raw_payload = "timer expired";
            state->event_poster(message);
        }
    }).detach();
}

void TimerManager::CancelTimer(TimerId timer_id)
{
    const auto state = m_state;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        ++state->generation_map[timer_id];
    }

    LOGD("[TimerManager] Cancel timer_id=%d", static_cast<int>(timer_id));
}

void TimerManager::CancelAll()
{
    const auto state = m_state;
    state->stopped = true;

    {
        std::lock_guard<std::mutex> lock(state->mutex);
        for (auto& item : state->generation_map) {
            ++item.second;
        }
    }

    LOGD("[TimerManager] Cancel all timers");
}

} // namespace app
