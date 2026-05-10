#pragma once

#include <atomic>
#include <thread>

#include "event_handler.hpp"
#include "message_queue.hpp"
#include "processor_manager.hpp"
#include "timer_manager.hpp"
#include "types.hpp"

namespace app {

class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    bool Initialize();
    bool Start();
    void RequestStop();
    int Wait();

private:
    void RegisterCallbacks();
    bool ConnectServices();
    void StartAppThread();
    void AppThreadLoop();
    void ProcessMessage(const AppMessage& message);
    void StopInternal();
    Region ParseRegionPayload(const AppMessage& message) const;

private:
    std::atomic<bool> m_running {false};
    std::atomic<bool> m_stop_requested {false};

    MessageQueue<AppMessage> m_queue;
    EventHandler m_event_handler;
    TimerManager m_timer_manager;
    ProcessorManager& m_processor_manager;

    std::thread m_app_thread;
    int m_exit_code {0};
};

} // namespace app
