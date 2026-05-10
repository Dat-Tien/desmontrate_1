#include "processor_manager.hpp"

namespace app {

ProcessorManager::ProcessorManager()
{
    m_processor_map.emplace(Region::RegionThree, std::make_shared<app::DestThreeProcessor>());
    m_processor_map.emplace(Region::RegionOne, std::make_shared<app::DestOneProcessor>());
    m_processor_map.emplace(Region::RegionTwo, std::make_shared<app::DestTwoProcessor>());
}

ProcessorManager& ProcessorManager::GetInstance()
{
    static ProcessorManager instance;
    return instance;
}

void ProcessorManager::SetTimerManager(ITimerManager* timer_manager)
{
    m_timer_manager = timer_manager;
    for (auto& item : m_processor_map) {
        item.second->SetTimerManager(m_timer_manager);
    }
}

void ProcessorManager::SetRegion(Region region) {
    m_current_region = region;
    LOGD("[ProcessorManager] Active region set to %s", ToString(region).c_str());
}

Region ProcessorManager::GetCurrentRegion() const {
    return m_current_region;
}

void ProcessorManager::HandleMessage(const AppMessage& message) {
    LOGD("[ProcessorManager] Dispatching event=%s for region=%s",
         ToString(message.event).c_str(),
         ToString(m_current_region).c_str());

    const auto it = m_processor_map.find(m_current_region);
    if (it != m_processor_map.end()) {
        it->second->HandleMessage(message);
    } else {
        LOGE("[ProcessorManager] Unknown region, ignoring message");
    }
}

} // namespace app
