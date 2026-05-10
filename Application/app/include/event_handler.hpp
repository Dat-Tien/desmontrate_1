#pragma once

#include <string>
#include <vector>

#include "message_queue.hpp"
#include "types.hpp"

namespace app {

class EventHandler {
public:
    explicit EventHandler(MessageQueue<AppMessage>& queue);

    void PostEvent(const AppMessage& message);

    void PostInternalEvent(EventType event,
                           const std::vector<std::string>& payloads = {},
                           const std::string& raw_payload = "");

private:
    MessageQueue<AppMessage>& m_queue;
};

} // namespace app
