#pragma once

#include "copilot/http.hpp"

namespace copilot {

HttpResponse handle_chat_request(const HttpRequest& request);

}  // namespace copilot

