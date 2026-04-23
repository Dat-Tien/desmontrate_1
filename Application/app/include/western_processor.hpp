#pragma once

#include "iprocessor.hpp"

namespace app {

class WesternProcessor : public IProcessor {
public:
    WesternProcessor();
    void HandleMessage(const AppMessage& message) override;

protected:
    virtual void InitializeStateMachine() override;
    virtual void OnIgnitionOn() override;
};

} // namespace app
