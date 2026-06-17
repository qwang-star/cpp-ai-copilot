# 09 test 测试代码逐行讲解

对照文件：

```text
code/cpp-ai-copilot/tests/test_core.cpp
```

> 你可以把 test 理解成：**每次改完代码后自动跑一遍的"健康检查"**——7 个测试逐个验证每个模块是否正常工作。

---

## 1. 这个文件在项目里的位置

```text
CMakeLists.txt
    ↓
add_executable(test_core tests/test_core.cpp)    ← 把 test_core.cpp 编译成 test_core.exe
    ↓
target_link_libraries(test_core PRIVATE copilot_core)  ← 链接核心库
    ↓
enable_testing()
add_test(NAME core COMMAND test_core)  ← 注册为可自动运行的测试
```

运行方式：

```powershell
# 方式一：先编译，再手动跑
cmake --build build
.\build\Debug\test_core.exe

# 方式二：让 CMake 帮你跑
cd build
ctest
```

如果 7 个测试全部通过（没有任何一个 `assert` 触发），程序正常退出，返回 0。任何一个 `assert` 失败，程序立即终止并打印失败行号。

---

## 2. 文件头：依赖和宏

```cpp
#include "copilot/application.hpp"
#include "copilot/config.hpp"
#include "copilot/http.hpp"
#include "copilot/router.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
```

按需引入，不引入不需要的：

| 头文件               | 测试里为什么要它                                  |
| ----------------- | ----------------------------------------- |
| `application.hpp` | 调 `create_app_router()`                   |
| `config.hpp`      | 调 `AppConfig::load()`                     |
| `http.hpp`        | 构造 `HttpRequest`，调 `HttpResponse::json()` |
| `router.hpp`      | 构造 `Router`，调 `router.route()`            |
| `<cassert>`       | **核心**：提供 `assert()` 宏                    |
| `<cstdio>`        | `std::remove()` 删临时文件                     |
| `<fstream>`       | `std::ofstream` 写临时 .env 文件               |
| `<string>`        | `std::string`                             |

```cpp
using namespace copilot;
```

偷懒写法——省得每个类型前面都写 `copilot::`。写测试无所谓，正式代码别这么干。

---

## 3. `assert()` 是什么

这是整个测试文件的核心机制。

```cpp
#include <cassert>

assert(条件);
// 条件为 true   → 什么都不发生，继续往下走
// 条件为 false  → 程序立刻终止，打印文件名、行号、失败的条件
```

**它不是异常，不通过 try/catch 捕获**。就是最粗暴的："你说这个一定是 true？不是？那我死给你看。"

```cpp
assert(response.status_code == 200);
//              ↑
//     如果 status_code 不是 200，程序直接崩，告诉你哪一行挂了
```

每个测试函数末尾没有 `return true/false`——只要跑到最后一行还没崩，就表示全通过。

### `assert` 的副作用陷阱（重要常识）

```cpp
// ❌ 千万别这样写
assert(open_database_connection() == true);
// 发布版本（NDEBUG 定义时）assert 会被编译器删掉，
// 那 open_database_connection() 就永远不会被调用了！
```

```cpp
// ✅ 正确写法
bool ok = open_database_connection();
assert(ok);
```

本项目的测试不存在这个问题——所有 `assert` 的条件都是纯读取，没有副作用。

---

## 4. 测试 1：验证 HttpResponse::json() 工厂方法

```cpp
void test_json_response_has_status_content_type_and_body() {
    HttpResponse response = HttpResponse::json(200, R"({"status":"ok"})");

    assert(response.status_code == 200);
    assert(response.reason == "OK");
    assert(response.headers.at("Content-Type") == "application/json; charset=utf-8");
    assert(response.body == R"({"status":"ok"})");
}
```

**测什么**：`HttpResponse::json()` 这个静态工厂方法到底有没有把该填的字段都填对。

| 断言 | 验证内容 |
|---|---|
| `status_code == 200` | 状态码传进去了 |
| `reason == "OK"` | 原因短语自动补上了（`reason_phrase(200)` → `"OK"`） |
| `headers.at("Content-Type") == "application/json; charset=utf-8"` | Content-Type 头自动设置了 |
| `body == R"({"status":"ok"})"` | JSON 字符串完整保存了 |

