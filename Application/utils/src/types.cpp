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
        // Audio Events
        case EventType::AudioStart: return "AudioStart";
        case EventType::AudioStop: return "AudioStop";
        case EventType::AudioBeep: return "AudioBeep";
        // Power Events
        case EventType::PowerStart: return "PowerStart";
        case EventType::PowerStop: return "PowerStop";
        case EventType::PowerShutdown: return "PowerShutdown";
        case EventType::PowerIgnitionOn: return "PowerIgnitionOn";
        case EventType::PowerIgnitionOff: return "PowerIgnitionOff";
        case EventType::PowerSleep: return "PowerSleep";
        case EventType::PowerWake: return "PowerWake";
        // HMI Events
        case EventType::HmiBeep: return "HmiBeep";
        // Region Events
        case EventType::RegionChanged: return "RegionChanged";
        // Application Events
        case EventType::ApplicationStartRequest: return "ApplicationStartRequest";
        case EventType::ApplicationRecovery: return "ApplicationRecovery";
        case EventType::ApplicationStopRequest: return "ApplicationStopRequest";
        case EventType::ApplicationRecoveryRequest: return "ApplicationRecoveryRequest";
        case EventType::ApplicationResetRequest: return "ApplicationResetRequest";
        case EventType::ApplicationUpdateRequest: return "ApplicationUpdateRequest";
        case EventType::ApplicationStatusRequest: return "ApplicationStatusRequest";
        // Common Events
        case EventType::Shutdown: return "Shutdown";
        default: return "Unknown";
    }
}

} // namespace app
