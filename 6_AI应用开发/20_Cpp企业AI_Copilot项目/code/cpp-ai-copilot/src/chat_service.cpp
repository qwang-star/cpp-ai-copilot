#include "copilot/chat_service.hpp"

#include "copilot/api_response.hpp"
#include "copilot/error_code.hpp"

#include "nlohmann/json.hpp"

#include <optional>
#include <string>

namespace copilot {
namespace {

std::optional<std::string> extract_message(const std::string& body) {
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

}  // namespace

HttpResponse handle_chat_request(const HttpRequest& request) {
    const auto message = extract_message(request.body);

    if (!message.has_value()) {
        return HttpResponse::json(
            400,
            make_error_body(ErrorCode::invalid_request, "message is required"));
    }

    nlohmann::json data;
    data["reply"] = "我收到了：" + *message;

    return HttpResponse::json(200, make_success_body(data));
}

}  // namespace copilot

