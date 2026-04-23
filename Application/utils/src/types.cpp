#include "types.hpp"

namespace app {

std::string ToString(Region region) {
    switch (region) {
        case Region::RegionThree: return "RegionThree";
        case Region::RegionOne: return "RegionOne";
        default: return "Unknown";
    }
}

std::string ToString(ServiceType service) {
    switch (service) {
        case ServiceType::HMI: return "HMI";
        case ServiceType::Audio: return "Audio";
        case ServiceType::Region: return "Region";
        case ServiceType::System: return "System";
        default: return "Unknown";
    }
}

std::string ToString(EventType event) {
    switch (event) {
        case EventType::IgnitionOn: return "IgnitionOn";
        case EventType::StartRequest: return "StartRequest";
        case EventType::StartRecovery: return "StartRecovery";
        case EventType::StopRequest: return "StopRequest";
        case EventType::AudioBeep: return "AudioBeep";
        case EventType::RegionChanged: return "RegionChanged";
        case EventType::Shutdown: return "Shutdown";
        default: return "Unknown";
    }
}

} // namespace app
