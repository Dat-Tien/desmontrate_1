#pragma once

#include <memory>
#include <map>

#include "dest_one_processor.hpp"
#include "dest_two_processor.hpp"
#include "dest_three_processor.hpp"
#include "timer_manager.hpp"
#include "types.hpp"

namespace app {

class ProcessorManager {
public:
    static ProcessorManager& GetInstance();

    ~ProcessorManager() = default;
    ProcessorManager(const ProcessorManager&) = delete;
    ProcessorManager& operator=(const ProcessorManager&) = delete;
    ProcessorManager(ProcessorManager&&) = delete;
    ProcessorManager& operator=(ProcessorManager&&) = delete;

    void SetTimerManager(ITimerManager* timer_manager);
    void SetRegion(Region region);
    Region GetCurrentRegion() const;
    void HandleMessage(const AppMessage& message);

private:
    ProcessorManager();

private:
    Region m_current_region {Region::RegionThree};
    std::map<Region, std::shared_ptr<app::IProcessor>> m_processor_map;
    ITimerManager* m_timer_manager {nullptr};
};

} // namespace app
