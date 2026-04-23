#pragma once

#include "processor_base.hpp"

namespace app {

class JPProcessor final : public ProcessorBase {
public:
    JPProcessor();
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
