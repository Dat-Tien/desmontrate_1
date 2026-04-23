#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

namespace {

constexpr const char* kHmiAppSocket = "/tmp/_hmi_service.sock";
constexpr const char* kHmiCmdSocket = "/tmp/_hmi_service_cmd.sock";

std::atomic<bool> g_running {true};

void HandleSignal(int) {
    g_running = false;
}

int CreateServerSocket(const char* socket_path) {
    const int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[HmiService] Failed to create socket: " << socket_path << std::endl;
        return -1;
    }

    unlink(socket_path);

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[HmiService] Bind failed: " << socket_path << std::endl;
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "[HmiService] Listen failed: " << socket_path << std::endl;
        close(server_fd);
        unlink(socket_path);
        return -1;
    }

    return server_fd;
}

bool ForwardLine(int app_client_fd, const std::string& line) {
    if (app_client_fd < 0) {
        std::cerr << "[HmiService] No Application client connected" << std::endl;
        return false;
    }

    const std::string payload = line + "\n";
    const ssize_t written = write(app_client_fd, payload.c_str(), payload.size());
    if (written <= 0) {
        std::cerr << "[HmiService] Failed to forward message to Application" << std::endl;
        return false;
    }

    std::cout << "[HmiService] Forwarded to Application: " << line << std::endl;
    return true;
}

} // namespace

int main() {
    std::signal(SIGINT, HandleSignal);
    std::signal(SIGTERM, HandleSignal);

    const int app_server_fd = CreateServerSocket(kHmiAppSocket);
    const int cmd_server_fd = CreateServerSocket(kHmiCmdSocket);

    if (app_server_fd < 0 || cmd_server_fd < 0) {
        if (app_server_fd >= 0) close(app_server_fd);
        if (cmd_server_fd >= 0) close(cmd_server_fd);
        unlink(kHmiAppSocket);
        unlink(kHmiCmdSocket);
        return 1;
    }

    std::cout << "[HmiService] Waiting for Application on " << kHmiAppSocket << std::endl;
    const int app_client_fd = accept(app_server_fd, nullptr, nullptr);
    if (app_client_fd < 0) {
        std::cerr << "[HmiService] Failed to accept Application client" << std::endl;
        close(app_server_fd);
        close(cmd_server_fd);
        unlink(kHmiAppSocket);
        unlink(kHmiCmdSocket);
        return 1;
    }

    std::cout << "[HmiService] Application connected" << std::endl;
    std::cout << "[HmiService] Waiting for SLDD commands on " << kHmiCmdSocket << std::endl;

    while (g_running) {
        const int cmd_client_fd = accept(cmd_server_fd, nullptr, nullptr);
        if (cmd_client_fd < 0) {
            if (!g_running) {
                break;
            }
            std::cerr << "[HmiService] Failed to accept SLDD client" << std::endl;
            continue;
        }

        std::string buffer;
        char read_buffer[128] {};

        while (true) {
            const ssize_t bytes_read = read(cmd_client_fd, read_buffer, sizeof(read_buffer) - 1);
            if (bytes_read <= 0) {
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

                std::cout << "[HmiService] Received from SLDD: " << line << std::endl;
                ForwardLine(app_client_fd, line);
            }
        }

        close(cmd_client_fd);
    }

    close(app_client_fd);
    close(app_server_fd);
    close(cmd_server_fd);
    unlink(kHmiAppSocket);
    unlink(kHmiCmdSocket);

    std::cout << "[HmiService] Shutdown" << std::endl;
    return 0;
}