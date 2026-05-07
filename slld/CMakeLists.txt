cmake_minimum_required(VERSION 3.14)
project(slld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(slld
    src/slld.cpp
)

target_include_directories(slld PUBLIC include)

target_link_libraries(slld PRIVATE common)