## 你现在做的不是 AI 逻辑本身，而是给后面的 RAG、聊天、文档上传、模型调用先修一条路：

```
浏览器发请求
 -> C++ 服务监听端口
 -> 收到 HTTP 文本
 -> 解析成 HttpRequest
 -> Router 找到对应处理函数
 -> Handler 生成 HttpResponse
 -> 序列化成 HTTP 响应
 -> socket 发回浏览器
```

| 编号 | 文件 | 一句话 |
|------|------|--------|
| `02` | config | 读配置文件 → AppConfig 对象 |
| `03` | router | 路由表——注册和分发请求 |
| `04` | http | 翻译官——HTTP 文本 ↔ C++ 对象 |
| `05` | simple_server | 主循环——收请求 → 调翻译 → 调路由 → 回复 |
| `06` | application | 业务登记表——注册所有接口，以后加功能只改这里 |

注意 `01` 那个是项目总览的执行记录，`02~06` 是按代码模块的逐行讲解。建议先读 `02~05` 弄懂基础设施，最后看 `06`——你会发现 application 小得惊人，因为前面四个模块已经把路全铺好了。
## 全链路拆解：
### 第 1 步：浏览器发请求

用户点了按钮 / 输入了 URL。前端 JS 代码执行了：

```js
fetch('http://localhost:8080/health')
```

浏览器内核自动把它拼成一段 HTTP 文本：

```http
GET /health HTTP/1.1
Host: localhost:8080
Accept: */*
Connection: keep-alive

```

这就是一根**字符串**，没有对象、没有结构——纯文本。

### 第 2 步：C++ 服务监听端口

对应 `simple_server.cpp` 第 73-97 行。

C++ 做的事情就像**开了一家店，在门口挂了个"营业中"的牌子**：

```cpp
// 第 73 行：创建 socket — 相当于买一部电话机
socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

// 第 89 行：bind — 把电话号码绑定到这个电话机上（127.0.0.1:8080）
bind(server_socket, &address, ...);

// 第 94 行：listen — 开始"接听模式"，等人打进来
listen(server_socket, ...);

// 第 103 行：进入死循环，"一直守着电话"
while (true) { ... }
```

做完这三步，服务就处于**等待状态**了——不干活，就在那等着。

### 第 3 步：收到 HTTP 文本

对应 `simple_server.cpp` 第 110-123 行。

```cpp
// 第 110 行：accept — 电话响了，接起来。
//     "谁的来电"信息存到 client_address 里。
client_socket = accept(server_socket, &client_address, &client_length);

// 第 117 行：准备一个 8192 字节的空箱子
array<char, 8192> buffer{};

// 第 119 行：recv — 从电话那头读对方说的话，
//     存进 buffer 这个箱子。received 是实际读到了多少字节。
received = recv(client_socket, buffer.data(), buffer.size() - 1, 0);
```

这时候服务器socket的 `buffer` 里就是那个 HTTP 文本字符串：

```
GET /health HTTP/1.1\r\nHost: localhost:8080\r\nAccept: */*\r\n\r\n
```

### 第 4 步：解析成 HttpRequest

对应 `simple_server.cpp` 第 125 行调用 `parse_http_request()`，实现在 `http.cpp` 第 46-78 行。

**这一步就是把那根大字符串拆成有结构的数据**：

```cpp
// http.cpp 第 46-78 行，简化版逻辑：
HttpRequest parse_http_request(const string& raw) {
    HttpRequest request;

    // 第 50 行：读第一行的前三个词
    // "GET /health HTTP/1.1" → method="GET", path="/health", version="HTTP/1.1"
    input >> request.method >> request.path >> request.version;

    // 第 54-71 行：逐行读 Header
    // "Host: localhost:8080" → headers["Host"] = "localhost:8080"
    // "Accept: */*"         → headers["Accept"] = "*/*"
    while (读到的行不是空行) {
        找到冒号位置;
        冒号左边 = header的名字;
        冒号右边 = header的值;
        request.headers[name] = value;
    }

    // 第 73-75 行：剩下的全部是 Body
    request.body = 读剩余内容;

    return request;
}
```

**输入**一根大字符串，**输出**一个结构化的 `HttpRequest` 对象：

```
输入： "GET /health HTTP/1.1\r\nHost: ...\r\n\r\n"
输出： HttpRequest {
         method: "GET",
         path: "/health",
         version: "HTTP/1.1",
         headers: {"Host": "localhost:8080", "Accept": "*/*"},
         body: ""
       }
```

### 第 5 步：Router 路由分发

对应 `simple_server.cpp` 第 128 行 → `router.cpp` 第 13-19 行。

