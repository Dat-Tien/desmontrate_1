#include "region_wrapper.hpp"
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

EventType ParseRegionEvent(const std::string& raw_event) {
    if (raw_event == "REGION_CHANGED") {
        return EventType::RegionChanged;
    }
    return EventType::Unknown;
}

} // namespace

RegionWrapper& RegionWrapper::GetInstance()
{
    static RegionWrapper instance;
    return instance;
}

RegionWrapper::RegionWrapper() = default;

RegionWrapper::~RegionWrapper() {
    Stop();
}

void RegionWrapper::RegisterCallback(Callback callback) {
    m_callback = std::move(callback);
}

bool RegionWrapper::Connect(const std::string& socket_path) {
    m_socket_path = socket_path;
    m_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socket_fd < 0) {
        std::cerr << "[RegionWrapper] Failed to create socket: " << std::strerror(errno) << std::endl;
        return false;
    }

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(m_socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[RegionWrapper] Failed to connect to RegionService: " << std::strerror(errno) << std::endl;
        close(m_socket_fd);
        m_socket_fd = -1;
        return false;
    }

    LOGD("[RegionWrapper] Connected to RegionService via IPC: %s", socket_path.c_str());
    return true;
}

void RegionWrapper::StartListening() {
    if (m_socket_fd < 0 || m_running) {
        return;
    }

    m_running = true;
    m_listener_thread = std::thread(&RegionWrapper::ListenLoop, this);
}

void RegionWrapper::Stop() {
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

void RegionWrapper::ListenLoop() {
    std::string buffer;
    char read_buffer[128] {};

    while (m_running) {
        const ssize_t bytes_read = read(m_socket_fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read <= 0) {
            LOGD("[RegionWrapper] RegionService connection closed");
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

            LOGD("[RegionWrapper] IPC message received: %s", line.c_str());

            const std::vector<std::string> tokens = SplitPayloadLine(line);
            if (tokens.empty()) {
                LOGD("[RegionWrapper] Empty payload after parsing");
                continue;
            }

            const EventType event = ParseRegionEvent(tokens[0]);

            if (event == EventType::Unknown) {
                LOGD("[RegionWrapper] Unknown Region event: %s", tokens[0].c_str());
                continue;
            }

            std::vector<std::string> payloads;
            if (tokens.size() > 1) {
                payloads.assign(tokens.begin() + 1, tokens.end());
            }

            if (m_callback) {
                AppMessage message;
                message.service = ServiceType::Region;
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
