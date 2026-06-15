#pragma once

#include <string>

namespace copilot {

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
};

class Logger {
public:
    explicit Logger(LogLevel level = LogLevel::Info);

    void debug(const std::string& message) const;
    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;

private:
    LogLevel level_;
    void write(LogLevel level, const std::string& label, const std::string& message) const;
};

LogLevel parse_log_level(const std::string& value);

}  // namespace copilot
