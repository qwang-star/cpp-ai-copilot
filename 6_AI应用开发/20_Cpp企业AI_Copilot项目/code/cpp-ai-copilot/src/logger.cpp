#include "copilot/logger.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace copilot {
namespace {

int rank(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return 0;
        case LogLevel::Info:
            return 1;
        case LogLevel::Warn:
            return 2;
        case LogLevel::Error:
            return 3;
    }
    return 1;
}

std::string now_string() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream output;
    output << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

}  // namespace

Logger::Logger(LogLevel level) : level_(level) {}

void Logger::debug(const std::string& message) const {
    write(LogLevel::Debug, "DEBUG", message);
}

void Logger::info(const std::string& message) const {
    write(LogLevel::Info, "INFO", message);
}

void Logger::warn(const std::string& message) const {
    write(LogLevel::Warn, "WARN", message);
}

void Logger::error(const std::string& message) const {
    write(LogLevel::Error, "ERROR", message);
}

void Logger::write(LogLevel level, const std::string& label, const std::string& message) const {
    if (rank(level) < rank(level_)) {
        return;
    }
    std::ostream& stream = level == LogLevel::Error ? std::cerr : std::cout;
    stream << '[' << now_string() << "] [" << label << "] " << message << '\n';
}

LogLevel parse_log_level(const std::string& value) {
    const std::string normalized = lower(value);
    if (normalized == "debug") {
        return LogLevel::Debug;
    }
    if (normalized == "warn") {
        return LogLevel::Warn;
    }
    if (normalized == "error") {
        return LogLevel::Error;
    }
    return LogLevel::Info;
}

}  // namespace copilot
