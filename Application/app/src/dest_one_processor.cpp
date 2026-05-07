#include "dest_one_processor.hpp"

#include <iostream>

namespace app {

DestOneProcessor::DestOneProcessor() = default;

void DestOneProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[DestOneProcessor] Handling event: %s, raw_payload= %s", ToString(message.event).c_str(), message.raw_payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region DestOneProcessor::GetRegion() const {
    return Region::RegionOne;
}

void DestOneProcessor::OnStartRequest() {
    LOGD("[DestOneProcessor] Execute RegionOne start request logic");
}

void DestOneProcessor::OnRecovery() {
    LOGD("[DestOneProcessor] Execute RegionOne recovery logic");
}

void DestOneProcessor::OnStopRequest() {
    LOGD("[DestOneProcessor] Execute RegionOne stop logic");
}

} // namespace app
