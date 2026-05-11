#include "types.hpp"

namespace app {

std::string ToString(Region region) {
    switch (region) {
        case Region::RegionThree: return "RegionThree";
        case Region::RegionOne: return "RegionOne";
        case Region::RegionTwo: return "RegionTwo";
        case Region::Unknown: return "Unknown";
        default: return "Unknown";
    }
}

std::string ToString(ServiceType service) {
    switch (service) {
        case ServiceType::HMI: return "HMI";
        case ServiceType::Audio: return "Audio";
        case ServiceType::Region: return "Region";
        case ServiceType::Power: return "Power";
        case ServiceType::Internal: return "Internal";
        case ServiceType::ApplicationTimerTimeout: return "ApplicationTimerTimeout";
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
        case EventType::AudioPlayCompleted: return "AudioPlayCompleted";
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
        // Timeout Events
        case EventType::ApplicationStartRequestTimeout: return "ApplicationStartRequestTimeout";
        case EventType::ApplicationRecoveryTimeout: return "ApplicationRecoveryTimeout";
        case EventType::ApplicationStopRequestTimeout: return "ApplicationStopRequestTimeout";
        case EventType::ApplicationResetRequestTimeout: return "ApplicationResetRequestTimeout";
        case EventType::ApplicatiOnTimeoutEvent: return "ApplicatiOnTimeoutEvent";
        case EventType::ApplicationHeartbeatTimeout: return "ApplicationHeartbeatTimeout";
        // TESTING
        case EventType::TestingApplicationStartRequestTimer: return "TestingApplicationStartRequestTimer";

        // Common Events
        case EventType::Shutdown: return "Shutdown";
        case EventType::Unknown: return "Unknown";
        default: return "Unknown";
    }
}

} // namespace app
