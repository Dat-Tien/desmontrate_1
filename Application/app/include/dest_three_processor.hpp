#pragma once

#include "iprocessor.hpp"

namespace app {

class DestThreeProcessor final : public IProcessor {
public:
    DestThreeProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    virtual void InitializeStateMachine() override;
    virtual void OnIgnitionOn() override;
    virtual void OnStartRequest() override;
    virtual void OnRecovery() override;
    virtual void OnStopRequest() override;
};

} // namespace app
