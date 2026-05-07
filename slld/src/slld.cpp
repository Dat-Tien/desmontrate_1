#include "slld.hpp"
#include "logger.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <vector>

namespace slld {

bool slldClient::Run(int argc, char* argv[]) {
    if (argc < 3) {
        LOGE("[slld] Missing service handle or payload");
        PrintUsage();
        return false;
    }

    const std::string service = argv[1];

    if (!IsSupportedService(service)) {
        LOGE("[slld] Unsupported service handle: %s", service.c_str());
        PrintUsage();
        return false;
    }

    std::vector<std::string> payloads;
    for (int i = 2; i < argc; ++i) {
        payloads.emplace_back(argv[i]);
    }

    if (payloads.size() < kMinPayloadCount || payloads.size() > kMaxPayloadCount) {
        LOGE("[slld] Invalid payload count: %zu. Expected 1 to 3 payload values",
             payloads.size());
        PrintUsage();
        return false;
    }

    return HandleCommand(service, payloads);
}

bool slldClient::HandleCommand(const std::string& service,
                               const std::vector<std::string>& payloads) {
    const auto it = m_socket_map.find(service);
    if (it == m_socket_map.end()) {
        LOGE("[slld] No socket mapping found for service: %s", service.c_str());
        return false;
    }

    const std::string payload_line = BuildPayloadLine(payloads);

    LOGI("[slld] Service=%s, payload=%s",
         service.c_str(),
         payload_line.c_str());

    return SendMessage(it->second, payload_line);
}

std::string slldClient::BuildPayloadLine(const std::vector<std::string>& payloads) const {
    std::string result;

    for (std::size_t i = 0; i < payloads.size(); ++i) {
        if (i > 0) {
            result += " ";
        }
        result += payloads[i];
    }

    return result;
}

bool slldClient::SendMessage(const std::string& socket_path,
                             const std::string& message) {
    const int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        LOGE("[slld] Failed to create socket");
        return false;
    }

    sockaddr_un addr {};
    addr.sun_family = AF_UNIX;

    std::strncpy(addr.sun_path,
                 socket_path.c_str(),
                 sizeof(addr.sun_path) - 1);

    if (connect(sock,
                reinterpret_cast<sockaddr*>(&addr),
                sizeof(addr)) < 0) {
        LOGE("[slld] Failed to connect to socket: %s", socket_path.c_str());
        close(sock);
        return false;
    }

    const std::string line = message + "\n";

    const ssize_t written = write(sock, line.c_str(), line.size());
    if (written <= 0) {
        LOGE("[slld] Failed to send payload: %s", message.c_str());
        close(sock);
        return false;
    }

    if (static_cast<std::size_t>(written) != line.size()) {
        LOGE("[slld] Partial write. Expected=%zu, actual=%zd",
             line.size(),
             written);
        close(sock);
        return false;
    }

    LOGI("[slld] Sent payload to %s: %s",
         socket_path.c_str(),
         message.c_str());

    close(sock);
    return true;
}

bool slldClient::IsSupportedService(const std::string& service) const {
    return m_socket_map.find(service) != m_socket_map.end();
}

void slldClient::PrintUsage() const {
    LOGI("[slld] Usage:");
    LOGI("[slld]   slld <service> <payload1> [payload2] [payload3]");
    LOGI("[slld] Examples:");
    LOGI("[slld]   slld audio BEEP");
    LOGI("[slld]   slld audio BEEP WARNING_TONE");
    LOGI("[slld]   slld audio BEEP WARNING_TONE HIGH");
    LOGI("[slld] Supported services:");
    LOGI("[slld]   audio, hmi, power, region");
}

} // namespace slld

int main(int argc, char* argv[]) {
    slld::slldClient client;
    return client.Run(argc, argv) ? 0 : 1;
}