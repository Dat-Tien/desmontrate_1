cmake_minimum_required(VERSION 3.14)
project(sldd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sldd
    src/sldd.cpp
)

target_include_directories(sldd PUBLIC include)

target_link_libraries(sldd PRIVATE common)