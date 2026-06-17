#pragma once

namespace copilot::product {

enum class ErrorCode {
    ok,
    invalid_request,
    route_not_found,
    method_not_allowed,
    internal_error,
};

const char* to_string(ErrorCode code);
const char* error_message(ErrorCode code);
int http_status(ErrorCode code);

}  // namespace copilot::product
