#include "timer_manager.hpp"

#include "logger.hpp"

#include <algorithm>
#include <thread>

namespace app {

namespace {

const char* ToTimerName(TimerId timer_id)
{
    switch (timer_id) {
        case TimerId::TestingApplicationStartRequestTimer: return "TestingApplicationStartRequestTimer";
        case TimerId::ApplicationStartRequestTimer: return "ApplicationStartRequestTimer";
        case TimerId::ApplicationRecoveryTimer: return "ApplicationRecoveryTimer";
        case TimerId::ApplicationStopRequestTimer: return "ApplicationStopRequestTimer";
        case TimerId::ApplicationResetRequestTimeout: return "ApplicationResetRequestTimeout";
        case TimerId::ApplicatiOnTimeoutEvent: return "ApplicatiOnTimeoutEvent";
        case TimerId::ApplicationHeartbeatTimeout: return "ApplicationHeartbeatTimeout";
        case TimerId::RunningTimeout: return "RunningTimeout";
        case TimerId::RecoveryTimeout: return "RecoveryTimeout";
        case TimerId::HeartbeatTimeout: return "HeartbeatTimeout";
    }
    return "UnknownTimer";
}

} // namespace

TimerManager::TimerManager(EventPoster event_poster)
    : m_state(std::make_shared<SharedState>())
{
    m_state->event_poster = std::move(event_poster);
    InitializeDefaultTimers();
}

TimerManager::~TimerManager()
{
    Stop();
}

void TimerManager::InitializeDefaultTimers()
{
    std::lock_guard<std::mutex> lock(m_state->mutex);

    // Centralized timer definition table.
    // Processors call StartTimer(timer_id), while TimerManager owns the duration/event mapping.
    m_state->config_map[TimerId::ApplicationStartRequestTimer] = {
        std::chrono::seconds(15),
        EventType::ApplicationStartRequestTimeout
    };

    m_state->config_map[TimerId::ApplicationRecoveryTimer] = {
        std::chrono::seconds(10),
        EventType::ApplicationRecoveryTimeout
    };

    m_state->config_map[TimerId::ApplicationStopRequestTimer] = {
        std::chrono::seconds(5),
        EventType::ApplicationStopRequestTimeout
    };

    m_state->config_map[TimerId::RunningTimeout] = {
        std::chrono::seconds(5),
        EventType::ApplicatiOnTimeoutEvent
    };

    m_state->config_map[TimerId::HeartbeatTimeout] = {
        std::chrono::seconds(10),
        EventType::ApplicationHeartbeatTimeout
    };

    // Testing purpose
    m_state->config_map[TimerId::TestingApplicationStartRequestTimer] = {
        std::chrono::seconds(10),
        EventType::TestingApplicationStartRequestTimer
    };

}

std::shared_ptr<TimerManager::TimerContext> TimerManager::GetOrCreateTimerContext(TimerId timer_id)
{
    std::lock_guard<std::mutex> lock(m_state->mutex);

    auto it = m_state->timer_map.find(timer_id);
    if (it != m_state->timer_map.end()) {
        return it->second;
    }

    auto context = std::make_shared<TimerContext>();
    m_state->timer_map.emplace(timer_id, context);
    return context;
}

bool TimerManager::GetTimerConfig(TimerId timer_id, TimerConfig& out_config) const
{
    std::lock_guard<std::mutex> lock(m_state->mutex);

    const auto it = m_state->config_map.find(timer_id);
    if (it == m_state->config_map.end()) {
        return false;
    }

    out_config = it->second;
    return true;
}

void TimerManager::StartTimer(TimerId timer_id)
{
    if (m_state->stopped.load()) {
        LOGD("[TimerManager] Ignore StartTimer after manager stopped. timer_id=%s", ToTimerName(timer_id));
        return;
    }

    TimerConfig config{};
    if (!GetTimerConfig(timer_id, config)) {
        LOGE("[TimerManager] Timer is not configured. timer_id=%s", ToTimerName(timer_id));
        return;
    }

    const auto context = GetOrCreateTimerContext(timer_id);

    std::uint64_t generation = 0;
    {
        std::lock_guard<std::mutex> lock(context->mutex);
        generation = ++context->generation;
        context->active = true;
        context->paused = false;
        context->remaining = config.duration;
        context->deadline = std::chrono::steady_clock::now() + config.duration;
    }
    context->cv.notify_all();

    LOGD("[TimerManager] Start timer_id=%s, duration_ms=%lld, timeout_event=%s",
         ToTimerName(timer_id),
         static_cast<long long>(config.duration.count()),
         ToString(config.timeout_event).c_str());

    StartWorker(timer_id, context, config, generation);
}

void TimerManager::PauseTimer(TimerId timer_id)
{
    const auto context = GetOrCreateTimerContext(timer_id);

    std::lock_guard<std::mutex> lock(context->mutex);
    if (!context->active || context->paused) {
        LOGD("[TimerManager] Pause ignored. timer_id=%s active=%d paused=%d",
             ToTimerName(timer_id),
             context->active,
             context->paused);
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (now >= context->deadline) {
        context->remaining = std::chrono::milliseconds(0);
    } else {
        context->remaining = std::chrono::duration_cast<std::chrono::milliseconds>(context->deadline - now);
    }

    context->paused = true;
    context->cv.notify_all();

    LOGD("[TimerManager] Pause timer_id=%s, remaining_ms=%lld",
         ToTimerName(timer_id),
         static_cast<long long>(context->remaining.count()));
}

void TimerManager::ResumeTimer(TimerId timer_id)
{
    const auto context = GetOrCreateTimerContext(timer_id);

    std::lock_guard<std::mutex> lock(context->mutex);
    if (!context->active || !context->paused) {
        LOGD("[TimerManager] Resume ignored. timer_id=%s active=%d paused=%d",
             ToTimerName(timer_id),
             context->active,
             context->paused);
        return;
    }

    context->paused = false;
    context->deadline = std::chrono::steady_clock::now() + context->remaining;
    context->cv.notify_all();

    LOGD("[TimerManager] Resume timer_id=%s, remaining_ms=%lld",
         ToTimerName(timer_id),
         static_cast<long long>(context->remaining.count()));
}

void TimerManager::CancelTimer(TimerId timer_id)
{
    const auto context = GetOrCreateTimerContext(timer_id);

    {
        std::lock_guard<std::mutex> lock(context->mutex);
        ++context->generation;
        context->active = false;
        context->paused = false;
        context->remaining = std::chrono::milliseconds(0);
    }
    context->cv.notify_all();

    LOGD("[TimerManager] Cancel timer_id=%s", ToTimerName(timer_id));
}

void TimerManager::CancelAll()
{
    std::lock_guard<std::mutex> state_lock(m_state->mutex);
    for (auto& item : m_state->timer_map) {
        const auto& context = item.second;
        {
            std::lock_guard<std::mutex> timer_lock(context->mutex);
            ++context->generation;
            context->active = false;
            context->paused = false;
            context->remaining = std::chrono::milliseconds(0);
        }
        context->cv.notify_all();
    }

    LOGD("[TimerManager] Cancel all active timers");
}

void TimerManager::Stop()
{
    if (m_state->stopped.exchange(true)) {
        return;
    }

    CancelAll();
    LOGD("[TimerManager] Stop timer manager");
}

void TimerManager::StartWorker(TimerId timer_id,
                               std::shared_ptr<TimerContext> context,
                               TimerConfig config,
                               std::uint64_t generation)
{
    const auto state = m_state;

    std::thread([state, timer_id, context, config, generation]() {
        bool expired = false;

        std::unique_lock<std::mutex> lock(context->mutex);
        while (!state->stopped.load()) {
            if (context->generation != generation || !context->active) {
                return;
            }

            if (context->paused) {
                context->cv.wait(lock, [&]() {
                    return state->stopped.load() ||
                           context->generation != generation ||
                           !context->active ||
                           !context->paused;
                });
                continue;
            }

            if (context->remaining.count() <= 0) {
                expired = true;
                break;
            }

            const auto wait_result = context->cv.wait_until(lock, context->deadline, [&]() {
                return state->stopped.load() ||
                       context->generation != generation ||
                       !context->active ||
                       context->paused;
            });

            if (!wait_result) {
                expired = true;
                break;
            }
        }

        if (!expired || state->stopped.load() || context->generation != generation || !context->active) {
            return;
        }

        context->active = false;
        context->paused = false;
        lock.unlock();

        LOGD("[TimerManager] Timer expired. timer_id=%s, event=%s",
             ToTimerName(timer_id),
             ToString(config.timeout_event).c_str());
        
        
        if (state->event_poster) {
            AppMessage message;
            message.service = ServiceType::ApplicationTimerTimeout;
            message.event = config.timeout_event;
            message.raw_payload = "timer expired";
            state->event_poster(message);
        }
    }).detach();
}

} // namespace app
