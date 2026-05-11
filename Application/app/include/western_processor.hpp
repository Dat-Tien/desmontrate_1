#pragma once

#include "iprocessor.hpp"

namespace app {

class WesternProcessor : public IProcessor {
public:
    WesternProcessor();
    void HandleMessage(const AppMessage& message) override;

protected:
    void InitializeStateMachine() override;
    void OnIgnitionOn() override;
    void OnIgnitionOff() override;
    void OnTimeoutEvent() override;
    void OnStopCompleted() override;
    void OnAudioPlayCompleted() override;
};

} // namespace app