**对应源码**：`http.cpp` 里的 `HttpResponse::json()` 静态方法。

**这个测试保证了**：以后任何人改 `HttpResponse::json()`，只要没改这 4 个字段的行为，测试就通过；改了的话立刻崩。

---

## 5. 测试 2：验证 Router 能找到并执行注册过的 Handler

```cpp
void test_router_returns_health_response() {
    Router router;
    router.get("/health", [](const HttpRequest&) {
        return HttpResponse::json(200, R"({"status":"ok","service":"cpp-ai-copilot"})");
    });

    HttpRequest request;
    request.method = "GET";
    request.path = "/health";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find(R"("status":"ok")") != std::string::npos);
    assert(response.body.find(R"("service":"cpp-ai-copilot")") != std::string::npos);
}
```

### 5.1 分步拆解

**第 1 步：造一个带路由的 Router**

```cpp
Router router;
router.get("/health", [](const HttpRequest&) {
    return HttpResponse::json(200, R"({"status":"ok","service":"cpp-ai-copilot"})");
});
```

注意：这里没有走 `create_app_router()`，而是**手动创建一个 Router 并注册 Handler**。这是"单元测试"的思路——不依赖 application 模块，只测 Router 自身的行为。

**第 2 步：手工构造一个 HttpRequest**

```cpp
HttpRequest request;
request.method = "GET";
request.path = "/health";
```

真实的请求是 `parse_http_request()` 从 socket 数据里解析出来的。测试里不需要走网络——直接手填一个 HttpRequest 对象，假装收到了 `GET /health`。

**第 3 步：让 Router 分发**

```cpp
HttpResponse response = router.route(request);
```

等价于真实运行时 `simple_server.cpp` 里的 `router_.route(request)`。

**第 4 步：验货**

```cpp
assert(response.body.find(R"("status":"ok")") != std::string::npos);
//          ↑                                    ↑
//     在 body 里找 "status":"ok"            找到了（不是 npos）
```

```cpp
assert(response.body.find(R"("service":"cpp-ai-copilot")") != std::string::npos);
```

为什么用 `find() != npos` 而不是 `==`？因为 body 里可能还有其他内容（比如格式化空格），只要包含这些关键字就算通过。这是更宽松、更健壮的断言方式。

---

## 6. 测试 3：验证不存在的路由返回 404

```cpp
void test_router_returns_404_for_unknown_route() {
    Router router;

    HttpRequest request;
    request.method = "GET";
    request.path = "/missing";

    HttpResponse response = router.route(request);

    assert(response.status_code == 404);
    assert(response.body.find("route_not_found") != std::string::npos);
}
```

**注意**：这里创建的 `Router` 是空的——一条路由都没注册。请求 `/missing`，Router 在 `routes_` map 里找 `"GET /missing"` → 找不到 → 返回 404。

验证两点：

| 断言 | 含义 |
|---|---|
| `status_code == 404` | 明确返回 404，不是 200 也不是 500 |
| `body.find("route_not_found") != npos` | 错误信息里包含 `route_not_found`，前端可以根据这个字符串做处理 |

**对应源码**：`router.cpp` 的 `Router::route()` 末尾——`find()` 失败后返回 404。

---

## 7. 测试 4：验证 AppConfig::load() 能正确读取 .env 文件

```cpp
void test_config_loads_key_value_file() {
    const std::string path = "tmp_test_app.env";
    {
        std::ofstream out(path);
        out << "APP_HOST=127.0.0.1\n";
        out << "APP_PORT=9090\n";
        out << "LOG_LEVEL=debug\n";
    }

    AppConfig config = AppConfig::load(path);

    assert(config.host == "127.0.0.1");
    assert(config.port == 9090);
    assert(config.log_level == "debug");

    std::remove(path.c_str());
}
```

### 7.1 为什么要创建和删除临时文件

这个测试需要测"从文件读配置"，但又不能依赖 `config/app.env` 的真实内容（那个内容可能被人改过）。

做法：

