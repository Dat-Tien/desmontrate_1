#include "sldd.hpp"
#include "logger.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <vector>
#include <map>

namespace {

constexpr int kMinMessages = 1;
constexpr int kMaxMessages = 3;

const std::map<std::string, std::string> socket_map = {
    {"audio", "/tmp/_audio_service_cmd.sock"},
    {"hmi", "/tmp/_hmi_service_cmd.sock"},
    {"power", "/tmp/_power_service_cmd.sock"},
    {"region", "/tmp/_region_service_cmd.sock"}
};

} // namespace

namespace sldd {

bool SlddClient::Run(int argc, char* argv[]) {
    if (argc < 3) {
        PrintUsage();
        return false;
    }

    const std::string domain = argv[1];
    std::vector<std::string> messages;
    for (int i = 2; i < argc; ++i) {
        messages.emplace_back(argv[i]);
    }

    if (socket_map.find(domain) != socket_map.end()) {
        return HandleCommand(domain, messages);
    }
    else
    {
        LOGE("[SLDD] Unsupported domain: %s", domain.c_str());
        PrintUsage();
        return false;
    }
}

bool SlddClient::HandleCommand(const std::string& domain, const std::vector<std::string>& messages) {
    if (messages.size() < kMinMessages || messages.size() > kMaxMessages) {
        LOGE("[SLDD] command requires 1 to 3 messages, got %zu", messages.size());
        PrintUsage();
        return false;
    }

    bool ret_val = true;
    if (socket_map.find(domain) != socket_map.end())
    {
        for (const auto& msg : messages) {
            if (!SendMessage(socket_map.at(domain).c_str(), msg)) {
                ret_val = false;
                break;
            }
        }
    }
    else {
        ret_val = false;
    }

    return ret_val;
}

bool SlddClient::SendMessage(const std::string& socket_path, const std::string& message) {
    const int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        LOGE("[SLDD] Failed to create socket");
        return false;
    }

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        LOGE("[SLDD] Failed to connect to %s", socket_path.c_str());
        close(sock);
        return false;
    }

    const std::string line = message + "\n";
    const ssize_t written = write(sock, line.c_str(), line.size());
    if (written <= 0) {
        LOGE("[SLDD] Failed to send message: %s", message.c_str());
        close(sock);
        return false;
    }

    LOGI("[SLDD] Sent audio message: %s", message.c_str());
    close(sock);
    return true;
}

void SlddClient::PrintUsage() const {
    LOGI("[SLDD] Usage: sldd audio <msg1> [msg2] [msg3]");
}

} // namespace sldd

int main(int argc, char* argv[]) {
    sldd::SlddClient client;
    return client.Run(argc, argv) ? 0 : 1;
}