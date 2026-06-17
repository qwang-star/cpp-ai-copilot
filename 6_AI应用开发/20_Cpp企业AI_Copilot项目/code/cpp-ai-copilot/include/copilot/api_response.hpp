#pragma once

#include "copilot/error_code.hpp"

#include "nlohmann/json.hpp"

#include <string>

namespace copilot {

std::string make_success_body(const nlohmann::json& data);
std::string make_error_body(ErrorCode code, const std::string& message);

}  // namespace copilot

