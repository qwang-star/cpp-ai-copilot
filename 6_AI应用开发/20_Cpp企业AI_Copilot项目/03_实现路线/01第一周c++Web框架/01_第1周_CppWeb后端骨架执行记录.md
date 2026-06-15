# 第 1 周执行记录：C++ Web 后端骨架

## 当前完成内容

已经创建第一版后端代码目录：

```text
6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot
```

当前先做的是零第三方依赖版本：

```text
C++20
Makefile
可选 CMakeLists.txt
Winsock 最小 HTTP Server
Router
HttpRequest / HttpResponse
AppConfig
Logger
GET /health
单元测试
```

为什么先不用 Drogon：

```text
本机当前没有检测到 CMake、vcpkg、Drogon。
所以先写一个能跑通请求链路的最小 C++ Web 骨架。
这样你能先理解 Web 框架底层做了什么：收请求、解析 HTTP、路由分发、序列化响应、socket 返回。
后面装好 Drogon 后，业务层可以迁移过去。
```

## 已验证命令

在目录：

```text
6_AI应用开发/20_Cpp企业AI_Copilot项目/code/cpp-ai-copilot
```

运行：

```bash
mingw32-make test
```

结果：

```text
单元测试通过
```

运行：

```bash
mingw32-make all
```

结果：

```text
服务端可执行文件编译成功：build/cpp-ai-copilot.exe
```

启动后访问：

```text
http://127.0.0.1:18080/health
```

结果：

```json
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

## 当前接口

### GET /health

用途：

```text
健康检查，确认服务已经启动并能处理 HTTP 请求。
```

返回：

```json
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

## 当前代码分层

```text
application
  注册业务路由，比如 /health，后面会注册 /api/v1/chat/stream

config
  从 config/app.env 读取 APP_HOST、APP_PORT、LOG_LEVEL

http
  表示 HttpRequest、HttpResponse，负责响应序列化和请求解析

router
  根据 method + path 找到对应 handler

logger
  输出请求日志和错误日志

simple_server
  负责 socket 监听、接收请求、调用 router、返回响应
```

## 当前启动方式

```bash
mingw32-make run
```

默认端口：

```text
18080
```

说明：

```text
之前测试 8080 时返回 404，换端口验证后发现是 8080 被其他服务占用。
所以当前项目默认用 18080，避免端口冲突。
```

## 下一步

下一步建议做：

```text
1. 增加统一错误码和 ApiResponse。
2. 增加 /api/v1/chat/stream 的 SSE 骨架。
3. 增加 MySQL / Redis 配置字段。
4. 增加 Docker Compose。
5. 再接入真实 MySQL / Redis 客户端。
```

