#include "copilot/product/error_code.hpp"

namespace copilot::product {

const char* to_string(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return "OK";
        case ErrorCode::invalid_request:
            return "INVALID_REQUEST";
        case ErrorCode::route_not_found:
            return "ROUTE_NOT_FOUND";
        case ErrorCode::method_not_allowed:
            return "METHOD_NOT_ALLOWED";
        case ErrorCode::internal_error:
            return "INTERNAL_ERROR";
    }
    return "INTERNAL_ERROR";
}

const char* error_message(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return "success";
        case ErrorCode::invalid_request:
            return "invalid request";
        case ErrorCode::route_not_found:
            return "route not found";
        case ErrorCode::method_not_allowed:
            return "method not allowed";
        case ErrorCode::internal_error:
            return "internal server error";
    }
    return "internal server error";
}

int http_status(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return 200;
        case ErrorCode::invalid_request:
            return 400;
        case ErrorCode::route_not_found:
            return 404;
        case ErrorCode::method_not_allowed:
            return 405;
        case ErrorCode::internal_error:
            return 500;
    }
    return 500;
}

}  // namespace copilot::product
