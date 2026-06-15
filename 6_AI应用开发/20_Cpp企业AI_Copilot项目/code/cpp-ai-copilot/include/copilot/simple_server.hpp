#pragma once

#include "copilot/config.hpp"
#include "copilot/logger.hpp"
#include "copilot/router.hpp"

namespace copilot {

class SimpleHttpServer {
public:
    SimpleHttpServer(AppConfig config, Router router, Logger logger);

    int run();

private:
    AppConfig config_;
    Router router_;
    Logger logger_;
};

}  // namespace copilot
