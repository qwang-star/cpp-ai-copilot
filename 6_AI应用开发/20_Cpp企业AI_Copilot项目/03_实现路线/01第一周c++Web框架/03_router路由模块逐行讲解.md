# 03 router 路由模块逐行讲解

这一篇专门对照下面两个文件看：

```text
code/cpp-ai-copilot/include/copilot/router.hpp
code/cpp-ai-copilot/src/router.cpp
```

你可以把 `router` 模块理解成：

```text
一家医院的分诊台。
病人（请求）来了 → 说"我挂 GET /health" → 分诊台查表 → 指你去对应诊室（Handler）
```
- `Router` 是后端服务中的核心组件，负责解析客户端请求的路径和方法，以便进行正确的处理。
- 通过将请求映射到相应的处理函数，`Router` 实现了请求的分发机制，提高了代码的可维护性和可扩展性。
- 在 C++ 后端开发中，`Router` 还可以支持中间件功能，允许在请求处理前后执行额外的逻辑，如身份验证和日志记录。
---

## 1. router.hpp 全貌

源码：

```cpp
#pragma once

#include "copilot/http.hpp"

#include <functional>
#include <map>
#include <string>

namespace copilot {

using Handler = std::function<HttpResponse(const HttpRequest&)>;

class Router {
public:
    void get(const std::string& path, Handler handler);
    void post(const std::string& path, Handler handler);

    HttpResponse route(const HttpRequest& request) const;

private:
    std::map<std::string, Handler> routes_;

    static std::string key(const std::string& method, const std::string& path);
};

}  // namespace copilot
```

---

## 2. `#pragma once`

和 `config.hpp` 一样，防止头文件被重复包含。

---

## 3. `#include "copilot/http.hpp"`

为什么 router 需要包含 http？

因为 `Router` 用到了 `HttpRequest` 和 `HttpResponse` 这两个类型，它们定义在 `http.hpp` 里。

```
Router::route 的参数是 (const HttpRequest& request)
Router::route 的返回值是 HttpResponse
Handler 的函数签名是 HttpResponse(const HttpRequest&)
```

编译器看到这些名字时，必须知道它们是什么，所以要包含定义了它们的头文件。

---

## 4. `#include <functional>` 和 `#include <map>`

| 标准库头文件 | 用到了什么 |
|-------------|----------|
| `<functional>` | `std::function` — 万能函数包装器 |
| `<map>` | `std::map` — 键值对容器，存路由表 |
| `<string>` | `std::string` — key 和方法名都用它 |

---

## 5. `namespace copilot`

和所有模块一样，放在项目全局命名空间下，全名是 `copilot::Router`。

---

## 6. `using Handler = ...`（本篇重点）

### 6.1 源码

```cpp
using Handler = std::function<HttpResponse(const HttpRequest&)>;
```

### 6.2 拆成三块看

```cpp
using Handler = std::function<HttpResponse(const HttpRequest&)>;
//  ↑       ↑                         ↑
//  关键词  新名字           包装一个"函数形状"的 std::function
```

### 6.3 第一层：`using A = B` — 给类型起别名

```cpp
using Handler = ...;
```

等于说：**以后写 `Handler` 就等价于写 `...` 那一长串。**

跟你熟悉的没区别：

```cpp
using Age = int;          // Age 就是 int 的别名
Age x = 18;               // 等价于 int x = 18;

using Name = std::string; // Name 就是 string 的别名
Name n = "hello";
```

### 6.4 第二层：`std::function<...>` — 万能函数包装器

`std::function` 可以**把任何可调用的东西装进去**——普通函数、lambda、函数对象，都能装。

尖括号 `<>` 里写的是**函数签名**（收什么、返回什么）。
 
### 6.5 第三层：`HttpResponse(const HttpRequest&)` — 函数签名

```text
HttpResponse(const HttpRequest&)
    ↑              ↑
  返回值          参数

翻译：收一个 HttpRequest 引用，返回一个 HttpResponse
```

### 6.6 合起来翻译

