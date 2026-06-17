#include "copilot/error_code.hpp"

namespace copilot {

const char* to_string(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return "OK";
        case ErrorCode::invalid_request:
            return "INVALID_REQUEST";
    }

    return "INVALID_REQUEST";
}

}  // namespace copilot

