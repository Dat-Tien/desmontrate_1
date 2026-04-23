#pragma once

#include "state_machine.hpp"
#include "types.hpp"
#include "logger.hpp"

namespace app {

class ProcessorBase {
public:
    virtual ~ProcessorBase() = default;
    virtual void HandleMessage(const AppMessage& message) = 0;
    virtual Region GetRegion() const = 0;

protected:
    virtual void InitializeStateMachine() = 0;
    virtual void OnIgnitionOn() = 0;
    virtual void OnStartRequest() = 0;
    virtual void OnRecovery() = 0;
    virtual void OnStopRequest() = 0;

    StateMachine m_state_machine;
};

} // namespace app
