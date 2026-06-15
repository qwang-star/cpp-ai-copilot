#include "copilot/http.hpp"

#include <sstream>

namespace copilot {

std::string reason_phrase(int status_code) {
    switch (status_code) {
        case 200:
            return "OK";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 500:
            return "Internal Server Error";
        default:
            return "OK";
    }
}

HttpResponse HttpResponse::json(int status_code_value, std::string response_body) {
    HttpResponse response;
    response.status_code = status_code_value;
    response.reason = reason_phrase(status_code_value);
    response.body = std::move(response_body);
    response.headers["Content-Type"] = "application/json; charset=utf-8";
    response.headers["Connection"] = "close";
    return response;
}

std::string HttpResponse::serialize() const {
    std::ostringstream output;
    output << "HTTP/1.1 " << status_code << ' ' << reason << "\r\n";
    for (const auto& [name, value] : headers) {
        output << name << ": " << value << "\r\n";
    }
    output << "Content-Length: " << body.size() << "\r\n";
    output << "\r\n";
    output << body;
    return output.str();
}

HttpRequest parse_http_request(const std::string& raw) {
    HttpRequest request;
    std::istringstream input(raw);

    input >> request.method >> request.path >> request.version;
    std::string line;
    std::getline(input, line);

    while (std::getline(input, line)) {
        if (line == "\r" || line.empty()) {
            break;
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        const auto separator = line.find(':');
        if (separator == std::string::npos) {
            continue;
        }
        std::string name = line.substr(0, separator);
        std::string value = line.substr(separator + 1);
        if (!value.empty() && value.front() == ' ') {
            value.erase(value.begin());
        }
        request.headers[name] = value;
    }

    std::ostringstream body;
    body << input.rdbuf();
    request.body = body.str();

    return request;
}

}  // namespace copilot
