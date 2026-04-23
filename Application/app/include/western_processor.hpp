#pragma once

#include "processor_base.hpp"

namespace app {

class WesternProcessor : public ProcessorBase {
public:
    WesternProcessor();
    void HandleMessage(const AppMessage& message) override;

protected:
    virtual void InitializeStateMachine() override;
    virtual void OnIgnitionOn() override;
};

} // namespace app
