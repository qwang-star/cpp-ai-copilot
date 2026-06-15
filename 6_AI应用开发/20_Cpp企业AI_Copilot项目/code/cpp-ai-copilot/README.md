# cpp-ai-copilot

这是 C++ 企业级 AI Copilot 的后端骨架。

当前阶段目标：

```text
先跑通一个零第三方依赖的 C++20 Web 服务骨架。
```

本机当前没有检测到 CMake、vcpkg、Drogon，所以第一版没有直接依赖 Drogon。它先用标准 C++ + Winsock 实现最小 HTTP Server，业务分层按真实后端项目设计，后面可以把底层 HTTP Server 替换成 Drogon。

## 已完成

```text
GET /health
统一 JSON 响应
Router 路由分发
AppConfig 配置读取
Logger 控制台日志
SimpleHttpServer 最小 HTTP 服务
单元测试
```

## 目录结构

```text
include/copilot
  application.hpp      应用路由装配
  config.hpp           配置读取
  http.hpp             HTTP 请求和响应
  logger.hpp           日志
  router.hpp           路由
  simple_server.hpp    最小 HTTP Server

src
  application.cpp
  config.cpp
  http.cpp
  logger.cpp
  main.cpp
  router.cpp
  simple_server.cpp

tests
  test_core.cpp

config
  app.env
```

## 编译和测试

```bash
mingw32-make test
mingw32-make all
```

## 启动

```bash
mingw32-make run
```

默认监听：

```text
http://127.0.0.1:18080
```

健康检查：

```bash
curl http://127.0.0.1:18080/health
```

预期返回：

```json
{"status":"ok","service":"cpp-ai-copilot","version":"0.1.0"}
```

## 为什么先不用 Drogon

第 1 天的目标是先让你看到 C++ Web 后端怎么跑起来：

```text
请求进来
  -> parse_http_request
  -> Router 分发
  -> Handler 处理
  -> HttpResponse 序列化
  -> socket 返回给浏览器
```

这条链路跑通后，再接 Drogon 就不会只是在“用框架”，而是知道框架帮你做了什么。

## 下一步

```text
1. 增加统一错误码。
2. 增加 /api/v1/chat/stream 的 SSE 骨架。
3. 增加 MySQL / Redis 配置字段。
4. 增加 Docker Compose。
5. 如果安装 CMake + Drogon，再迁移到 Drogon 版本。
```

