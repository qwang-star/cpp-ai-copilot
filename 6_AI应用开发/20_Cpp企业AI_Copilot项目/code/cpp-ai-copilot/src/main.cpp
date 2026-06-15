#include "copilot/application.hpp"
#include "copilot/config.hpp"
#include "copilot/logger.hpp"
#include "copilot/simple_server.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.env";

    try {
        copilot::AppConfig config = copilot::AppConfig::load(config_path);
        copilot::Logger logger(copilot::parse_log_level(config.log_level));

        copilot::SimpleHttpServer server(config, copilot::create_app_router(), logger);
        return server.run();
    } catch (const std::exception& error) {
        std::cerr << "fatal: " << error.what() << '\n';
        return 1;
    }
}
