cmake_minimum_required(VERSION 3.14)
project(RegionServiceDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(region_service
    region_service.cpp
)

target_link_libraries(region_service PRIVATE common)