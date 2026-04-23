#include "eu_processor.hpp"

#include <iostream>

namespace app {

EUProcessor::EUProcessor() = default;

void EUProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[EUProcessor] Handling event: %s, payload= %s", ToString(message.event).c_str(), message.payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region EUProcessor::GetRegion() const {
    return Region::EU;
}

void EUProcessor::OnStartRequest() {
    LOGD("[EUProcessor] Execute EU start request logic");
}

void EUProcessor::OnRecovery() {
    LOGD("[EUProcessor] Execute EU recovery logic");
}

void EUProcessor::OnStopRequest() {
    LOGD("[EUProcessor] Execute EU stop logic");
}

} // namespace app
