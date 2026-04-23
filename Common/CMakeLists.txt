cmake_minimum_required(VERSION 3.14)

add_library(common
    src/logger.cpp
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)