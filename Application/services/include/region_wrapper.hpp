#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "types.hpp"

namespace app {

class RegionWrapper {
public:
    using Callback = std::function<void(const AppMessage&)>;

    ~RegionWrapper();
    RegionWrapper(const RegionWrapper&) = delete;
    RegionWrapper& operator=(const RegionWrapper&) = delete;
    RegionWrapper(RegionWrapper&&) = delete;
    RegionWrapper& operator=(RegionWrapper&&) = delete;

    static RegionWrapper& GetInstance();
    
    void RegisterCallback(Callback callback);
    bool Connect(const std::string& socket_path);
    void StartListening();
    void Stop();

private:
    RegionWrapper();

    void ListenLoop();

    int m_socket_fd {-1};
    std::string m_socket_path;
    Callback m_callback;
    std::atomic<bool> m_running {false};
    std::thread m_listener_thread;
};

} // namespace app
