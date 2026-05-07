#include "dest_two_processor.hpp"

#include <iostream>

namespace app {

DestTwoProcessor::DestTwoProcessor() = default;

void DestTwoProcessor::HandleMessage(const AppMessage& message) {
    LOGD("[DestTwoProcessor] Handling event: %s , raw_payload= %s" ,ToString(message.event).c_str(), message.raw_payload.c_str());
    m_state_machine.HandleEvent(message.event);
}

Region DestTwoProcessor::GetRegion() const {
    return Region::RegionTwo;
}

void DestTwoProcessor::OnStartRequest() {
    LOGD("[DestTwoProcessor] Execute RegionTwo start request logic");
}

void DestTwoProcessor::OnRecovery() {
    LOGD("[DestTwoProcessor] Execute RegionTwo recovery logic");
}

void DestTwoProcessor::OnStopRequest() {
    LOGD("[DestTwoProcessor] Execute RegionTwo stop logic");
}

} // namespace app