> 定义一个叫 `Handler` 的新类型名，它是"收 HttpRequest 引用 → 返回 HttpResponse"的**万能函数包装器**。

### 6.7 为什么需要它

不用别名前，到处都要写这么长：

```cpp
// router.hpp — 太长了，眼睛累
class Router {
    std::map<std::string, std::function<HttpResponse(const HttpRequest&)>> routes_;
    void get(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler);
};
```

用了别名后：

```cpp
using Handler = std::function<HttpResponse(const HttpRequest&)>;

class Router {
    std::map<std::string, Handler> routes_;    // 干净
    void get(const std::string& path, Handler handler);  // 清爽
};
```

### 6.8 为什么用 `std::function` 而不是普通函数指针

因为 Handler 可以是**两种东西**，`std::function` 都装得下：

```cpp
// 情况1：普通函数
HttpResponse health_handler(const HttpRequest& req) {
    return HttpResponse::json(200, "...");
}
router.get("/health", health_handler);  // ✅ 装得下

// 情况2：lambda 表达式（这个项目实际用的就是这个）
router.get("/health", [](const HttpRequest&) {
    return HttpResponse::json(200, "...");
});                                     // ✅ 也装得下
```

如果用普通函数指针，lambda 带捕获列表时就会出错。`std::function` 是"万能包装器"，什么都能装。

### 6.9 在你项目里的完整链路

```
router.hpp：
    using Handler = std::function<HttpResponse(const HttpRequest&)>;
    // 定义了 Handler 这个类型名

    void get(const std::string& path, Handler handler);
    //                                    ↑ 用 Handler 作参数类型

    std::map<std::string, Handler> routes_;
    //                        ↑ 用 Handler 作 map 的 value 类型

application.cpp：
    router.get("/health", [](const HttpRequest&) { return ...; });
    //                      └──────────────┬──────────────┘
    //                 这个 lambda 的形状是：收 (const HttpRequest&)，返回 HttpResponse
    //
    // get() 的参数类型是 Handler，而 Handler 的定义是：
    //   std::function<HttpResponse(const HttpRequest&)>
    //
    // std::function 的构造函数接受任何"形状匹配"的可调用对象，
    // 所以 lambda 被隐式转换成了 Handler 类型，塞进了参数。
    // 不是"类型推导"，是"隐式转换"——std::function 吞下了这个 lambda。
```

---

## 7. `class Router` — 路由表类

### 7.1 源码

```cpp
class Router {
public:
    void get(const std::string& path, Handler handler);
    void post(const std::string& path, Handler handler);

    HttpResponse route(const HttpRequest& request) const;

private:
    std::map<std::string, Handler> routes_;

    static std::string key(const std::string& method, const std::string& path);
};
```

### 7.2 `class` 是什么

`class` 和 `struct` 几乎一样，都是自定义数据类型。唯一的区别是：

| | struct | class |
|--|--------|-------|
| 默认访问权限 | `public`（谁都能碰） | `private`（外面不能碰） |

为什么 Router 用 `class` 而 AppConfig 用 `struct`？

`AppConfig` 只是装数据的盒子，字段直接读就行。`Router` 有一个内部的 `routes_` 表需要保护——**外部不能直接改路由表**，必须通过 `get()` / `post()` 方法注册。

### 7.3 `public:` — 公开接口

```cpp
public:
    void get(const std::string& path, Handler handler);
    void post(const std::string& path, Handler handler);
    HttpResponse route(const HttpRequest& request) const;
```

这三个是外部可以调的：

| 方法                    | 做什么               | 谁调                                        |
| --------------------- | ----------------- | ----------------------------------------- |
| `get(path, handler)`  | 注册一条 GET 路由       | `application.cpp` 的 `create_app_router()` |
| `post(path, handler)` | 注册一条 POST 路由      | 以后加新接口时用                                  |
| `route(request)`      | 根据请求分发到对应 Handler | `simple_server.cpp` 每次收到请求时调              |

