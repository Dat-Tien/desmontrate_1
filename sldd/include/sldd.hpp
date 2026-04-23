#pragma once

#include <string>
#include <vector>

namespace sldd {

class SlddClient {
public:
    bool Run(int argc, char* argv[]);

private:
    bool HandleCommand(const std::string& domain, const std::vector<std::string>& messages);
    bool SendMessage(const std::string& socket_path, const std::string& message);
    void PrintUsage() const;
};

} // namespace sldd