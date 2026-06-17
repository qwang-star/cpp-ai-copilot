#include "copilot/application.hpp"

#include "copilot/http.hpp"

namespace copilot {

static std::string extract_message(const std::string& body){
    const std::string key = R"("message")"; // 只找message
    auto pos = body.find(key);
    if (pos == std::string::npos) {
        return "";  
    }

    pos = body.find(':', pos + key.size());
    if (pos == std::string::npos) {
        return "";  
    }

    pos = body.find('"', pos+1);
    if (pos == std::string::npos) {
        return "";  
    }

    auto end = body.find('"', pos + 1);
    if (end == std::string::npos) {
        return "";  
    }

    return body.substr(pos + 1, end - pos - 1);
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
        const std::string message = extract_message(request.body);

        if(message.empty()) {
            return HttpResponse::json(400,  R"({"code":"INVALID_REQUEST","message":"message is required","data":null})");
        }
        
        return HttpResponse::json(
            200, 
            R"({"code":"OK","data":{"reply":"我收到了：)" + message + R"("}})"
        );
    });

    return router;                          // 把表交出去
}

}  // namespace copilot