### 7.3.1 `get()` 和 `post()` 深入理解

`get()` 和 `post()` 是**注册函数**——往路由表里记一条规则。

```cpp
void get(const std::string& path, Handler handler);
//         ↑                         ↑
//    URL 路径，如 "/health"    处理这个路径的函数（普通函数或 lambda）
```

**调用示例：**

```cpp
// 注册：当收到 GET /health 时，用这个 lambda 处理
router.get("/health", [](const HttpRequest&) {
    return HttpResponse::json(200, R"({"status":"ok"})");
});

// 以后注册更多接口：
router.get("/users", user_list_handler);      // GET /users → 用户列表
router.post("/upload", upload_handler);       // POST /upload → 上传文件
router.post("/chat", chat_handler);           // POST /chat → 聊天
```

**内部干了什么：**

```cpp
// router.cpp
void Router::get(const std::string& path, Handler handler) {
    routes_[key("GET", path)] = std::move(handler);
}
//         ↑
//    routes_ 就是那张 map 路由表
//    把 "GET /health" → handler 存进去
```

翻译成人话：

> 在路由表里记一条：**以后凡是 `GET /health` 的请求，统统交给这个 handler 处理。**

**类比——医院分诊台：**

```text
router.get("/health", handler_A)
  = 你在本子上记：挂"健康检查"的病人 → 去 1 号诊室

router.get("/users", handler_B)
  = 你再记一条：挂"用户列表"的病人 → 去 2 号诊室

router.post("/upload", handler_C)
  = 再记：来做"上传"检查的病人 → 去 3 号诊室

以后病人来了说"我挂健康检查" → 你翻本子 → 查到 1 号诊室 → 指他过去
```

**`get()` 和 `post()` 的唯一区别**就是 key 的前缀：

```cpp
void Router::get(const std::string& path, Handler handler) {
    routes_[key("GET", path)] = std::move(handler);     // key = "GET /health"
}

void Router::post(const std::string& path, Handler handler) {
    routes_[key("POST", path)] = std::move(handler);    // key = "POST /upload"
}
```

一个存 `"GET /xxx"`，一个存 `"POST /xxx"`，其余完全一样。

### 7.3.2`HttpResponse route(const HttpRequest& request) const;`

### 拆成三块看

```cpp
HttpResponse route(const HttpRequest& request) const;
//  ↑            ↑                            ↑
//  返回值类型    函数名+参数                    第2个const
```

### 第 1 块：`HttpResponse` — 返回值

这个函数执行完，返回一个 `HttpResponse` 对象。

### 第 2 块：`route(const HttpRequest& request)` — 函数名和参数

```
route(                          — 函数名叫 route
    const HttpRequest& request  — 收一个 HttpRequest 引用，标记 const 表示不修改它
)
```

### 第 3 块：`const` — 承诺不修改对象自身

```cpp
... const;
//   ↑
//   这个 const 是承诺：route() 函数不会修改 Router 对象本身
```

它修饰的是**函数所属的对象**（即调用它的那个 Router），不是你传进去的 `request`。

### 两个 const 的区别

```cpp
HttpResponse route(const HttpRequest& request) const;
//                 └────────┬────────┘        └┬┘
//              承诺不修改参数 request      承诺不修改 Router 本身
```

|const 位置|修饰谁|意思|
|---|---|---|
|`const HttpRequest& request`|参数|不会改传入的请求|
|末尾的 `const`|Router 对象|不会改路由表|

### 为什么末尾要加 const

`route()` 只做查表和调用 Handler，不修改路由表本身。加 `const` 是一种**自我约束+告诉别人**——"我是只读操作，放心调"。

### 7.4 `private:` — 私有成员

```cpp
private:
    std::map<std::string, Handler> routes_;
    static std::string key(const std::string& method, const std::string& path);
```

`routes_` 是路由表本体，外部不能直接碰。`key()` 是内部工具函数，外部不需要知道。

### 7.5 `routes_` — 路由表的数据结构

