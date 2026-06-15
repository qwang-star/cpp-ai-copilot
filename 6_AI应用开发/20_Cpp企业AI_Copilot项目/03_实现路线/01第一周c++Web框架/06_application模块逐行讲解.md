# 06 application 模块逐行讲解

这一篇专门对照下面两个文件看：

```text
code/cpp-ai-copilot/include/copilot/application.hpp
code/cpp-ai-copilot/src/application.cpp
```

你可以把 `application` 模块理解成：

```text
公司的"业务登记表"。
公司开张前，把有哪些窗口（接口）、每个窗口谁负责（Handler）
全部登记好，然后交给前台（Router）。
```

---

## 1. 这个模块为什么这么小

整个模块只有一个函数：

```text
application.hpp → 7 行
application.cpp → 20 行
```

**这是故意的。** 它的职责极单纯：创建 Router，往里注册 Handler，返回 Router。以后项目变复杂了，所有新接口都只在这里加——别的模块不用动。

---

## 2. application.hpp 全貌

源码：

```cpp
#pragma once

#include "copilot/router.hpp"

namespace copilot {

Router create_app_router();

}  // namespace copilot
```

### 2.1 逐行解释

```cpp
#include "copilot/router.hpp"
```

需要 `Router` 这个类型——函数返回值是 `Router`。

```cpp
Router create_app_router();
// ↑           ↑
// 返回类型   函数名（创建应用的路由表）
```

**声明**一个函数，告诉编译器：

> 存在一个叫 `create_app_router` 的函数，它不接受参数，返回一个配置好的 `Router` 对象。

实现放在 `.cpp` 里。

---

## 3. application.cpp 全貌

源码：

```cpp
#include "copilot/application.hpp"
#include "copilot/http.hpp"

namespace copilot {

Router create_app_router() {
    Router router;

    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(
            200,
            R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
    });

    return router;
}

}  // namespace copilot
```

### 3.1 为什么还包含了 `http.hpp`

虽然 `application.hpp` 只包含 `router.hpp`，但 `.cpp` 里多包含了 `http.hpp`，因为 Handler（那个 lambda）里用到了 `HttpRequest` 和 `HttpResponse`。

```cpp
[](const HttpRequest&) {                    // ← HttpRequest 定义在 http.hpp
    return HttpResponse::json(200, "...");  // ← HttpResponse 定义在 http.hpp
}
```

---

## 4. 逐行翻译

### 4.1 函数签名

```cpp
Router create_app_router() {
```

`Router` 是自建类型，跟 `int`、`string` 没本质区别——函数返回一个 `Router` 对象。

### 4.2 建空表

```cpp
    Router router;
```

此时 `router` 内部 `routes_` 是空的——还没有注册任何接口。

### 4.3 注册路由

```cpp
    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(
            200,
            R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
    });
```

拆开看：

| 部分                               | 含义                                |
| -------------------------------- | --------------------------------- |
| `router.get(...)`                | 调 Router 的 `get()` 方法，注册一条 GET 路由 |
| `"/health"`                      | URL 路径，以后 `GET /health` 的请求归它处理   |
| `[](const HttpRequest&) { ... }` | 处理函数（Handler），一个 lambda           |
| `HttpResponse::json(200, ...)`   | 工厂方法，创建一个 200 响应，body 是 JSON      |
| `R"(...)"`                       | 原始字符串，避免 JSON 里的 `"` 需要转义         |

**效果**：在路由表里记了一条：

```text
"GET /health"  →  返回 {"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

### 4.4 返回路由表

```cpp
    return router;
}
```

把填好的 Router 交出去。谁调 `create_app_router()` 谁拿到配置好的路由表。

---

## 5. 它在整个项目里的位置

```text
main.cpp
    ↓
create_app_router()          ← application.cpp（就是这里）
    ↓
返回填好的 Router 对象
    ↓
传给 SimpleHttpServer 构造函数
    ↓
server 存进 router_ 成员
    ↓
server.run() 里每次收到请求 → router_.route(request)
```

它是 `main.cpp` 和 `Router` 之间的**唯一桥梁**。

---

## 6. 以后怎么加新接口

现在只有一个 `/health`。以后你要加 RAG、聊天、文档上传，全在这里改。

比如加一个获取用户列表的接口：

```cpp
Router create_app_router() {
    Router router;

    // 原来的 /health
    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(200, R"({"status":"ok"})");
    });

    // 新加的 /users
    router.get("/users", [](const HttpRequest&) {
        std::string users_json = get_all_users_from_db();  // 你的业务逻辑
        return HttpResponse::json(200, users_json);
    });

    // 新加的 /chat（POST）
    router.post("/chat", [](const HttpRequest& request) {
        // request.body 里有用户发来的 JSON
        // 调用 AI 模型，返回结果
        return HttpResponse::json(200, ai_response);
    });

    return router;
}
```

**只改这一个文件**，Router 不用动，server 不用动，http 不用动。

---

## 7. 这段代码你最该掌握什么

```text
1. application 是项目的"业务入口"——所有接口在这里注册
2. create_app_router() 返回填好路由的 Router 对象
3. router.get("/health", lambda) — 注册一条路由的完整写法
4. lambda 作为 Handler 传入 Router，体现了 std::function 的万能包装能力
5. 以后加新接口只改这一个文件——这就是"开闭原则"的体现
```
