#include "logger.hpp"
#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <chrono>
#include <iomanip>

static std::mutex log_mutex;

static const char* ToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

void Logger::Log(LogLevel level, const char* fmt, ...) {
    char buffer[512];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::lock_guard<std::mutex> lock(log_mutex);

    std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] "
              << "[" << ToString(level) << "] "
              << buffer
              << std::endl;
}