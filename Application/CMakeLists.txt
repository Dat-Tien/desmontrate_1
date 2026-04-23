cmake_minimum_required(VERSION 3.14)
project(ArchitectureDemoMain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(_arch_demo
    app/src/main.cpp
    app/src/state_machine.cpp
    app/src/processor_manager.cpp
    app/src/eu_processor.cpp
    app/src/jp_processor.cpp
    app/src/us_processor.cpp
    app/src/western_processor.cpp
    utils/src/types.cpp
    services/src/hmi_wrapper.cpp
    services/src/audio_wrapper.cpp
    services/src/region_wrapper.cpp
    services/src/power_wrapper.cpp
)

target_include_directories(_arch_demo
    PRIVATE
        app/include
        services/include
        utils/include
)

target_link_libraries(_arch_demo PRIVATE
    Threads::Threads
    common)
