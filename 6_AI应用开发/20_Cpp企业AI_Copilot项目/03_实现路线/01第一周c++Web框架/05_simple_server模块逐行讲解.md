# 05 simple_server 模块逐行讲解

这一篇专门对照下面两个文件看：

```text
code/cpp-ai-copilot/include/copilot/simple_server.hpp
code/cpp-ai-copilot/src/simple_server.cpp
```

你可以把 `simple_server` 模块理解成：

```text
一家店的大门 + 前台。
开门营业（启动监听）→ 来人（accept）→ 听他说（recv）→ 
翻译（parse_http_request）→ 分诊（router.route）→ 
回复（serialize）→ 送客（send → close）→ 记日志
```

---

## 1. simple_server.hpp 全貌

源码：

```cpp
#pragma once

#include "copilot/config.hpp"
#include "copilot/logger.hpp"
#include "copilot/router.hpp"

namespace copilot {

class SimpleHttpServer {
public:
    SimpleHttpServer(AppConfig config, Router router, Logger logger);

    int run();

private:
    AppConfig config_;
    Router router_;
    Logger logger_;
};

}  // namespace copilot
```

### 1.1 它为什么需要包含那三个头文件

| 头文件          | 用到了什么                           |
| ------------ | ------------------------------- |
| `config.hpp` | `AppConfig` — 构造函数参数类型 + 成员变量类型 |
| `logger.hpp` | `Logger` — 构造函数参数类型 + 成员变量类型    |
| `router.hpp` | `Router` — 构造函数参数类型 + 成员变量类型    |

### 1.2 `#pragma once` + 三个 `#include`

把队友全部引进来，服务器才能用它们。

---

## 2. `class SimpleHttpServer` — 服务器类

```cpp
class SimpleHttpServer {
public:
    SimpleHttpServer(AppConfig config, Router router, Logger logger);
    //               └──────────────── 三个"零件" ────────────────┘
    int run();

private:
    AppConfig config_;   // 配置——在哪监听
    Router router_;      // 路由表——怎么分发请求
    Logger logger_;      // 日志器——怎么记录
};
```

### 2.1 为什么是 `class` 而不用 `struct`

`config_`、`router_`、`logger_` 是内部零件，外部不应该直接碰。用 `class` 默认 `private` 更安全。

### 2.2 构造函数

```cpp
SimpleHttpServer(AppConfig config, Router router, Logger logger);
```

创建服务器时，把三样东西塞进去。`main.cpp` 里正是这样：

```cpp
copilot::SimpleHttpServer server(config, copilot::create_app_router(), logger);
```

### 2.3 `int run()` — 启动服务器

```cpp
int run();
```

返回 `int` 表示退出码（0 = 正常退出）。正常情况下 `run()` 永远不会返回——`while(true)` 死循环一直跑。

---

## 3. simple_server.cpp — 最大的文件

这个文件分三层：

```
第一层（匿名 namespace）   —— 平台兼容 + 小工具
第二层（构造函数）          —— 存零件
第三层（run()）            —— 核心循环，全项目的汇合点
```

---

## 4. 第一层：匿名 namespace 里的平台兼容代码

### 4.1 源码结构

```cpp
namespace copilot {
namespace {                    // ← 匿名 namespace，只在本文件可见

#ifdef _WIN32                 // Windows 平台
    using SocketHandle = SOCKET;
    constexpr SocketHandle invalid_socket_handle = INVALID_SOCKET;

    void close_socket(SocketHandle socket) { closesocket(socket); }

    class WinsockSession { ... };   // 初始化/清理 Windows 网络库

#else                         // Linux/macOS 平台
    using SocketHandle = int;
    constexpr SocketHandle invalid_socket_handle = -1;

    void close_socket(SocketHandle socket) { close(socket); }

    class WinsockSession { ... };   // 空壳，什么都不做
#endif

    std::string socket_error_message(const std::string& action) {
        std::ostringstream output;
        output << action << " failed";
        return output.str();
    }

}  // namespace
```

### 4.2 `#ifdef _WIN32` — 平台条件编译

同一个程序，Windows 和 Linux 用的 socket API 不同。用 `#ifdef` 在编译时选择：

```text
Windows 下编译：保留 #ifdef _WIN32 ... #else 之间的代码，删掉 #else ... #endif
Linux 下编译：  保留 #else ... #endif 之间的代码，删掉 #ifdef _WIN32 ... #else
```

