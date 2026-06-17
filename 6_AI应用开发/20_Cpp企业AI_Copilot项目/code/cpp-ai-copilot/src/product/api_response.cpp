#include "copilot/product/api_response.hpp"

namespace copilot::product {

std::string success_body(const std::string& data_json) {
    return std::string(R"({"code":")") + to_string(ErrorCode::ok) +
           R"(","message":")" + error_message(ErrorCode::ok) +
           R"(","data":)" + data_json + '}';
}

std::string error_body(ErrorCode code) {
    return std::string(R"({"code":")") + to_string(code) +
           R"(","message":")" + error_message(code) +
           R"(","data":null})";
}

}  // namespace copilot::product
