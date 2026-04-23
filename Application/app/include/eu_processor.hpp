#pragma once

#include "western_processor.hpp"

namespace app {

class EUProcessor final : public WesternProcessor {
public:
    EUProcessor();
    void HandleMessage(const AppMessage& message) override;
    Region GetRegion() const override;

protected:
    void OnStartRequest() override;
    void OnRecovery() override;
    void OnStopRequest() override;
};

} // namespace app