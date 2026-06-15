#pragma once

#include "copilot/http.hpp"

#include <functional>
#include <map>
#include <string>

namespace copilot {

using Handler = std::function<HttpResponse(const HttpRequest&)>;
//  ↑       ↑                         ↑
//  关键词  新名字           包装一个"函数形状"的 std::function

// Handler 是一个函数类型，接受一个 HttpRequest 参数，返回一个 HttpResponse 对象
// std::function 是 C++ 标准库提供的一个模板类，可以用来存储和调用任何可调用对象（函数、函数指针、lambda 表达式等）。
// 尖括号 <> 里写的是函数签名（收什么、返回什么）。

// 定义一个叫 Handler 的类型，它是"收 HttpRequest 引用 → 返回 HttpResponse"的万能函数包装器。


class Router {
public:
    void get(const std::string& path, Handler handler);   // 函数声明
    void post(const std::string& path, Handler handler);

    HttpResponse route(const HttpRequest& request) const;

private:
    std::map<std::string, Handler> routes_;    // key: "METHOD PATH", value: handler

    static std::string key(const std::string& method, const std::string& path);
};

}  // namespace copilot
