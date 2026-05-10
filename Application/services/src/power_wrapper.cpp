#include "power_wrapper.hpp"
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

EventType ParsePowerEvent(const std::string& raw_event) {
    if (raw_event == "IGNITION_ON" || raw_event == "IGONITION_ON" || raw_event == "IG_ON" || raw_event == "POWER_IGNITION_ON") {
        return EventType::PowerIgnitionOn;
    }
    if (raw_event == "IGNITION_OFF" || raw_event == "IGONITION_OFF" || raw_event == "IG_OFF" || raw_event == "POWER_IGNITION_OFF") {
        return EventType::PowerIgnitionOff;
    }
    if (raw_event == "POWER_SLEEP") {
        return EventType::PowerSleep;
    }
    if (raw_event == "POWER_WAKE") {
        return EventType::PowerWake;
    }
    return EventType::Unknown;
}

} // namespace

PowerWrapper& PowerWrapper::GetInstance()
{
    static PowerWrapper instance;
    return instance;
}

PowerWrapper::PowerWrapper() = default;

PowerWrapper::~PowerWrapper() {
    Stop();
}

void PowerWrapper::RegisterCallback(Callback callback) {
    m_callback = std::move(callback);
}

bool PowerWrapper::Connect(const std::string& socket_path) {
    m_socket_path = socket_path;
    m_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socket_fd < 0) {
        std::cerr << "[PowerWrapper] Failed to create socket: " << std::strerror(errno) << std::endl;
        return false;
    }

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(m_socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[PowerWrapper] Failed to connect to PowerService: " << std::strerror(errno) << std::endl;
        close(m_socket_fd);
        m_socket_fd = -1;
        return false;
    }

    LOGD("[PowerWrapper] Connected to PowerService via IPC: %s", socket_path.c_str());
    return true;
}

void PowerWrapper::StartListening() {
    if (m_socket_fd < 0 || m_running) {
        return;
    }

    m_running = true;
    m_listener_thread = std::thread(&PowerWrapper::ListenLoop, this);
}

void PowerWrapper::Stop() {
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

void PowerWrapper::ListenLoop() {
    std::string buffer;
    char read_buffer[128] {};

    while (m_running) {
        const ssize_t bytes_read = read(m_socket_fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read <= 0) {
            LOGD("[PowerWrapper] PowerService connection closed");
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

            LOGD("[PowerWrapper] IPC message received: %s", line.c_str());

            const std::vector<std::string> tokens = SplitPayloadLine(line);
            if (tokens.empty()) {
                LOGD("[PowerWrapper] Empty payload after parsing");
                continue;
            }

            const EventType event = ParsePowerEvent(tokens[0]);

            if (event == EventType::Unknown) {
                LOGD("[PowerWrapper] Unknown Power event: %s", tokens[0].c_str());
                continue;
            }

            std::vector<std::string> payloads;
            if (tokens.size() > 1) {
                payloads.assign(tokens.begin() + 1, tokens.end());
            }

            if (m_callback) {
                AppMessage message;
                message.service = ServiceType::Power;
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
