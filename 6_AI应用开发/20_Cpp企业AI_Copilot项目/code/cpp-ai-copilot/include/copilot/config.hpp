#pragma once

#include <string>

namespace copilot {

struct AppConfig {
    std::string host = "127.0.0.1";
    int port = 8080;
    std::string log_level = "info";

    static AppConfig load(const std::string& path);
};

}  // namespace copilot
