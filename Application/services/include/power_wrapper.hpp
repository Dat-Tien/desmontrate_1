#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "types.hpp"

namespace app {

class PowerWrapper {
public:
    using Callback = std::function<void(const AppMessage&)>;

    ~PowerWrapper();
    PowerWrapper(const PowerWrapper&) = delete;
    PowerWrapper& operator=(const PowerWrapper&) = delete;
    PowerWrapper(PowerWrapper&&) = delete;
    PowerWrapper& operator=(PowerWrapper&&) = delete;

    static PowerWrapper& GetInstance();
    
    void RegisterCallback(Callback callback);
    bool Connect(const std::string& socket_path);
    void StartListening();
    void Stop();

private:
    PowerWrapper();

    void ListenLoop();

    int m_socket_fd {-1};
    std::string m_socket_path;
    Callback m_callback;
    std::atomic<bool> m_running {false};
    std::thread m_listener_thread;
};

} // namespace app
