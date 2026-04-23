#pragma once
#include <string>


#define LOGD(fmt, ...) Logger::Log(LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) Logger::Log(LogLevel::INFO,  fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) Logger::Log(LogLevel::WARN,  fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) Logger::Log(LogLevel::ERROR, fmt, ##__VA_ARGS__)

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static void Log(LogLevel level, const char* fmt, ...);
};