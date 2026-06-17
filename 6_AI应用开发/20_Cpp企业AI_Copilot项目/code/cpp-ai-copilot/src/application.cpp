#include "copilot/application.hpp"

#include "copilot/chat_service.hpp"
#include "copilot/http.hpp"

namespace copilot {

Router create_app_router() {
    Router router;              // 建一个空表

    router.get("/health", [](const HttpRequest&) {       // 往表里注册 Handler
        return HttpResponse::json(
            200,
            R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
    });

    router.get("/api/v1/ping", [](const HttpRequest&) {
        return HttpResponse::json(200, R"({"message":"pong"})");
    });


    router.post("/api/v1/chat", [](const HttpRequest& request) {
        return handle_chat_request(request);
    });

    return router;                          // 把表交出去
}

}  // namespace copilot
