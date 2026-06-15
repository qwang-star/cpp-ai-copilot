#pragma once

#include <map>
#include <string>

namespace copilot {

    // HttpRequest — 解析后的请求对象
struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};

// HttpResponse — 还没发出去的响应对象
struct HttpResponse {
    int status_code = 200;
    std::string reason = "OK";
    std::map<std::string, std::string> headers;
    std::string body;

    static HttpResponse json(int status_code, std::string body);    //函数声明
    std::string serialize() const;  //函数声明
};

HttpRequest parse_http_request(const std::string& raw);  //函数声明
std::string reason_phrase(int status_code);     //函数声明

}  // namespace copilot