```cpp
// ① 创建一个临时文件，写入已知内容
std::ofstream out(path);
out << "APP_HOST=127.0.0.1\n";
// ...

// ② 用 AppConfig::load() 去读它

// ③ 验证读出来的值对不对

// ④ 删掉临时文件，不留痕迹
std::remove(path.c_str());
```

### 7.2 那个花括号 `{ }` 是干嘛的

```cpp
{
    std::ofstream out(path);
    out << "APP_HOST=127.0.0.1\n";
    out << "APP_PORT=9090\n";
    out << "LOG_LEVEL=debug\n";
}   // ← out 在这里被销毁，文件自动关闭
```

这是一个**局部作用域**。`out` 对象在 `}` 处析构，自动关闭文件。确保在调用 `AppConfig::load(path)` 之前文件已经写完且关闭。这就是 RAII——不用手动 `close()`。

### 7.3 为什么 write 的 `HOST` 是大写，但 Config 里的字段是小写

回顾 `config.cpp` 的 `AppConfig::load()`：

```cpp
line.find("APP_HOST")
```

读的时候找的是大写 `APP_HOST`，存入的是小写字段 `host`。测试也相应地：写大写、断小写。

### 7.4 `std::remove(path.c_str())` 为啥要 `.c_str()`

`std::remove()` 是 C 标准库函数，参数是 `const char*`，不是 `std::string`。`.c_str()` 把 C++ string 转成 C 字符串。

---

## 8. 测试 5：验证 create_app_router() 里确实注册了 /health

```cpp
void test_application_router_registers_health_route() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "GET";
    request.path = "/health";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find(R"("version":"0.1.0")") != std::string::npos);
}
```

**和测试 2 的区别**：

| | 测试 2 | 测试 5 |
|---|---|---|
| Router 哪来的 | 手动创建 + 手动注册 | 调 `create_app_router()` |
| 测什么 | Router 的路由分发功能 | application 模块是否**真的注册了** `/health` |
| 测试类型 | 单元测试 | **集成测试**（跨了 application + router 两个模块） |

断言里用 `"version":"0.1.0"` 作为特征字符串——只要 `create_app_router()` 里 `/health` 的返回内容包含版本号，就算通过。

---

## 9. 测试 6：验证 POST /api/v1/chat 正常返回

```cpp
void test_chat_returns_reply_from_message() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"message":"你好"})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 200);
    assert(response.body.find(R"("code":"OK")") != std::string::npos);
    assert(response.body.find("我收到了：你好") != std::string::npos);
}
```

### 9.1 逐行讲解

```cpp
request.method = "POST";
request.path = "/api/v1/chat";
request.body = R"({"message":"你好"})";
```

前面几个测试都没填 `body`（GET 请求不需要 body）。这个测试**第一次填了 `request.body`**——因为 POST `/api/v1/chat` 的 Handler 要从 body 里提取 message。

```cpp
HttpResponse response = router.route(request);
```

Router 内部流程：

```text
routes_ 里找 key = "POST /api/v1/chat"
    ↓ 找到了
执行 lambda:
  extract_message(request.body)  →  "你好"
  message 不为空 → 走成功分支
  return HttpResponse::json(200, ...)
```

```cpp
assert(response.status_code == 200);
assert(response.body.find(R"("code":"OK")") != std::string::npos);
assert(response.body.find("我收到了：你好") != std::string::npos);
```

验证三点：状态码 200、code 字段是 `"OK"`、reply 里包含 `"你好"`。

**这个测试保证了**：正常发送 `{"message":"你好"}` → 返回 200 + 正确的回复内容。

---

## 10. 测试 7：验证 message 缺失时返回 400

```cpp
void test_chat_returns_400_when_message_missing() {
    Router router = create_app_router();

    HttpRequest request;
    request.method = "POST";
    request.path = "/api/v1/chat";
    request.body = R"({"text":"你好"})";

    HttpResponse response = router.route(request);

    assert(response.status_code == 400);
    assert(response.body.find("INVALID_REQUEST") != std::string::npos);
}
```

**这是最重要的测试——验证了错误处理的正确性。**

### 10.1 为什么 body 是 `{"text":"你好"}` 而不是 `{"message":"你好"}`

