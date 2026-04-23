#include "processor_manager.hpp"

#include <iostream>

namespace app {

ProcessorManager::ProcessorManager() 
{
    m_processor_map.insert(std::make_pair(Region::JP, std::make_shared<app::JPProcessor>()));
    m_processor_map.insert(std::make_pair(Region::EU, std::make_shared<app::EUProcessor>()));
    m_processor_map.insert(std::make_pair(Region::US, std::make_shared<app::USProcessor>()));
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
