#include "copilot/application.hpp"

#include "copilot/http.hpp"

#include "nlohmann/json.hpp"

#include <optional>
#include <string>

namespace copilot {

static std::optional<std::string> extract_message(const std::string& body) {
    try {
        const auto json = nlohmann::json::parse(body);
        if (!json.contains("message") || !json["message"].is_string()) {
            return std::nullopt;
        }

        const std::string message = json["message"].get<std::string>();
        if (message.empty()) {
            return std::nullopt;
        }

        return message;
    } catch (const nlohmann::json::exception&) {
        return std::nullopt;
    }
}

static std::string make_chat_response_body(const std::string& message) {
    nlohmann::json response_body;
    response_body["code"] = "OK";
    response_body["data"]["reply"] = "我收到了：" + message;
    return response_body.dump();
}

static std::string make_invalid_request_body(const std::string& message) {
    nlohmann::json response_body;
    response_body["code"] = "INVALID_REQUEST";
    response_body["message"] = message;
    response_body["data"] = nullptr;
    return response_body.dump();
}

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


    // 这个接口只认路径和方法，不看 body 内容。
    // 没有发生读取request.body的操作，所以不管你发什么内容，它都回"我收到了：你好"。
    // 没有解析message
    // 没有根据message生成reply
    // router.post("/api/v1/chat", [](const HttpRequest& request) {
    //     return HttpResponse::json(
    //         200, 
    //         R"({"code":"OK","data":{"reply":"我收到了：你好"}})"
    //     );
    // });


    router.post("/api/v1/chat", [](const HttpRequest& request) {
        const auto message = extract_message(request.body);

        if (!message.has_value()) {
            return HttpResponse::json(400, make_invalid_request_body("message is required"));
        }
        
        return HttpResponse::json(
            200, 
            make_chat_response_body(*message)
        );
    });

    return router;                          // 把表交出去
}

}  // namespace copilot
