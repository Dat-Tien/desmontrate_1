cmake_minimum_required(VERSION 3.14)
project(HmiServiceDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(hmi_service
    hmi_service.cpp
)

target_link_libraries(hmi_service PRIVATE common)