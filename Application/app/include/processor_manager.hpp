#pragma once

#include <memory>
#include <map>

#include "eu_processor.hpp"
#include "us_processor.hpp"
#include "jp_processor.hpp"
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
    
    void SetRegion(Region region);
    Region GetCurrentRegion() const;
    void HandleMessage(const AppMessage& message);

private:
    ProcessorManager();
    Region m_current_region {Region::JP};
    std::map<Region, std::shared_ptr<app::ProcessorBase>> m_processor_map;
};

} // namespace app