```cpp
std::map<std::string, Handler> routes_;
//       ↑ key 的类型    ↑ value 的类型
//   key: "METHOD PATH", value: handler
```

这是一个 `map`（字典），存的是：

```
"GET /health"     →  Handler_A（处理健康检查的 lambda）
"POST /upload"    →  Handler_B（处理上传的函数）
"GET /chat"       →  Handler_C（处理聊天的函数）
```

`std::map` 的作用就是**快速查找**：给一个字符串 key，瞬间找到对应的 Handler。

### 7.6 `static std::string key(...)` — 拼凑 key

```cpp
static std::string key(const std::string& method, const std::string& path);
```

`static` 表示这个函数**不属于任何一个 Router 对象**，直接通过类名调用：

```cpp
Router::key("GET", "/health")   // → "GET /health"
Router::key("POST", "/upload")  // → "POST /upload"
```

作用就是把 method 和 path 中间加个空格，拼成 map 的 key。

### 7.7 `const` 在 `route()` 后面 — 两个 const 的区别

```cpp
HttpResponse route(const HttpRequest& request) const;
//                 └────────┬────────┘        └┬┘
//              承诺不修改参数 request      承诺不修改 Router 本身
```

**逐块拆解：**

```cpp
HttpResponse route(const HttpRequest& request) const;
//  ↑            ↑                            ↑
//  返回值类型    函数名+参数                    第2个const
```

| 块 | 代码 | 含义 |
|----|------|------|
| 返回值 | `HttpResponse` | 执行完返回一个 HttpResponse 对象 |
| 函数名+参数 | `route(const HttpRequest& request)` | 函数叫 route，收一个 HttpRequest 引用，标记 const 表示不修改它 |
| 末尾 const | `const` | 承诺 route() 不会修改 Router 对象自身 |

**两个 const 修饰的不是同一个东西：**

| const 位置 | 修饰谁 | 意思 |
|-----------|--------|------|
| `const HttpRequest& request` | **参数** request | 不会改传进来的请求 |
| 末尾的 `const` | **Router 对象** | 不会改路由表（routes_） |

**为什么末尾要加 const：**

`route()` 只做两件事——查表（`routes_.find`）和调用 Handler。它不往路由表里增删任何东西。加 `const` 是一种**自我约束 + 告诉调用者**——"我是只读操作，放心调，不会改你的路由表。"

---

## 8. router.cpp — 路由表实现

源码：

```cpp
#include "copilot/router.hpp"

namespace copilot {

// 函数定义
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
```

### 8.1 `Router::get(...)` — 注册 GET 路由

```cpp
void Router::get(const std::string& path, Handler handler) {
    routes_[key("GET", path)] = std::move(handler);
}
```

逐段翻译：

```
Router::get
  ↑
 Router 类的 get 方法

routes_[key("GET", path)] = handler;
  ↑        ↑                  ↑
路由表  拼 key：          存入 Handler
        "GET /health"

std::move(handler)
  ↑
把 handler 的"所有权"转给 routes_，
而不是复制一份（省资源）
```

执行效果：

```text
"GET /health" → Handler（那个 lambda）
```

被存进 `routes_` 这个 map 里。

### 8.2 `Router::post(...)` — 注册 POST 路由

逻辑和 `get()` 完全一样，只是 key 拼的是 `"POST " + path`。

### 8.3 `Router::route(...)` — 分发请求（核心方法）

```cpp
HttpResponse Router::route(const HttpRequest& request) const {
    // 第 1 步：拼 key
    const auto found = routes_.find(key(request.method, request.path));
    //                            ↑
    //                  key("GET", "/health") → "GET /health"
    //                  map::find 在 map 里查找这个 key
    //                  返回一个"迭代器"（类似指向找到位置的指针）

    // 第 2 步：没找到 → 返回 404
    if (found == routes_.end()) {
        // routes_.end() 是 map 的特殊值，表示"没找到"
        return HttpResponse::json(404, R"({"error":"route_not_found"})");
    }

    // 第 3 步：找到了 → 调用 Handler，传入请求，返回响应
    return found->second(request);
    //     ↑           ↑      ↑
    //  found 指向      取值     把 request
    //  map 元素     部分      传给 Handler
    //             (Handler)
}
```

