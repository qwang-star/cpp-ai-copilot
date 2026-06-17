#include "copilot/api_response.hpp"

namespace copilot {

std::string make_success_body(const nlohmann::json& data) {
    nlohmann::json response_body;
    response_body["code"] = to_string(ErrorCode::ok);
    response_body["data"] = data;
    return response_body.dump();
}

std::string make_error_body(ErrorCode code, const std::string& message) {
    nlohmann::json response_body;
    response_body["code"] = to_string(code);
    response_body["message"] = message;
    response_body["data"] = nullptr;
    return response_body.dump();
}

}  // namespace copilot

