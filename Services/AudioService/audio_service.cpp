#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <poll.h>
#include <string>
#include <thread>

namespace {

constexpr const char* kAudioAppSocket = "/tmp/_audio_service.sock";
constexpr const char* kAudioCmdSocket = "/tmp/_audio_service_cmd.sock";

std::atomic<bool> g_running {true};
std::mutex g_app_write_mutex;

void HandleSignal(int) {
    g_running = false;
}

int CreateServerSocket(const char* socket_path) {
    const int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "[AudioService] Failed to create socket: " << socket_path << std::endl;
        return -1;
    }

    unlink(socket_path);

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[AudioService] Bind failed: " << socket_path << std::endl;
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "[AudioService] Listen failed: " << socket_path << std::endl;
        close(server_fd);
        unlink(socket_path);
        return -1;
    }

    return server_fd;
}

bool ForwardLine(int app_client_fd, const std::string& line) {
    if (app_client_fd < 0) {
        std::cerr << "[AudioService] No Application client connected" << std::endl;
        return false;
    }

    const std::string payload = line + "\n";
    std::lock_guard<std::mutex> lock(g_app_write_mutex);
    const ssize_t written = write(app_client_fd, payload.c_str(), payload.size());
    if (written <= 0) {
        std::cerr << "[AudioService] Failed to forward message to Application" << std::endl;
        return false;
    }

    std::cout << "[AudioService] Forwarded to Application: " << line << std::endl;
    return true;
}

void HandleApplicationRequest(int app_client_fd, const std::string& line) {
    std::cout << "[AudioService] Received request from Application: " << line << std::endl;

    if (line == "PLAY_STOP_COMPLETED_SOUND") {
        std::thread([app_client_fd]() {
            std::cout << "[AudioService] Playing stop-completed sound..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ForwardLine(app_client_fd, "AUDIO_PLAY_COMPLETED");
        }).detach();
        return;
    }

    std::cout << "[AudioService] Unknown application request: " << line << std::endl;
}

void ProcessLines(std::string& buffer, const std::function<void(const std::string&)>& handler) {
    std::size_t newline_pos = std::string::npos;
    while ((newline_pos = buffer.find('\n')) != std::string::npos) {
        const std::string line = buffer.substr(0, newline_pos);
        buffer.erase(0, newline_pos + 1);

        if (!line.empty()) {
            handler(line);
        }
    }
}

} // namespace

int main() {
    std::signal(SIGINT, HandleSignal);
    std::signal(SIGTERM, HandleSignal);

    const int app_server_fd = CreateServerSocket(kAudioAppSocket);
    const int cmd_server_fd = CreateServerSocket(kAudioCmdSocket);

    if (app_server_fd < 0 || cmd_server_fd < 0) {
        if (app_server_fd >= 0) close(app_server_fd);
        if (cmd_server_fd >= 0) close(cmd_server_fd);
        unlink(kAudioAppSocket);
        unlink(kAudioCmdSocket);
        return 1;
    }

    std::cout << "[AudioService] Waiting for Application on " << kAudioAppSocket << std::endl;
    const int app_client_fd = accept(app_server_fd, nullptr, nullptr);
    if (app_client_fd < 0) {
        std::cerr << "[AudioService] Failed to accept Application client" << std::endl;
        close(app_server_fd);
        close(cmd_server_fd);
        unlink(kAudioAppSocket);
        unlink(kAudioCmdSocket);
        return 1;
    }

    std::cout << "[AudioService] Application connected" << std::endl;
    std::cout << "[AudioService] Waiting for slld commands on " << kAudioCmdSocket << std::endl;

    std::string app_buffer;
    char read_buffer[128]{};

    while (g_running) {
        pollfd fds[2]{};
        fds[0].fd = cmd_server_fd;
        fds[0].events = POLLIN;
        fds[1].fd = app_client_fd;
        fds[1].events = POLLIN;

        const int ret = poll(fds, 2, 500);
        if (ret < 0) {
            if (!g_running) {
                break;
            }
            std::cerr << "[AudioService] poll failed" << std::endl;
            continue;
        }

        if (ret == 0) {
            continue;
        }

        if (fds[1].revents & POLLIN) {
            const ssize_t bytes_read = read(app_client_fd, read_buffer, sizeof(read_buffer) - 1);
            if (bytes_read <= 0) {
                std::cout << "[AudioService] Application connection closed" << std::endl;
                g_running = false;
                break;
            }

            read_buffer[bytes_read] = '\0';
            app_buffer.append(read_buffer, static_cast<std::size_t>(bytes_read));
            ProcessLines(app_buffer, [app_client_fd](const std::string& line) {
                HandleApplicationRequest(app_client_fd, line);
            });
        }

        if (fds[0].revents & POLLIN) {
            const int cmd_client_fd = accept(cmd_server_fd, nullptr, nullptr);
            if (cmd_client_fd < 0) {
                if (!g_running) {
                    break;
                }
                std::cerr << "[AudioService] Failed to accept slld client" << std::endl;
                continue;
            }

            std::string cmd_buffer;
            while (g_running) {
                const ssize_t bytes_read = read(cmd_client_fd, read_buffer, sizeof(read_buffer) - 1);
                if (bytes_read <= 0) {
                    break;
                }

                read_buffer[bytes_read] = '\0';
                cmd_buffer.append(read_buffer, static_cast<std::size_t>(bytes_read));

                ProcessLines(cmd_buffer, [app_client_fd](const std::string& line) {
                    std::cout << "[AudioService] Received from slld: " << line << std::endl;
                    ForwardLine(app_client_fd, line);
                });
            }

            close(cmd_client_fd);
        }
    }

    close(app_client_fd);
    close(app_server_fd);
    close(cmd_server_fd);
    unlink(kAudioAppSocket);
    unlink(kAudioCmdSocket);

    std::cout << "[AudioService] Shutdown" << std::endl;
    return 0;
}
