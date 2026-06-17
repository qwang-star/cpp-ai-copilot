# C++ 企业 AI Copilot 中已经用到的设计模式

这份文档不是为了硬套设计模式，而是把当前项目里已经自然出现的设计思想整理出来。

当前项目虽然还处在 toy HTTP Server 阶段，但已经出现了很多真实后端常见的设计模式和设计原则。

## 1. 工厂方法：`HttpResponse::json`

对应代码：

```cpp
HttpResponse response = HttpResponse::json(200, body);
```

它解决的问题是：

```text
创建 JSON 响应时，不想每次都手动填 status_code、reason、headers、body。
```

如果没有工厂方法，每个接口都要写：

```cpp
HttpResponse response;
response.status_code = 200;
response.reason = "OK";
response.headers["Content-Type"] = "application/json; charset=utf-8";
response.headers["Connection"] = "close";
response.body = body;
```

现在统一变成：

```cpp
HttpResponse::json(200, body)
```

这就是工厂方法的思想：

```text
把对象创建细节封装到一个函数里。
```

面试表达：

```text
项目里 HttpResponse::json 使用了工厂方法思想。调用方不需要知道一个 JSON 响应要设置哪些 header，只需要传状态码和 body，工厂函数负责创建完整响应对象。
```

## 2. 回调 / Handler：路由处理函数

对应代码：

```cpp
using Handler = std::function<HttpResponse(const HttpRequest&)>;
```

以及：

```cpp
router.post("/api/v1/chat", [](const HttpRequest& request) {
    return handle_chat_request(request);
});
```

`Handler` 的意思是：

```text
只要一个东西能接收 HttpRequest，并返回 HttpResponse，它就可以成为路由处理函数。
```

它可以是：

```text
普通函数
lambda
函数对象
```

这就是回调思想：

```text
Router 不关心具体业务怎么写。
Router 只保存一组可以被调用的 Handler。
请求来了，再调用对应 Handler。
```

面试表达：

```text
Router 使用 std::function 保存 Handler，本质上是回调机制。Router 只负责分发，不负责业务，具体业务通过 Handler 注入。
```

## 3. 路由表模式：`std::map<std::string, Handler>`

对应代码：

```cpp
std::map<std::string, Handler> routes_;
```

路由 key 类似：

```text
GET /health
POST /api/v1/chat
```

value 是对应 Handler。

请求来了以后：

```cpp
routes_.find(key(request.method, request.path))
```

这就是路由表模式：

```text
把路径和处理函数做成一张表。
请求来了，查表找到处理逻辑。
```

如果不用路由表，可能会写成：

```cpp
if (method == "GET" && path == "/health") ...
else if (method == "POST" && path == "/api/v1/chat") ...
else if ...
```

接口多了会很乱。

面试表达：

```text
我用 method + path 作为 key，把 Handler 放进 map，形成一个最小路由表。这样新增接口只需要注册路由，不需要改一长串 if/else。
```

## 4. 委托：`application.cpp` 委托给 `chat_service`

对应代码：

```cpp
router.post("/api/v1/chat", [](const HttpRequest& request) {
    return handle_chat_request(request);
});
```

这句的意思是：

```text
application.cpp 不自己处理 chat 业务。
它把请求委托给 chat_service.cpp。
```

这是一种很重要的工程思想：

```text
入口层不要堆业务。
业务交给专门模块。
```

面试表达：

```text
我把 application 设计成路由装配层，只负责把路径和业务函数连接起来。真正的聊天业务委托给 chat_service，这样路由层和业务层解耦。
```

## 5. 单一职责原则：每个模块只做一类事

当前模块职责：

| 模块 | 单一职责 |
|---|---|
| `http.cpp` | HTTP 请求解析和响应序列化 |
| `router.cpp` | 路由分发 |
| `application.cpp` | 路由装配 |
| `chat_service.cpp` | 聊天业务 |
| `api_response.cpp` | 统一响应 |
| `error_code.cpp` | 错误码转换 |
| `config.cpp` | 配置读取 |
| `logger.cpp` | 日志输出 |

这不是某个 GoF 设计模式，但是真实项目里非常重要。

面试表达：

```text
我在 toy server 阶段就有意识地按职责拆模块。这样后面迁移 Drogon 或接数据库时，不需要把所有代码推倒重来。
```

## 6. 封装：`HttpRequest`、`HttpResponse`、`Router`

`HttpRequest` 把一次请求封装成：

```cpp
struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};
```

`HttpResponse` 把一次响应封装成：

```cpp
struct HttpResponse {
    int status_code;
    std::string reason;
    std::map<std::string, std::string> headers;
    std::string body;
};
```

这样业务代码不用直接处理原始 HTTP 文本。

封装的价值是：

```text
底层字符串细节藏起来。
业务层只面对结构化对象。
```

面试表达：

```text
我把 HTTP 原始文本解析成 HttpRequest，把返回内容封装成 HttpResponse，这样业务层不用关心 socket 和 HTTP 文本格式。
```

## 7. 适配器雏形：未来 Drogon 迁移

当前业务层是：

```cpp
HttpResponse handle_chat_request(const HttpRequest& request);
```

未来 Drogon 里请求对象会变成：

```text
drogon::HttpRequestPtr
```

这时可以做一层适配：

```text
Drogon Request
  -> 转换成业务层需要的输入
  -> 调用 ChatService
  -> 转换成 Drogon Response
```

这就是适配器思想：

```text
外部框架接口和内部业务接口不一致时，用一层适配连接起来。
```

面试表达：

```text
为了后续迁移 Drogon，我把业务逻辑从 application.cpp 拆出来。这样未来只需要写 Controller 适配 Drogon 请求，业务层可以继续复用。
```

## 8. 策略模式雏形：未来 RAG 和模型调用

当前项目还没有正式策略模式，但未来一定会出现。

比如 RAG 检索可以有多种策略：

```text
纯向量检索
关键词检索
混合检索
Rerank 后排序
```

模型调用也可以有多种策略：

```text
OpenAI 兼容 API
本地 vLLM
备用模型
降级回答
```

未来可以抽象成：

```text
RetrieverStrategy
ModelProviderStrategy
RerankStrategy
```

面试表达：

```text
当前阶段还没有硬套策略模式，但后续 RAG 检索和模型网关会天然需要策略模式，因为不同检索方式和模型供应商需要可替换。
```

## 9. 当前最重要的设计思想

现在最值得讲的不是“用了多少设计模式”，而是：

```text
我先让功能跑通。
然后发现 application.cpp 开始承担太多职责。
于是拆出 chat_service、api_response、error_code。
这个过程体现了单一职责、委托、封装和分层架构。
```

这是比背模式名字更真实的工程表达。

