#include "copilot/router.hpp"

namespace copilot {

void Router::get(const std::string& path, Handler handler) {
    routes_[key("GET", path)] = std::move(handler);
}

void Router::post(const std::string& path, Handler handler) {
    routes_[key("POST", path)] = std::move(handler);
}

HttpResponse Router::route(const HttpRequest& request) const {
    const auto found = routes_.find(key(request.method, request.path));
    if (found == routes_.end()) {
        return HttpResponse::json(404, R"({"error":"route_not_found"})");
    }
    return found->second(request);
}

std::string Router::key(const std::string& method, const std::string& path) {
    return method + " " + path;
}

}  // namespace copilot
