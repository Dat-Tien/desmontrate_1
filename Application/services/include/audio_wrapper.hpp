#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "types.hpp"

namespace app {

class ServiceOneWrapper {
public:
    using Callback = std::function<void(const AppMessage&)>;

    ~ServiceOneWrapper();
    ServiceOneWrapper(const ServiceOneWrapper&) = delete;
    ServiceOneWrapper& operator=(const ServiceOneWrapper&) = delete;
    ServiceOneWrapper(ServiceOneWrapper&&) = delete;
    ServiceOneWrapper& operator=(ServiceOneWrapper&&) = delete;

    static ServiceOneWrapper& GetInstance();
    
    void RegisterCallback(Callback callback);
    bool Connect(const std::string& socket_path);
    void StartListening();
    void Stop();

private:
    ServiceOneWrapper();

    void ListenLoop();

    int m_socket_fd {-1};
    std::string m_socket_path;
    Callback m_callback;
    std::atomic<bool> m_running {false};
    std::thread m_listener_thread;
};

} // namespace app
