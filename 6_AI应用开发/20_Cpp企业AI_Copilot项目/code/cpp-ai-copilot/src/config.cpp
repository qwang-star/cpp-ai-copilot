#include "copilot/config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>

namespace copilot {
namespace {

std::string trim(std::string value) {
    const auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

}  // namespace

AppConfig AppConfig::load(const std::string& path) {
    AppConfig config;
    std::ifstream input(path);
    if (!input) {
        return config;
    }

    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const auto separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const std::string key = trim(line.substr(0, separator));
        const std::string value = trim(line.substr(separator + 1));

        if (key == "APP_HOST") {
            config.host = value;
        } else if (key == "APP_PORT") {
            config.port = std::stoi(value);
        } else if (key == "LOG_LEVEL") {
            config.log_level = value;
        }
    }

    if (config.port <= 0 || config.port > 65535) {
        throw std::runtime_error("APP_PORT must be between 1 and 65535");
    }

    return config;
}

}  // namespace copilot
