#include "us_processor.hpp"

#include <iostream>

namespace app {

USProcessor::USProcessor() = default;

void USProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[USProcessor] Handling event: %s , payload= %s" ,ToString(message.event).c_str(), message.payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region USProcessor::GetRegion() const {
    return Region::US;
}

void USProcessor::OnStartRequest() {
    LOGD("[USProcessor] Execute JP start request logic");
}

void USProcessor::OnRecovery() {
    LOGD("[USProcessor] Execute US recovery logic");
}

void USProcessor::OnStopRequest() {
    LOGD("[USProcessor] Execute US stop logic");
}

} // namespace app