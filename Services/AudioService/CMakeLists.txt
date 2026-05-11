cmake_minimum_required(VERSION 3.14)
project(AudioServiceDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(audio_service
    audio_service.cpp
)

target_link_libraries(audio_service PRIVATE
    common
    Threads::Threads
)

target_compile_features(audio_service PRIVATE cxx_std_17)