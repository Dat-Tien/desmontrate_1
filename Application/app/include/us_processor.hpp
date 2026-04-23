#pragma once

#include "western_processor.hpp"

namespace app {

class USProcessor final : public WesternProcessor {
public:
    USProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    virtual void OnStartRequest() override;
    virtual void OnRecovery() override;
    virtual void OnStopRequest() override;
};

} // namespace app
