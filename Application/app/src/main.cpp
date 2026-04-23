#include <atomic>
#include <csignal>
#include <chrono>
#include <iostream>
#include <thread>

#include "audio_wrapper.hpp"
#include "hmi_wrapper.hpp"
#include "message_queue.hpp"
#include "processor_manager.hpp"
#include "power_wrapper.hpp"
#include "region_wrapper.hpp"
#include "types.hpp"

namespace {

using namespace app;

std::atomic<bool> g_running{true};
MessageQueue<AppMessage>* g_queue = nullptr;

void SignalHandler(int signal) {
    LOGD("[Main] Signal received: %d", signal);
    g_running = false;
    if (g_queue != nullptr) {
        g_queue->Stop();
    }
}

void PushToQueue(MessageQueue<AppMessage>& queue, const AppMessage& message) {
    LOGD("[Main] Queueing message from service=%s, event=%s, payload=%s",
         ToString(message.service).c_str(),
         ToString(message.event).c_str(),
         message.payload.c_str());
    queue.Push(message);
}

} // namespace

int main() {
    using namespace app;

    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    constexpr const char* kAudioSocketPath = "/tmp/_audio_service.sock";
    constexpr const char* kHmiSocketPath = "/tmp/_hmi_service.sock";
    constexpr const char* kPowerSocketPath = "/tmp/_power_service.sock";
    constexpr const char* kRegionSocketPath = "/tmp/_region_service.sock";

    MessageQueue<AppMessage> queue;
    g_queue = &queue;

    ProcessorManager& processor_manager = ProcessorManager::GetInstance();

    HmiWrapper& hmi_wrapper = HmiWrapper::GetInstance();
    ServiceOneWrapper& audio_wrapper = ServiceOneWrapper::GetInstance();
    RegionWrapper& region_wrapper = RegionWrapper::GetInstance();
    PowerWrapper& power_wrapper = PowerWrapper::GetInstance();

    hmi_wrapper.RegisterCallback([&queue](const AppMessage& message) {
        PushToQueue(queue, message);
    });

    audio_wrapper.RegisterCallback([&queue](const AppMessage& message) {
        PushToQueue(queue, message);
    });

    region_wrapper.RegisterCallback([&queue](const AppMessage& message) {
        PushToQueue(queue, message);
    });

    power_wrapper.RegisterCallback([&queue](const AppMessage& message) {
        PushToQueue(queue, message);
    });

    std::thread app_thread([&]() {
        bool running = true;
        AppMessage message{};

        while (running && queue.WaitAndPop(message)) {
            if (message.event == EventType::Shutdown) {
                LOGD("[AppThread] Shutdown received");
                running = false;
                break;
            }

            if (message.event == EventType::RegionChanged) {
                const auto region = (message.payload == "RegionOne") ? Region::RegionOne : Region::RegionThree;
                processor_manager.SetRegion(region);
                continue;
            }

            processor_manager.HandleMessage(message);
        }

        LOGD("[AppThread] Exiting");
    });

    LOGD("===  Architecture Demo Start ===");

    if (audio_wrapper.Connect(kAudioSocketPath)) {
        audio_wrapper.StartListening();
    } else {
        LOGD("[Main] AudioService is not connected. Start AudioService first for IPC demo.");
    }

    if (hmi_wrapper.Connect(kHmiSocketPath)) {
        hmi_wrapper.StartListening();
    } else {
        LOGD("[Main] HmiService is not connected. Start HmiService first for IPC demo.");
    }

    app_thread.join();

    LOGD("===  Architecture Demo End ===");
    return 0;
}