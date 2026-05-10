#include "event_handler.hpp"

#include "logger.hpp"

namespace app {

EventHandler::EventHandler(MessageQueue<AppMessage>& queue)
    : m_queue(queue)
{
}

void EventHandler::PostEvent(const AppMessage& message)
{
    if (message.event == EventType::Unknown) {
        LOGD("[EventHandler] Ignore Unknown event from service=%s, raw_payload=%s",
             ToString(message.service).c_str(),
             message.raw_payload.c_str());
        return;
    }

    LOGD("[EventHandler] Post event service=%s, event=%s, raw_payload=%s",
         ToString(message.service).c_str(),
         ToString(message.event).c_str(),
         message.raw_payload.c_str());

    m_queue.Push(message);
}

void EventHandler::PostInternalEvent(EventType event,
                                     const std::vector<std::string>& payloads,
                                     const std::string& raw_payload)
{
    AppMessage message;
    message.service = ServiceType::Internal;
    message.event = event;
    message.payloads = payloads;
    message.raw_payload = raw_payload;

    PostEvent(message);
}

} // namespace app