逐条解释：

#### `routes_.find(key(...))`

```cpp
routes_.find("GET /health")
```

在 map 里查 `"GET /health"` 这个 key。

- **找到了**：返回一个迭代器，指向这个 map 元素
- **没找到**：返回 `routes_.end()`（一个特殊标记，表示"不存在"）

#### `if (found == routes_.end())`

迭代器指向 `end()`，说明没找到 → 返回 404。

#### `found->second(request)`

`found` 指向 map 的一个元素，map 元素是键值对：

```cpp
found->first   // key："GET /health"
found->second  // value：Handler 函数
```

`found->second(request)` 就是**调用这个 Handler，把 `request` 传进去，拿到 `response`**。

这就是"路由分发"的核心动作——查表 → 调函数。

### 8.4 `Router::key(...)` — 拼凑方法+路径

```cpp
std::string Router::key(const std::string& method, const std::string& path) {
    return method + " " + path;
}
```

C++ 里 `string + string` 就是拼接：

```cpp
"GET" + " " + "/health"  →  "GET /health"
```

---

## 9. 为什么 Router 要单独成一个模块

```text
如果把路由逻辑写在 SimpleHttpServer 里：
  → 新增接口要改服务器代码
  → 路由匹配逻辑和网络层耦合
  → 难以测试

单独 Router 模块：
  → 加新接口只改 application.cpp
  → 路由逻辑可以独立测试
  → 网络层不关心路由细节
```

这就是"单一职责原则"：Router 只管查表分发，服务器只管收发包。

---

## 10. 完整工作流程

分两个阶段：**注册阶段**（程序启动时，只执行一次）和**运行阶段**（每次来请求，反复执行）。

---

### 注册阶段（main.cpp 启动 → application.cpp → router.cpp）

```text
main.cpp                              application.cpp                     router.cpp
───────                               ────────────────                    ──────────
server 构造时调用了                     
create_app_router()           →        Router router;             →        routes_ = {}（空表）
                                       router.get("/health", λ)   →        routes_["GET /health"] = λ
                                       return router;             →        把填好的 Router 返回
                                       
                                server 收到这个 Router，存进 router_ 成员。
```

注册阶段只做一件事：**把路由表填好**。目前只有一条：

```text
routes_ = {
    "GET /health"  →  lambda（返回 {"status":"ok"} 的那个）
}
```

---

### 运行阶段（每次浏览器发请求 → simple_server.cpp → router.cpp）

```text
simple_server.cpp                     router.cpp
─────────────────                     ──────────
收到 HTTP 文本 "GET /health HTTP/1.1..."
      ↓
parse_http_request(raw)  →  HttpRequest {method:"GET", path:"/health", ...}
      ↓
router_.route(request)        →        routes_.find("GET /health")
                                             ↓
                                       找到了！found->second(request)
                                             ↓
                                       lambda 执行，返回 HttpResponse {200, body:{"status":"ok"}}
      ↓
response.serialize()  →  "HTTP/1.1 200 OK\r\n..."
      ↓
send() 发回浏览器
```

运行阶段每次收到请求都走这条路：**收 → 解析 → 路由分发 → 序列化 → 发**。

---

## 11. 这段代码你最该掌握什么

```text
1. using Handler = std::function<...> 给函数类型起别名，简化代码
2. class 和 struct 的区别：默认 public vs 默认 private
3. std::map 是键值对字典，find() 查找，end() 表示没找到
4. Router 只做两件事：get/post 注册路由，route 分发请求
5. static 函数属于类本身，调用写法是 Router::key(...)
6. 方法后面加 const = 承诺不修改对象
7. found->second = 取 map 元素的 value 部分
```

掌握这些之后，你再去看 `simple_server.cpp` 里怎么调 `router_.route(request)`，链路就完全通了。
