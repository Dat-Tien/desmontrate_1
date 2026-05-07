#include "audio_wrapper.hpp"
#include "logger.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>

namespace app {

namespace {

std::vector<std::string> SplitPayloadLine(const std::string& line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

EventType ParseAudioEvent(const std::string& raw_event) {
    if (raw_event == "AUDIO_BEEP") {
        return EventType::AudioBeep;
    }
    else if (raw_event == "AUDIO_START") {
        return EventType::AudioStart;
    }
    else if (raw_event == "AUDIO_STOP") {
        return EventType::AudioStop;
    }
    return EventType::Unknown;
}

} // namespace

AudioWrapper& AudioWrapper::GetInstance()
{
    static AudioWrapper instance;
    return instance;
}

AudioWrapper::AudioWrapper() = default;

AudioWrapper::~AudioWrapper() {
    Stop();
}

void AudioWrapper::RegisterCallback(Callback callback) {
    m_callback = std::move(callback);
}

bool AudioWrapper::Connect(const std::string& socket_path) {
    m_socket_path = socket_path;
    m_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socket_fd < 0) {
        std::cerr << "[AudioWrapper] Failed to create socket: " << std::strerror(errno) << std::endl;
        return false;
    }

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(m_socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[AudioWrapper] Failed to connect to AudioService: " << std::strerror(errno) << std::endl;
        close(m_socket_fd);
        m_socket_fd = -1;
        return false;
    }

    LOGD("[AudioWrapper] Connected to AudioService via IPC: %s", socket_path.c_str());
    return true;
}

void AudioWrapper::StartListening() {
    if (m_socket_fd < 0 || m_running) {
        return;
    }

    m_running = true;
    m_listener_thread = std::thread(&AudioWrapper::ListenLoop, this);
}

void AudioWrapper::Stop() {
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

void AudioWrapper::ListenLoop() {
    std::string buffer;
    char read_buffer[128] {};

    while (m_running) {
        const ssize_t bytes_read = read(m_socket_fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read <= 0) {
            LOGD("[AudioWrapper] AudioService connection closed");
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

            LOGD("[AudioWrapper] IPC message received: %s", line.c_str());

            const std::vector<std::string> tokens = SplitPayloadLine(line);
            if (tokens.empty()) {
                LOGD("[AudioWrapper] Empty payload after parsing");
                continue;
            }

            const EventType event = ParseAudioEvent(tokens[0]);

            if (event == EventType::Unknown) {
                LOGD("[AudioWrapper] Unknown Audio event: %s", tokens[0].c_str());
                continue;
            }

            std::vector<std::string> payloads;
            if (tokens.size() > 1) {
                payloads.assign(tokens.begin() + 1, tokens.end());
            }

            if (m_callback) {
                AppMessage message;
                message.service = ServiceType::Audio;
                message.event = event;
                message.payloads = payloads;
                message.raw_payload = line;

                m_callback(message);
            }
        }
    }

    m_running = false;
}

} // namespace app
