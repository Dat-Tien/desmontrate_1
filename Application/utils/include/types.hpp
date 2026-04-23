#pragma once

#include <string>

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
    System
};

enum class EventType {
    IgnitionOn,
    StartRequest,
    StartRecovery,
    StopRequest,
    AudioBeep,
    RegionChanged,
    Unknown,
    Shutdown
};

struct AppMessage {
    ServiceType service {ServiceType::System};
    EventType event {EventType::Unknown};
    std::string payload {};
};

std::string ToString(Region region);
std::string ToString(ServiceType service);
std::string ToString(EventType event);

} // namespace app
