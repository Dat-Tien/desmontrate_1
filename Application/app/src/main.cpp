#include "application.hpp"
#include "logger.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace {

std::atomic<bool> g_stop_requested{false};

void SignalHandler(int)
{
    g_stop_requested = true;
}

} // namespace

int main()
{
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    app::Application application;

    if (!application.Initialize()) {
        LOGE("[Main] Application initialization failed");
        return 1;
    }

    if (!application.Start()) {
        LOGE("[Main] Application start failed");
        return 1;
    }

    while (!g_stop_requested.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    LOGD("[Main] Stop signal received");
    application.RequestStop();

    return application.Wait();
}