因为这个测试要模拟 **"客户端发来的 JSON 里没有 message 字段"** 的情况。

```cpp
request.body = R"({"text":"你好"})";
// 字段名是 text，不是 message
```

`extract_message()` 在 body 里找 `"message"` → 找不到 → 返回空串 → Handler 判断 `message.empty()` 为 `true` → 返回 400。

### 10.2 数据流追踪

```text
request.body = {"text":"你好"}
       ↓
extract_message(request.body)
       ↓
body.find("message")  →  npos（没找到）
       ↓
return ""   （空串）
       ↓
if (message.empty())  →  true
       ↓
return HttpResponse::json(400, R"({"code":"INVALID_REQUEST",...})")
```

### 10.3 断言了什么

```cpp
assert(response.status_code == 400);
assert(response.body.find("INVALID_REQUEST") != std::string::npos);
```

验证两点：
1. **状态码是 400**——不是 200（不要把错误当成功），不是 500（不是服务器崩了）
2. **body 里包含 `INVALID_REQUEST`**——前端可以根据这个 code 字符串做统一的错误处理

---

## 11. main() 函数：测试运行器

```cpp
int main() {
    test_json_response_has_status_content_type_and_body();
    test_router_returns_health_response();
    test_router_returns_404_for_unknown_route();
    test_config_loads_key_value_file();
    test_application_router_registers_health_route();
    test_chat_returns_reply_from_message();
    test_chat_returns_400_when_message_missing();
}
```

没有 `return 0;`——C++ 的 `main()` 可以不写 return，编译器自动补 `return 0;`。

### 11.1 执行顺序

```text
① → ② → ③ → ④ → ⑤ → ⑥ → ⑦
```

一个接一个，串行执行。任何一个 `assert` 失败 → 程序立刻终止 → 后面的测试不会跑。

**所以看到第 3 个测试挂了，不代表第 4~7 个也有问题**——它们根本没机会跑。

### 11.2 为什么不写成 `return` 形式

某些测试框架的测试函数返回 `bool`：

```cpp
bool test_xxx() {
    if (xxx) return true; else return false;
}
```

这里用的是 `assert` 风格——更粗暴，但也更简洁。在真正的企业项目里会引入 Google Test 或 Catch2，但学习阶段这个写法够了。

---

## 12. 7 个测试的分类

```text
┌─────────────────────────────────────────────┐
│              单元测试（只测一个模块）           │
├─────────────────────────────────────────────┤
│ ① test_json_response_...  → 测 http.cpp     │
│ ② test_router_returns_... → 测 router.cpp   │
│ ③ test_router_returns_404 → 测 router.cpp   │
│ ④ test_config_loads_...   → 测 config.cpp   │
├─────────────────────────────────────────────┤
│           集成测试（跨多个模块）                │
├─────────────────────────────────────────────┤
│ ⑤ test_application_router → application +   │
│                              router          │
│ ⑥ test_chat_returns_reply → application +   │
│                              router + http   │
│ ⑦ test_chat_returns_400   → application +   │
│                              router + http   │
└─────────────────────────────────────────────┘
```

---

## 13. 这个测试文件的 C++ 新手应该注意什么

```text
1. assert() 不是异常——它是"条件为假就立刻死"的宏
2. test 函数不需要返回值——跑完没崩就是通过
3. 使用临时文件后要删除——std::remove(path.c_str())
4. {} 局部作用域——控制对象生命周期（RAII）
5. c_str() → std::string 转 C 字符串，因为 std::remove() 是 C 函数
6. find() != npos → 判断字符串包含，比 == 更灵活
7. 测试不是程序功能——它不参与最终产品编译，只在开发阶段跑
```

---

## 14. 和前面的讲解文档对应关系

| 测试编号 | 测的是 | 对应讲解文档 |
|---|---|---|
| ① | http 模块 | [[04_http模块逐行讲解]] |
| ② ③ | router 模块 | [[03_router路由模块逐行讲解]] |
| ④ | config 模块 | [[02_config配置模块逐行讲解]] |
| ⑤ ⑥ ⑦ | application 模块 | [[06_application模块逐行讲解]] |
