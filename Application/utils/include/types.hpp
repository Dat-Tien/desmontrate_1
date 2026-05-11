#pragma once

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace app {

enum class Region {
    RegionThree,
    RegionOne,
    RegionTwo,
    Unknown
};

enum class ServiceType {
    HMI,
    Audio,
    Region,
    Power,
    Internal,
    ApplicationTimerTimeout,
    System
};

enum class EventType {
    // Audio Events
    AudioStart,
    AudioStop,
    AudioBeep,
    AudioPlayCompleted,
    // Power Events
    PowerStart,
    PowerStop,
    PowerShutdown,
    PowerIgnitionOn,
    PowerIgnitionOff,
    PowerSleep,
    PowerWake,
    // HMI Events
    HmiBeep,
    // Region Events
    RegionChanged,
    // Application Events
    ApplicationStartRequest,
    ApplicationRecovery,
    ApplicationStopRequest,
    ApplicationRecoveryRequest,
    ApplicationResetRequest,
    ApplicationUpdateRequest,
    ApplicationStatusRequest,
    // Application Timeout events
    ApplicationStartRequestTimeout,
    ApplicationRecoveryTimeout,
    ApplicationStopRequestTimeout,
    ApplicationResetRequestTimeout,
    ApplicatiOnTimeoutEvent,
    ApplicationHeartbeatTimeout,
    // TESTING
    TestingApplicationStartRequestTimer,
    // End TESTING
    // Common Events
    Unknown,
    Shutdown
};

struct AppMessage {
    ServiceType service {ServiceType::System};
    EventType event {EventType::Unknown};
    std::vector<std::string> payloads {};
    std::string raw_payload;
};

using EventPoster = std::function<void(const AppMessage&)>;

std::string ToString(Region region);
std::string ToString(ServiceType service);
std::string ToString(EventType event);

} // namespace app