### 4.3 `using SocketHandle = SOCKET;` — 统一名字

Windows 上 socket 类型叫 `SOCKET`，Linux 上就是普通的 `int`。用 `using` 起一个统一的别名 `SocketHandle`，后面代码只用这个名字，不用关心平台。

### 4.4 `constexpr` — 编译期常量

```cpp
constexpr SocketHandle invalid_socket_handle = INVALID_SOCKET;  // Windows: -1 的特殊值
constexpr SocketHandle invalid_socket_handle = -1;              // Linux: 也是 -1
```

`constexpr` = 在编译时就能确定值，运行时不改。相当于 C 的 `#define`，但有类型检查。

### 4.5 `WinsockSession` — RAII（资源获取即初始化）

源码：

```cpp
// Windows 版
class WinsockSession {
public:
    WinsockSession() {
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {  // 构造时：初始化 Windows 网络库
            throw std::runtime_error("WSAStartup failed");
        }
    }
    ~WinsockSession() {
        WSACleanup();                                   // 析构时：清理 Windows 网络库
    }
};

// Linux 版
class WinsockSession {
public:
    WinsockSession() = default;   // 构造时：什么都不做
};
```

这是 C++ 的核心惯用法——**RAII**：

> 构造函数获取资源，析构函数释放资源。对象销毁时自动清理，不会忘记。

`run()` 第一行就是 `WinsockSession winsock;`，这个对象活到 `run()` 结束，自动调用析构函数清理。

**Windows 启动网络库三步走：**

```text
类比：开老式电脑前，得先按电源按钮通电。
Windows 用 socket 之前必须手动启动网络库，Linux 不需要——网络功能天生开着。
```

**`WSADATA data{};`**

```text
WSADATA  →  Windows Sockets API DATA（一个结构体）
           装着网络库的版本、厂商信息等

data{}   →  创建一个 WSADATA 对象，{} 表示全部初始化为 0
```

它只是一个"信息盒子"，`WSAStartup` 会往里填东西。你的代码不直接读它，只负责把它传进去。

**`WSAStartup(MAKEWORD(2, 2), &data)`**

```text
WSAStartup  →  Windows Sockets API Startup
              启动 Windows 网络库的函数

参数1: MAKEWORD(2, 2)  →  告诉 Windows："我要 2.2 版本"
参数2: &data           →  把启动结果信息写到这个盒子里

返回值: 0 表示成功，非 0 表示失败
```

**`MAKEWORD(2, 2)`**

```text
MAKEWORD  →  make word（造一个 WORD 值，WORD = 16 位整数）
            MAKEWORD(低字节, 高字节)
            MAKEWORD(2, 2) → 拼出 0x0202 → 表示 "主版本 2，次版本 2"

这是一个宏，把两个数字拼成版本号。
```

**完整流程：**

```text
WSAStartup( MAKEWORD(2, 2) , &data )
    ↑             ↑              ↑
  启动网络库   "我要2.2版本"   把结果写进这个盒子
              ↑         ↑
             主版本2   次版本2

返回值 == 0  →  成功，网卡通电了
返回值 != 0  →  失败，抛异常退出
```

你现阶段只需要知道：`WinsockSession` 对象创建时自动"通电"，销毁时自动"断电"。这三样 Windows 专属的东西不需要记。

### 4.6 `socket_error_message` — 拼错误信息

```cpp
std::string socket_error_message(const std::string& action) {
    std::ostringstream output;
    output << action << " failed";
    return output.str();
}

// socket_error_message("bind")   → "bind failed"
// socket_error_message("listen") → "listen failed"
```

小工具，避免到处重复写 `"xxx failed"`。

---

## 5. 第二层：构造函数 — 存零件

```cpp
SimpleHttpServer::SimpleHttpServer(AppConfig config, Router router, Logger logger)
    : config_(std::move(config))
    , router_(std::move(router))
    , logger_(std::move(logger))
{}
```

### 5.1 初始化列表 `: xxx(...)`

构造函数体 `{}` 是空的。冒号后面的是**初始化列表**——在进入函数体之前就把成员变量初始化好：

```text
: config_(std::move(config))    →  把传进来的 config，移入成员 config_
, router_(std::move(router))    →  把传进来的 router，移入成员 router_
, logger_(std::move(logger))    →  把传进来的 logger，移入成员 logger_
```

