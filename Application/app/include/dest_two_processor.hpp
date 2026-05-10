#pragma once

#include "western_processor.hpp"

namespace app {

class DestTwoProcessor final : public WesternProcessor {
public:
    DestTwoProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    void OnStartRequest() override;
    void OnRecovery() override;
    void OnStopRequest() override;
    void OnTimeoutEvent() override;
};

} // namespace app
