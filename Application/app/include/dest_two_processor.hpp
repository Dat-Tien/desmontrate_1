#pragma once

#include "western_processor.hpp"

namespace app {

class DestTwoProcessor final : public WesternProcessor {
public:
    DestTwoProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    virtual void OnStartRequest() override;
    virtual void OnRecovery() override;
    virtual void OnStopRequest() override;
};

} // namespace app