`std::move` = "移交所有权"，避免复制。现阶段理解为"塞进去"即可。

---

## 6. 第三层：`run()` — 核心循环（全部汇合点）

项目所有模块最终在这里汇合。拆成四大块看。

---

### 6.1 开店营业（socket → bind → listen）

```cpp
int SimpleHttpServer::run() {
    WinsockSession winsock;                                    // ① 初始化网络
```

**② 创建 socket：**

```cpp
    SocketHandle server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			    //                           ↑       ↑            ↑
			    //                          IPv4    流式传输      TCP 协议
    if (server_socket == invalid_socket_handle) {
        throw std::runtime_error(socket_error_message("socket"));
    }
```

类比：买一部电话机。`AF_INET` = 电话类型（IPv4），`SOCK_STREAM` = 通话方式（持续连接），`IPPROTO_TCP` = 通信协议（TCP）。

如果买失败了，抛异常，程序退出。

**③ 设置端口复用：**

```cpp
    int reuse = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, ...);
```

允许"关机后立即重启同一个端口"，开发时方便调试。可以跳过不深究。

**④ 设置地址和端口：**

```cpp
    sockaddr_in address{};
    address.sin_family = AF_INET;                             // IPv4
    address.sin_port = htons(static_cast<unsigned short>(config_.port));
    //                                                         ↑  config 里的端口，如 8080
    //                  htons: 转成网络字节序（大端）
    inet_pton(AF_INET, config_.host.c_str(), &address.sin_addr);
    //                  ↑ config_.host，如 "127.0.0.1"
```

类比：在电话机上贴标签——"这台电话的号码是 127.0.0.1:8080"。

**⑤ bind — 绑定地址：**

```cpp
    if (bind(server_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        close_socket(server_socket);
        throw std::runtime_error(socket_error_message("bind"));
    }
```

类比：把标签上的号码注册到这部电话机上。失败 = 端口被占用。

**⑥ listen — 开始监听：**

```cpp
    if (listen(server_socket, SOMAXCONN) != 0) {
        close_socket(server_socket);
        throw std::runtime_error(socket_error_message("listen"));
    }
```

类比：拿起话筒，进入"等人打进来"的状态。

**⑦ 打个招呼：**

```cpp
    std::ostringstream startup;
    startup << "cpp-ai-copilot listening on http://" << config_.host << ':' << config_.port;
    logger_.info(startup.str());
```

控制台输出：

```text
[INFO] cpp-ai-copilot listening on http://127.0.0.1:8080
```

---

### 6.2 死循环 — 等人来（accept → recv → 处理 → send → close）

```cpp
    while (true) {
```

`while(true)` = 无限循环，服务器永远不会自己停。

**① accept — 接电话：**

```cpp
        sockaddr_in client_address{};
        SocketHandle client_socket =
            accept(server_socket, reinterpret_cast<sockaddr*>(&client_address), &client_length);
        if (client_socket == invalid_socket_handle) {
            logger_.warn(socket_error_message("accept"));
            continue;   // 接失败了，跳过，继续等下一个
        }
```

类比：电话响了，接起来。`client_socket` 是这次通话的"专线"，`client_address` 记录了对方的信息。接失败了就 `continue`，跳回 `while` 开头继续等。

**② recv — 收数据：**

```cpp
        std::array<char, 8192> buffer{};                          // 一个 8KB 缓冲区
        const auto start = std::chrono::steady_clock::now();      // 开始计时
        const int received = recv(client_socket, buffer.data(),
                                  static_cast<int>(buffer.size() - 1), 0);
        if (received <= 0) {
            close_socket(client_socket);
            continue;   // 对方挂断了或出错，跳过
        }
```

类比：听对方说话，记在 `buffer` 这个 8192 字节的本子上。`received` 是实际记了多少字。`start` 是开始处理的时间戳，最后用于计算处理耗时。

此时 `buffer` 里就是原始的 HTTP 文本。

**③ parse_http_request — 解析：**

```cpp
        HttpRequest request = parse_http_request(
            std::string(buffer.data(), static_cast<std::size_t>(received)));
        //  buffer 里的字节  →  string  →  parse_http_request()  →  HttpRequest 对象
```

把收到的原始文本翻译成结构化的请求对象。这是 http 模块的入口。

