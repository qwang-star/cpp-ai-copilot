#include "copilot/application.hpp"

#include "copilot/http.hpp"

#if __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#define COPILOT_HAS_NLOHMANN_JSON 1
#else
#define COPILOT_HAS_NLOHMANN_JSON 0
#endif

#include <optional>
#include <string>

namespace copilot {

#if COPILOT_HAS_NLOHMANN_JSON
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
#else
static std::optional<std::string> extract_message(const std::string& body) {
    const std::string key = R"("message")";
    auto pos = body.find(key);
    if (pos == std::string::npos) {
        return std::nullopt;
    }

    pos = body.find(':', pos + key.size());
    if (pos == std::string::npos) {
        return std::nullopt;
    }

    pos = body.find('"', pos + 1);
    if (pos == std::string::npos) {
        return std::nullopt;
    }

    std::string message;
    bool escaping = false;
    for (std::size_t index = pos + 1; index < body.size(); ++index) {
        const char ch = body[index];
        if (escaping) {
            message.push_back(ch);
            escaping = false;
            continue;
        }
        if (ch == '\\') {
            escaping = true;
            continue;
        }
        if (ch == '"') {
            if (message.empty()) {
                return std::nullopt;
            }
            return message;
        }
        message.push_back(ch);
    }

    return std::nullopt;
}

static std::string escape_json_string(const std::string& value) {
    std::string escaped;
    for (const char ch : value) {
        if (ch == '"' || ch == '\\') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

static std::string make_chat_response_body(const std::string& message) {
    return R"({"code":"OK","data":{"reply":"我收到了：)" +
           escape_json_string(message) +
           R"("}})";
}
#endif

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
            return HttpResponse::json(400,  R"({"code":"INVALID_REQUEST","message":"message is required","data":null})");
        }
        
        return HttpResponse::json(
            200, 
            make_chat_response_body(*message)
        );
    });

    return router;                          // 把表交出去
}

}  // namespace copilot
