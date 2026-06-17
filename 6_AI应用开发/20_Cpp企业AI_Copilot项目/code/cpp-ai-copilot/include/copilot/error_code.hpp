#pragma once

namespace copilot {

enum class ErrorCode {
    ok,
    invalid_request,
};

const char* to_string(ErrorCode code);

}  // namespace copilot