**④ router_.route — 路由分发 + try-catch 保护：**

```cpp
        HttpResponse response;
        try {
            response = router_.route(request);
        } catch (const std::exception& error) {
            logger_.error(error.what());
            response = HttpResponse::json(500, R"({"error":"internal_server_error"})");
        }
```

`try { ... } catch (...) { ... }` 是异常处理：

| 情况          | 发生了什么                                               |
| ----------- | --------------------------------------------------- |
| `try` 里面正常  | `router_.route(request)` 找到 Handler → 返回 200 + JSON |
| `try` 里面抛异常 | `catch` 捕获 → 记错误日志 → 返回 500 + 错误 JSON               |

这样即使 Handler 内部出 bug，服务器也不会崩溃，而是礼貌地回 500。

**⑤ serialize + send — 回复：**

```cpp
        const std::string serialized = response.serialize();
        //  HttpResponse 对象 → HTTP 文本字符串

        send(client_socket, serialized.data(),
             static_cast<int>(serialized.size()), 0);
        //  对着 client_socket 把整根字符串发回去
```

**⑥ close — 挂断：**

```cpp
        close_socket(client_socket);
```

这次通话结束，挂断。

**⑦ 记日志：**

```cpp
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        //  "现在"减"开始" = 处理耗时，转为毫秒

        std::ostringstream access;
        access << request.method << ' ' << request.path
               << " -> " << response.status_code
               << " in " << elapsed.count() << "ms";
        logger_.info(access.str());
```

控制台输出：

```text
[INFO] GET /health -> 200 in 2ms
```

---

## 7. 整条链路走一遍

一个完整的请求处理——从浏览器到服务器再回去：

```text
浏览器                               SimpleHttpServer::run()
──────                               ───────────────────────
                                     while(true) {
                                         accept()   ← 电话响了，接
                                         recv()     ← 听内容
                                            ↓
浏览器发来：                            buffer = "GET /health HTTP/1.1\r\n..."
"GET /health HTTP/1.1\r\n..."
                                            ↓
                                     parse_http_request()
                                            ↓
                                     HttpRequest {method:"GET", path:"/health", ...}
                                            ↓
                                     router_.route(request)
                                            ↓
                                     查 routes_["GET /health"] → lambda
                                            ↓
                                     lambda 返回 HttpResponse {200, body:{"status":"ok"}}
                                            ↓
                                     response.serialize()
                                            ↓
                                     "HTTP/1.1 200 OK\r\nContent-Type:...\r\n\r\n{...}"
                                            ↓
                                     send()     ← 回复客户

浏览器收到：                               close_socket()  ← 挂断
HTTP/1.1 200 OK                              ↓
Content-Type: application/json           记日志: "GET /health -> 200 in 2ms"
                                         } ← 回到 while(true) 开头，等下一个人
{"status":"ok"}
```

---

## 8. 这里面你暂时不需要深究的

| 内容                                | 为什么可以先跳过                |
| --------------------------------- | ----------------------- |
| `#ifdef _WIN32` 里的 socket 类型转换    | 平台兼容代码，不影响理解流程          |
| `reinterpret_cast`、`static_cast`  | C++ 类型转换，现阶段知道是"转换类型"即可 |
| `sockaddr_in`、`htons`、`inet_pton` | 网络编程 API，属于独立知识领域       |
| `setsockopt`                      | 端口复用，调试用                |

**现阶段重点关注 `while(true)` 循环里的 7 步——那是你之前画的那个流程图在代码里的真实对应。**

---

## 9. 这段代码你最该掌握什么

```text
1. 构造函数初始化列表 : xxx(...) — 把参数存入成员变量
2. while(true) 死循环 — 服务器一直运行不退出
3. try/catch — 出错不崩溃，返回 500
4. chrono::steady_clock — 计时算处理耗时
5. ostringstream 拼日志 — "GET /health -> 200 in 2ms"
6. RAII — WinsockSession 构造时初始化，析构时自动清理
7. recv → parse → route → serialize → send — 整个处理链
8. accept/recv/send/close — socket 四件套的作用（类比打电话）
```

前四个模块全部学完后，整个项目的链路就通了：

```text
config.cpp  →  http.cpp  →  router.cpp  →  application.cpp  →  simple_server.cpp
（配置）       （翻译）      （路由）        （业务注册）          （主循环，汇合所有模块）
```
