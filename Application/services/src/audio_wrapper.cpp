#include "audio_wrapper.hpp"
#include "logger.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

namespace app {

namespace {

EventType ParseAudioEvent(const std::string& raw_event) {
    if (raw_event == "AUDIO_BEEP") {
        return EventType::AudioBeep;
    }
    return EventType::Unknown;
}

} // namespace

ServiceOneWrapper& ServiceOneWrapper::GetInstance()
{
    static ServiceOneWrapper instance;
    return instance;
}

ServiceOneWrapper::ServiceOneWrapper() = default;

ServiceOneWrapper::~ServiceOneWrapper() {
    Stop();
}

void ServiceOneWrapper::RegisterCallback(Callback callback) {
    m_callback = std::move(callback);
}

bool ServiceOneWrapper::Connect(const std::string& socket_path) {
    m_socket_path = socket_path;
    m_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socket_fd < 0) {
        std::cerr << "[ServiceOneWrapper] Failed to create socket: " << std::strerror(errno) << std::endl;
        return false;
    }

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(m_socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[ServiceOneWrapper] Failed to connect to AudioService: " << std::strerror(errno) << std::endl;
        close(m_socket_fd);
        m_socket_fd = -1;
        return false;
    }

    LOGD("[ServiceOneWrapper] Connected to AudioService via IPC: %s", socket_path.c_str());
    return true;
}

void ServiceOneWrapper::StartListening() {
    if (m_socket_fd < 0 || m_running) {
        return;
    }

    m_running = true;
    m_listener_thread = std::thread(&ServiceOneWrapper::ListenLoop, this);
}

void ServiceOneWrapper::Stop() {
    if (m_running.exchange(false) && m_socket_fd >= 0) {
        shutdown(m_socket_fd, SHUT_RDWR);
    }

    if (m_listener_thread.joinable()) {
        m_listener_thread.join();
    }

    if (m_socket_fd >= 0) {
        close(m_socket_fd);
        m_socket_fd = -1;
    }
}

void ServiceOneWrapper::ListenLoop() {
    std::string buffer;
    char read_buffer[128] {};

    while (m_running) {
        const ssize_t bytes_read = read(m_socket_fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read <= 0) {
            LOGD("[ServiceOneWrapper] AudioService connection closed");
            break;
        }

        read_buffer[bytes_read] = '\0';
        buffer.append(read_buffer, static_cast<std::size_t>(bytes_read));

        std::size_t newline_pos = std::string::npos;
        while ((newline_pos = buffer.find('\n')) != std::string::npos) {
            const std::string line = buffer.substr(0, newline_pos);
            buffer.erase(0, newline_pos + 1);

            if (line.empty()) {
                continue;
            }

            LOGD("[ServiceOneWrapper] IPC message received: %s", line.c_str());
            const EventType event = ParseAudioEvent(line);
            if (event != EventType::Shutdown && m_callback) {
                m_callback(AppMessage{ServiceType::Audio, event, line});
            }
        }
    }

    m_running = false;
}

} // namespace app
