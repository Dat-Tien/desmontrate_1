#include "application.hpp"

#include "audio_wrapper.hpp"
#include "hmi_wrapper.hpp"
#include "logger.hpp"
#include "power_wrapper.hpp"
#include "region_wrapper.hpp"

#include <thread>

namespace app {

namespace {

constexpr const char* kAudioSocketPath = "/tmp/_audio_service.sock";
constexpr const char* kHmiSocketPath = "/tmp/_hmi_service.sock";
constexpr const char* kPowerSocketPath = "/tmp/_power_service.sock";
constexpr const char* kRegionSocketPath = "/tmp/_region_service.sock";

} // namespace

Application::Application()
    : m_queue(2048),
      m_event_handler(m_queue),
      m_timer_manager([this](const AppMessage& message) {
          m_event_handler.PostEvent(message);
      }),
      m_processor_manager(ProcessorManager::GetInstance())
{
}

Application::~Application()
{
    StopInternal();
}

bool Application::Initialize()
{
    LOGD("[Application] Initialize");

    m_processor_manager.SetTimerManager(&m_timer_manager);
    RegisterCallbacks();

    if (!ConnectServices()) {
        LOGD("[Application] Some services are not connected. Continue with available services.");
    }

    return true;
}

bool Application::Start()
{
    if (m_running.exchange(true)) {
        LOGD("[Application] Already running");
        return false;
    }

    m_stop_requested = false;
    LOGD("=== Architecture Demo Start ===");
    StartAppThread();
    return true;
}

void Application::RequestStop()
{
    if (m_stop_requested.exchange(true)) {
        return;
    }

    LOGD("[Application] Stop requested");
    m_running = false;
    m_timer_manager.Stop();
    m_queue.Stop();
    StopInternal();
}

int Application::Wait()
{
    if (m_app_thread.joinable()) {
        m_app_thread.join();
    }

    LOGD("=== Architecture Demo End ===");
    return m_exit_code;
}

void Application::StopInternal()
{
    AudioWrapper::GetInstance().Stop();
    HmiWrapper::GetInstance().Stop();
    PowerWrapper::GetInstance().Stop();
    RegionWrapper::GetInstance().Stop();

    m_timer_manager.Stop();
    m_queue.Stop();

    if (m_app_thread.joinable() && m_app_thread.get_id() != std::this_thread::get_id()) {
        m_app_thread.join();
    }
}

void Application::RegisterCallbacks()
{
    HmiWrapper::GetInstance().RegisterCallback([this](const AppMessage& message) {
        m_event_handler.PostEvent(message);
    });

    AudioWrapper::GetInstance().RegisterCallback([this](const AppMessage& message) {
        m_event_handler.PostEvent(message);
    });

    RegionWrapper::GetInstance().RegisterCallback([this](const AppMessage& message) {
        m_event_handler.PostEvent(message);
    });

    PowerWrapper::GetInstance().RegisterCallback([this](const AppMessage& message) {
        m_event_handler.PostEvent(message);
    });
}

bool Application::ConnectServices()
{
    bool all_connected = true;

    if (AudioWrapper::GetInstance().Connect(kAudioSocketPath)) {
        AudioWrapper::GetInstance().StartListening();
    } else {
        LOGD("[Application] AudioService is not connected");
        all_connected = false;
    }

    if (HmiWrapper::GetInstance().Connect(kHmiSocketPath)) {
        HmiWrapper::GetInstance().StartListening();
    } else {
        LOGD("[Application] HmiService is not connected");
        all_connected = false;
    }

    if (RegionWrapper::GetInstance().Connect(kRegionSocketPath)) {
        RegionWrapper::GetInstance().StartListening();
    } else {
        LOGD("[Application] RegionService is not connected");
        all_connected = false;
    }

    if (PowerWrapper::GetInstance().Connect(kPowerSocketPath)) {
        PowerWrapper::GetInstance().StartListening();
    } else {
        LOGD("[Application] PowerService is not connected");
        all_connected = false;
    }

    return all_connected;
}

void Application::StartAppThread()
{
    m_app_thread = std::thread([this]() {
        AppThreadLoop();
    });
}

void Application::AppThreadLoop()
{
    LOGD("[Application] App thread started");

    AppMessage message{};
    while (m_running && m_queue.WaitAndPop(message)) {
        ProcessMessage(message);
    }

    LOGD("[Application] App thread exiting");
}

void Application::ProcessMessage(const AppMessage& message)
{
    if (message.event == EventType::Shutdown) {
        LOGD("[Application] Shutdown event received");
        RequestStop();
        return;
    }

    if (message.event == EventType::RegionChanged) {
        const Region region = ParseRegionPayload(message);
        if (region == Region::Unknown) {
            LOGD("[Application] RegionChanged event with invalid payload: %s",
                 message.raw_payload.c_str());
            return;
        }

        m_processor_manager.SetRegion(region);
        return;
    }

    m_processor_manager.HandleMessage(message);
}

Region Application::ParseRegionPayload(const AppMessage& message) const
{
    if (message.payloads.empty()) {
        return Region::Unknown;
    }

    const std::string& token = message.payloads[0];
    if (token == "RegionOne") {
        return Region::RegionOne;
    }
    if (token == "RegionTwo") {
        return Region::RegionTwo;
    }
    if (token == "RegionThree") {
        return Region::RegionThree;
    }
    return Region::Unknown;
}

} // namespace app
