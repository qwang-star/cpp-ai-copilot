#include "copilot/api_response.hpp"
#include "copilot/application.hpp"
#include "copilot/chat_service.hpp"
#include "copilot/config.hpp"
#include "copilot/error_code.hpp"
#include "copilot/http.hpp"
#include "copilot/router.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>

using namespace copilot;

void test_json_response_has_status_content_type_and_body() {
    HttpResponse response = HttpResponse::json(200, R"({"status":"ok"})");

    assert(response.status_code == 200);
    assert(response.reason == "OK");
    assert(response.headers.at("Content-Type") == "application/json; charset=utf-8");
    assert(response.body == R"({"status":"ok"})");
}

void test_router_returns_health_response() {
    Router router;
    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(200, R"({"status":"ok","service":"cpp-ai-copilot"})");
    });

    HttpRequest request;
    request.method = "GET";
    request.path = "/health";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find(R"("status":"ok")") != std::string::npos);
    assert(response.body.find(R"("service":"cpp-ai-copilot")") != std::string::npos);
}

void test_router_returns_404_for_unknown_route() {
    Router router;

    HttpRequest request;
    request.method = "GET";
    request.path = "/missing";

    HttpResponse response = router.route(request);

    assert(response.status_code == 404);
    assert(response.body.find("route_not_found") != std::string::npos);
}

void test_config_loads_key_value_file() {
    const std::string path = "tmp_test_app.env";
    {
        std::ofstream out(path);
        out << "APP_HOST=127.0.0.1\n";
        out << "APP_PORT=9090\n";
        out << "LOG_LEVEL=debug\n";
    }

    AppConfig config = AppConfig::load(path);

    assert(config.host == "127.0.0.1");
    assert(config.port == 9090);
    assert(config.log_level == "debug");

    std::remove(path.c_str());
}

void test_application_router_registers_health_route() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "GET";
    request.path = "/health";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find(R"("version":"0.1.0")") != std::string::npos);
}

void test_error_code_to_string() {
    assert(std::string(to_string(ErrorCode::ok)) == "OK");
    assert(std::string(to_string(ErrorCode::invalid_request)) == "INVALID_REQUEST");
}

void test_api_response_builds_success_body() {
    nlohmann::json data;
    data["reply"] = "我收到了：你好";

    const std::string body = make_success_body(data);

    assert(body.find(R"("code":"OK")") != std::string::npos);
    assert(body.find("我收到了：你好") != std::string::npos);
}

void test_api_response_builds_error_body() {
    const std::string body = make_error_body(ErrorCode::invalid_request, "message is required");

    assert(body.find(R"("code":"INVALID_REQUEST")") != std::string::npos);
    assert(body.find(R"("message":"message is required")") != std::string::npos);
    assert(body.find(R"("data":null)") != std::string::npos);
}

// 测 {"message":"你好"}
void test_chat_returns_reply_from_message() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":"你好"})";

    HttpResponse response = router.route(request);

    //std::cout << "response.body = [" << response.body << "]\n";

    assert(response.status_code == 200);
    assert(response.body.find(R"("code":"OK")") != std::string::npos);
    assert(response.body.find("我收到了：你好") != std::string::npos);
}

void test_chat_service_returns_reply_from_message() {
    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":"你好"})";

    HttpResponse response = handle_chat_request(request);

    assert(response.status_code == 200);
    assert(response.body.find("我收到了：你好") != std::string::npos);
}

void test_chat_returns_400_when_message_missing() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"text":"你好"})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 400);
    assert(response.body.find("INVALID_REQUEST") != std::string::npos);
}

void test_chat_returns_400_when_message_is_not_string() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":123})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 400);
    assert(response.body.find("INVALID_REQUEST") != std::string::npos);
}

void test_chat_returns_400_when_message_is_empty() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":""})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 400);
    assert(response.body.find("INVALID_REQUEST") != std::string::npos);
}

void test_chat_returns_400_when_json_is_malformed() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":"你好")";

    HttpResponse response = router.route(request);

    assert(response.status_code == 400);
    assert(response.body.find("INVALID_REQUEST") != std::string::npos);
}

// 测了更常见的 {"message": "你好"}，也就是冒号后面带空格的情况
void test_chat_accepts_space_after_message_colon() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message": "你好"})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find("我收到了：你好") != std::string::npos);
}

// 测了 {"message":"他说\"你好\""}，也就是 message 里带转义引号的情况
void test_chat_handles_escaped_quotes_in_message() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":"他说\"你好\""})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find(R"(我收到了：他说\"你好\")") != std::string::npos);
}

int main() {
    test_json_response_has_status_content_type_and_body();
    test_router_returns_health_response();
    test_router_returns_404_for_unknown_route();
    test_config_loads_key_value_file();
    test_application_router_registers_health_route();
    test_error_code_to_string();
    test_api_response_builds_success_body();
    test_api_response_builds_error_body();
    test_chat_returns_reply_from_message();
    test_chat_service_returns_reply_from_message();
    test_chat_returns_400_when_message_missing();
    test_chat_returns_400_when_message_is_not_string();
    test_chat_returns_400_when_message_is_empty();
    test_chat_returns_400_when_json_is_malformed();
    test_chat_accepts_space_after_message_colon();
    test_chat_handles_escaped_quotes_in_message();
}