```cpp
// router.cpp 第 13 行：
HttpResponse Router::route(const HttpRequest& request) const {
    // 第 14 行：拼出 key = "GET /health"
    // 第 14 行：在 routes_ 这个 map 里查找 "GET /health"
    const auto found = routes_.find(key(request.method, request.path));
    //         key("GET", "/health") → "GET /health"

    // 第 15-16 行：没找到 → 返回 404
    if (found == routes_.end()) {
        return HttpResponse::json(404, R"({"error":"route_not_found"})");
    }

    // 第 18 行：找到了！found->second 就是 Handler 函数。
    //     把 request 传进去，拿到 response。
    return found->second(request);
    //                        ↑
    //          这个 request 就是第 4 步解析出来的 HttpRequest
}
```

**本质上就是一次 map 查找**，跟你查字典一样：拿 `"GET /health"` 翻目录，翻到了就把活交给对应的处理人。
### 第 6 步：Handler 生成 HttpResponse

对应 `application.cpp` 第 10-14 行。

Handler 就是这个 lambda：

```cpp
// application.cpp 第 10-14 行
router.get("/health", [](const HttpRequest&) {       // ← 这个 lambda 就是 Handler
    return HttpResponse::json(                       // ← 调用静态工厂方法
        200,
        R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
});
```

`HttpResponse::json()` 在 `http.cpp` 第 24-32 行：

```cpp
HttpResponse HttpResponse::json(int status_code_value, string response_body) {
    HttpResponse response;
    response.status_code = 200;                               // 状态码
    response.reason = "OK";                                   // 状态文本
    response.body = R"({"status":"ok",...})";                  // JSON 正文
    response.headers["Content-Type"] = "application/json; charset=utf-8";  // 告诉浏览器：这是 JSON
    response.headers["Connection"] = "close";                 // 告诉浏览器：我回完就关连接
    return response;
}
```

所以 Handler 的输出就是一个填好了所有字段的 `HttpResponse` 对象：

```
HttpResponse {
  status_code: 200,
  reason: "OK",
  headers: {"Content-Type": "application/json", "Connection": "close"},
  body: "{\"status\":\"ok\",\"service\":\"cpp-ai-copilot\",\"version\":\"0.1.0\"}"
}
```

---
### 第 7 步：序列化成 HTTP 响应

对应 `simple_server.cpp` 第 134 行 → `http.cpp` 第 34-44 行。

**第 6 步的输出是 C++ 对象，浏览器不认识。必须转回纯文本字符串。**

```cpp
// http.cpp 第 34-44 行：
string HttpResponse::serialize() const {
    ostringstream output;  // 一个字符串拼接器

    // 第 36 行：写状态行 "HTTP/1.1 200 OK\r\n"
    output << "HTTP/1.1 " << status_code << ' ' << reason << "\r\n";

    // 第 37-39 行：逐行写 Header
    //   "Content-Type: application/json; charset=utf-8\r\n"
    //   "Connection: close\r\n"
    for (const auto& [name, value] : headers) {
        output << name << ": " << value << "\r\n";
    }

    // 第 40 行：加上 Content-Length
    //   "Content-Length: 55\r\n"
    output << "Content-Length: " << body.size() << "\r\n";

    // 第 41 行：空行 — Header 和 Body 之间的分隔线
    output << "\r\n";

    // 第 42 行：Body
    //   {"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
    output << body;

    return output.str();  // 全部拼成一根字符串返回
}
```

**输入** HttpResponse 对象，**输出**一根 HTTP 文本：

```
HTTP/1.1 200 OK\r\n
Content-Type: application/json; charset=utf-8\r\n
Connection: close\r\n
Content-Length: 55\r\n
\r\n
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

---
### 第 8 步：socket 发回浏览器

对应 `simple_server.cpp` 第 135-136 行：

```cpp
// 第 134 行：序列化（第 7 步）
const string serialized = response.serialize();

// 第 135 行：send — 把字符串通过 socket 写回去
//     相当于对着电话说完了回复，然后挂断
send(client_socket, serialized.data(), serialized.size(), 0);

// 第 136 行：挂断这次通话
close_socket(client_socket);
```

浏览器收到这个 HTTP 文本后，自己的内核解析它，取出 JSON，交给前端 JS。JS 代码拿到 `{"status":"ok",...}`，就能更新页面了。

---
## 整条链路一张图总结

```
原始 HTTP 文本                 C++ 对象                    C++ 对象                 原始 HTTP 文本
"GET /health ..."   解析→    HttpRequest    路由→Handler→   HttpResponse   序列化→   "HTTP/1.1 200 OK..."
      ↑                      {method,path,     │            {status,body,              │
      │                       headers,body}    │             headers}                  │
      │                                        │                                       │
   recv()                              router_.route()                          send()
   (收)                                (查表+调函数)                            (发)
      ↑                                        │                                       │
   socket  ←────────── 网络 ──────────→  浏览器  ←────────── 网络 ──────────→  socket
