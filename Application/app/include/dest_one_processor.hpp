#pragma once

#include "western_processor.hpp"

namespace app {

class DestOneProcessor final : public WesternProcessor {
public:
    DestOneProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    void OnStartRequest() override;
    void OnRecovery() override;
    void OnStopRequest() override;
    void OnTimeoutEvent() override;
};

} // namespace app
