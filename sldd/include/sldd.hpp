#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace sldd {

class SlddClient {
public:
    bool Run(int argc, char* argv[]);

private:
    bool HandleCommand(const std::string& service,
                       const std::vector<std::string>& payloads);

    bool SendMessage(const std::string& socket_path,
                     const std::string& message);

    std::string BuildPayloadLine(const std::vector<std::string>& payloads) const;

    bool IsSupportedService(const std::string& service) const;

    void PrintUsage() const;

private:
    static constexpr int kMinPayloadCount = 1;
    static constexpr int kMaxPayloadCount = 3;

    const std::unordered_map<std::string, std::string> m_socket_map {
        {"audio",  "/tmp/_audio_service_cmd.sock"},
        {"hmi",    "/tmp/_hmi_service_cmd.sock"},
        {"power",  "/tmp/_power_service_cmd.sock"},
        {"region", "/tmp/_region_service_cmd.sock"}
    };
};

} // namespace sldd