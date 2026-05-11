#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "types.hpp"

namespace app {

class AudioWrapper {
public:
    using Callback = std::function<void(const AppMessage&)>;

    ~AudioWrapper();
    AudioWrapper(const AudioWrapper&) = delete;
    AudioWrapper& operator=(const AudioWrapper&) = delete;
    AudioWrapper(AudioWrapper&&) = delete;
    AudioWrapper& operator=(AudioWrapper&&) = delete;

    static AudioWrapper& GetInstance();
    
    void RegisterCallback(Callback callback);
    bool Connect(const std::string& socket_path);
    void StartListening();
    void Stop();
    bool RequestPlayStopCompletedSound();

private:
    AudioWrapper();

    void ListenLoop();

    int m_socket_fd {-1};
    std::string m_socket_path;
    Callback m_callback;
    std::atomic<bool> m_running {false};
    std::thread m_listener_thread;
};

} // namespace app
