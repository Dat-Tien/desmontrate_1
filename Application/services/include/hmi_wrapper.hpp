#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "types.hpp"

namespace app {

class HmiWrapper {
public:
    using Callback = std::function<void(const AppMessage&)>;

    ~HmiWrapper();
    HmiWrapper(const HmiWrapper&) = delete;
    HmiWrapper& operator=(const HmiWrapper&) = delete;
    HmiWrapper(HmiWrapper&&) = delete;
    HmiWrapper& operator=(HmiWrapper&&) = delete;

    static HmiWrapper& GetInstance();
    
    void RegisterCallback(Callback callback);
    bool Connect(const std::string& socket_path);
    void StartListening();
    void Stop();

private:
    HmiWrapper();

    void ListenLoop();

    int m_socket_fd {-1};
    std::string m_socket_path;
    Callback m_callback;
    std::atomic<bool> m_running {false};
    std::thread m_listener_thread;
};

} // namespace app