```

**核心就两个转换**：

- **解析（parse）**：字符串 → 对象（第 4 步）
- **序列化（serialize）**：对象 → 字符串（第 7 步）

中间的 Router + Handler（第 5-6 步）就是拿着对象干活——查表、执行函数、生成结果——全程操作的都是结构化对象，不再碰字符串。


#### **HTTP**（HyperText Transfer Protocol，超文本传输协议）就是**浏览器和后端之间"对话的格式"**。

>正是因为有 HTTP 这个**统一的、纯文本的协议**，浏览器（前端）和 C++（后端）才能互相理解——不管它们用什么语言写的，只要都按 HTTP 的格式说话就行。

一个完整的 HTTP 交互长什么样

**请求**（浏览器发过来的原始文本）：

```http
GET /health HTTP/1.1
Host: localhost:8080
Accept: application/json
```

就三部分：
1. **请求行**：`GET /health HTTP/1.1` — 方法 + 路径 + 协议版本
2. **Header（头部）**：`Host: localhost:8080` — 键值对，描述请求的元信息
3. **Body（正文）**：GET 请求通常没有，POST 请求才会有（JSON 数据放这里）

**响应**（你的 C++ 服务回过去）：

```http
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 55

{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

也是三部分：
1. **状态行**：`HTTP/1.1 200 OK` — 版本 + 状态码 + 状态描述
2. **Header**：`Content-Type: application/json` — 告诉浏览器返回的是什么格式
3. **Body**：空行后面就是真正的 JSON 内容
用户只决定"去哪"（URL），**浏览器内核**自动补齐网络传输必需的字段，**前端 JS 代码**决定业务相关的请求行、Header 和 Body。你的 C++ 后端解析 HTTP 时，关心的主要是 JS 代码写的那部分。

#### Router查表的路由器

Router 负责**路由**（分发），Handler 负责**处理**（干活）。分开之后，加新接口只需要在 `application.cpp` 里多写一行 `router.get("/xxx", handler)`，不用改任何路由逻辑。

所以application里面存的是在router里面注册的handler函数

`router.cpp` 的核心就是一个 **map 查找**：
```cpp
// router.cpp 第 13-18 行
HttpResponse Router::route(const HttpRequest& request) const {
    const auto found = routes_.find(key(request.method, request.path));
    if (found == routes_.end()) {
        return HttpResponse::json(404, R"({"error":"route_not_found"})");
    }
    return found->second(request);  // ← 找到 Handler，调用它
}
```

它内部存了一张表：
```
"GET /health"    →  Handler A
"POST /upload"   →  Handler B
"GET /chat"      →  Handler C
```

**Router 只做一件事**：拿 `method + path` 查表，找到了就调用对应的 Handler，找不到就返回 404。

#### Handler：干活的函数

Handler 就是**注册在 Router 表里的那个函数**。看 `application.cpp`：

```cpp
// application.cpp 第 10-14 行
router.get("/health", [](const HttpRequest&) {
    return HttpResponse::json(
        200,
        R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})");
});
```

这里的 `[](const HttpRequest&) { ... }` 这个 lambda 就是 **Handler**。

它的签名是：**收一个 `HttpRequest`，返回一个 `HttpResponse`**。里面写的就是业务逻辑。
#### application：**唯一需要你写业务的地方**——以后加新接口，只改这一个文件就行
`main.cpp` 第 17 行把整条链串起来了：

```cpp
copilot::SimpleHttpServer server(config, copilot::create_app_router(), logger);
```

`create_app_router()` 就是 `application.cpp` 里的那个函数，它做的事情就是：

```cpp
Router create_app_router() {
    Router router;                              // 建一个空表
    router.get("/health", [](...) { ... });     // 往表里注册 Handler
    return router;                              // 把表交出去
}
```

然后把这张表传给 `SimpleHttpServer`，server 存起来当 `router_` 成员，每次来请求就调 `router_.route(request)` 去查表。

所以你的理解完全准确：

```
application.cpp  →  往 Router 里注册 Handler  →   Router 存着这张表  →  server 用它分发请求
```


这就是 Web 后端最小闭环。
## **整体思路**

你可以把这个后端想成一家刚开张的公司：

```
main.cpp 公司大门，负责启动公司 
config.cpp 行政，读取公司地址和端口 
simple_server.cpp 前台，接电话、收请求、回消息 
http.cpp 翻译，把 HTTP 文本翻成对象，也把对象翻回 HTTP 文本 
router.cpp 分诊台，看请求该交给谁处理 
application.cpp 业务登记表，注册有哪些接口 
logger.cpp 记录员，记下谁来了、处理多久、有没有出错`
```
现在公司只开了一个窗口：

`GET /health`

它的作用是告诉外界：

`服务活着，可以接请求。`

返回：

```json
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```
**为什么先做这个**

后面你的 AI 项目会有很多接口：

```
POST /api/v1/documents/upload 
POST /api/v1/chat/stream 
POST /api/v1/tools/reimbursement/status 
GET /api/v1/conversations/{id}/messages`
```

但所有接口的底层动作都一样：

`接请求 -> 找路由 -> 执行业务 -> 返回响应`

所以第一步先把这条链路跑通。AI 应用不是飘在天上的，它最终还是后端服务。


## 代码怎么串起来
入口在 main.cpp (line 10)。

这里做三件事：
```cpp
AppConfig config = AppConfig::load(config_path); 
Logger logger(parse_log_level(config.log_level)); 
SimpleHttpServer server(config, create_app_router(), logger);
```

意思是：
`读取配置 -> 创建日志 -> 创建路由 -> 启动 HTTP Server`

配置在 config.cpp (line 20)。

它读取这种文件：
```
APP_HOST=127.0.0.1 
APP_PORT=18080 
LOG_LEVEL=info
```

为什么配置不能写死？因为以后本地、测试、线上端口和数据库地址都不一样。

路由注册在 application.cpp (line 7)。

```cpp
router.get("/health", [](const HttpRequest&) { 
	return HttpResponse::json( 
		200, 
		R"({"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"})"); 
});
```

这段就是告诉系统：

`如果有人 GET /health，就返回一段 JSON。`

Router 在 router.hpp (line 13) 和 router.cpp (line 13)。

它内部用：
```cpp
std::map<std::string, Handler> routes_;
```

key 是：
`GET /health`

value 是对应的处理函数。  
所以 Router 像一个分诊台：

```
GET /health -> health handler 
POST /chat -> chat handler 
POST /upload -> upload handler
```

HTTP 处理在 http.cpp (line 46)。

`parse_http_request` 做的是把浏览器发来的原始文本：
```http
GET /health HTTP/1.1 
Host: 127.0.0.1:18080`
```

拆成：

```text
method = GET 
path = /health 
headers = ... 
body = ...
```

`HttpResponse::serialize` 在 http.cpp (line 34)，它反过来把对象拼成浏览器能懂的 HTTP 响应：

```http
HTTP/1.1 200 OK 
Content-Type: application/json; charset=utf-8 
Content-Length: ... 

{"status":"ok"}
```

真正监听端口的是 `simple_server.cpp (line 70)`。

主流程是：
```text
socket() 
 -> bind() 
 -> listen() 
 -> accept() 
 -> recv() 
 -> parse_http_request() 
 -> router.route() 
 -> response.serialize() 
 -> send() 
 -> close_socket()
```

这就是你网络八股里的东西真的落到代码里了：

```
socket 是网络连接入口 
bind 是绑定 IP 和端口 
listen 是开始营业 
accept 是接待一个客户端 
recv 是收请求 
send 是发响应
```

**测试怎么保证它没乱**

测试在 `test_core.cpp (line 13)`。

它测了几件事：

```
JSON 响应格式对不对 
/health 路由能不能返回 200 
不存在的路由是不是 404 
配置文件能不能读出来 
application 是否真的注册了 /health
```

所以我们不是只“写了代码”，而是验证了最小后端骨架能工作。

**这一阶段你要记住的面试说法**

你可以这样讲：

```text
我先实现了一个 C++ Web 后端最小骨架，核心链路是 socket 监听端口，收到 HTTP 请求后解析成 HttpRequest，再通过 Router 根据 method 和 path 分发到对应 Handler，Handler 返回统一的 HttpResponse，最后序列化成 HTTP 响应写回客户端。这个阶段先实现了 /health 健康检查、配置读取、日志和单元测试，为后续文档上传、SSE 流式聊天、RAG 检索和模型网关打基础。
```

这部分的意义很朴素：你先把“服务能接客”做出来了。后面 AI 的东西，都是在这个服务上继续加窗口。


## 代码讲解

你从 main 函数看这个项目，先把它理解成一条流水线：

`配置文件 -> 日志器 -> 路由表 -> HTTP服务器 -> 接收请求 -> 分发路由 -> 返回响应`