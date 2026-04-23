#include "processor_manager.hpp"

#include <iostream>

namespace app {

ProcessorManager::ProcessorManager() 
{
    m_processor_map.insert(std::make_pair(Region::RegionThree, std::make_shared<app::DestThreeProcessor>()));
    m_processor_map.insert(std::make_pair(Region::RegionOne, std::make_shared<app::DestOneProcessor>()));
    m_processor_map.insert(std::make_pair(Region::RegionTwo, std::make_shared<app::DestTwoProcessor>()));
}

ProcessorManager& ProcessorManager::GetInstance()
{
    static ProcessorManager instance;
    return instance;
}
void ProcessorManager::SetRegion(Region region) {
    m_current_region = region;
    LOGD("[ProcessorManager] Active region set to %s", ToString(region).c_str());
}

Region ProcessorManager::GetCurrentRegion() const {
    return m_current_region;
}

void ProcessorManager::HandleMessage(const AppMessage& message) {
    LOGD("[ProcessorManager] Dispatching event %s for region  %s",
            ToString(message.event).c_str(), ToString(m_current_region).c_str());

    if (m_processor_map.find(m_current_region) != m_processor_map.end())
    {
        m_processor_map[m_current_region]->HandleMessage(message);
    }
    else
    {
        LOGE("[ProcessorManager] Unknown region, ignoring message");
    }
}

} // namespace app
