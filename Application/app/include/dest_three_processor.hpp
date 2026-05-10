#pragma once

#include "iprocessor.hpp"

namespace app {

class DestThreeProcessor final : public IProcessor {
public:
    DestThreeProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    void InitializeStateMachine() override;
    void OnIgnitionOn() override;
    void OnIgnitionOff() override;
    void OnStartRequest() override;
    void OnRecovery() override;
    void OnStopRequest() override;
    void OnTimeoutEvent() override;
};

} // namespace app
