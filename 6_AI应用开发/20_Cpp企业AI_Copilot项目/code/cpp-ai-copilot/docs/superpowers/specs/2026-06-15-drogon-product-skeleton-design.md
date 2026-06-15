# Drogon 产品骨架设计

## 1. 目标

把当前项目从用于理解 HTTP 原理的手写服务器，升级为能够继续承载数据库、缓存、SSE 和 RAG 功能的产品型 C++ Web 后端。

本阶段不会删除或替换手写服务器。手写服务器继续作为教学版保留，Drogon 作为产品版新增，两者能够独立构建和运行。

## 2. 本阶段范围

本阶段只完成产品后端的基础骨架：

1. 获取并构建 Drogon 源码。
2. 新增 Drogon 产品版程序入口。
3. 实现 `GET /health`。
4. 实现统一 JSON 响应格式。
5. 定义第一批统一错误码。
6. 为正常请求、未知路由和服务端异常提供统一响应。
7. 记录请求路径、状态码和耗时。
8. 从独立配置文件读取产品版监听地址、端口和日志配置。
9. 保证现有教学版测试继续通过。

以下内容不在本阶段实现：

- MySQL 和 Redis 客户端连接
- Docker Compose
- SSE 流式接口
- 模型 API 调用
- RAG、文档上传和用户鉴权
- 将现有教学版源码移动到新目录

这些功能将在产品骨架验证通过后分阶段加入。

## 3. 方案选择

### 方案 A：教学版与产品版并存

保留当前手写服务器，新增 Drogon 产品版。公共业务代码以后逐步提取到与 Web 框架无关的模块。

优点：

- 保留底层原理学习成果。
- 产品功能直接建立在成熟框架上。
- 可以清楚展示项目从教学原型到产品后端的演进。

缺点：

- 短期内存在两个程序入口和两套构建目标。

这是本设计采用的方案。

### 方案 B：直接将现有服务器改造成 Drogon

优点是目录表面更简单，缺点是会失去教学版，并且迁移过程中难以比较两种实现。

### 方案 C：继续增强手写服务器

可以继续学习网络编程，但需要自行处理并发、长连接、异常边界和框架级能力，不适合作为产品主线。

## 4. 目录与组件

本阶段新增以下文件：

```text
cpp-ai-copilot/
  config/
    product.json
  include/copilot/product/
    api_response.hpp
    error_code.hpp
  src/product/
    main.cpp
    api_response.cpp
    health_controller.hpp
    health_controller.cpp
  tests/
    test_api_response.cpp
  third_party/
    drogon/
```

现有 `src/main.cpp` 和 `src/simple_server.cpp` 暂不移动，避免目录调整与框架接入同时发生。构建目标使用明确名称区分：

```text
cpp-ai-copilot-legacy
cpp-ai-copilot-product
test_core
test_api_response
```

### 4.1 产品入口

`src/product/main.cpp` 负责：

- 加载 `config/product.json`
- 初始化 Drogon
- 注册全局错误处理和请求日志
- 启动产品服务

业务响应结构不直接写在入口文件中。

### 4.2 统一响应

成功响应采用以下结构：

```json
{
  "code": "OK",
  "message": "success",
  "data": {
    "status": "ok",
    "service": "cpp-ai-copilot",
    "version": "0.2.0"
  }
}
```

失败响应采用以下结构：

```json
{
  "code": "ROUTE_NOT_FOUND",
  "message": "route not found",
  "data": null
}
```

本阶段定义以下错误码：

```text
OK
INVALID_REQUEST
ROUTE_NOT_FOUND
METHOD_NOT_ALLOWED
INTERNAL_ERROR
```

HTTP 状态码仍表达 HTTP 层结果，`code` 表达项目内部稳定错误类型。

### 4.3 健康检查

接口：

```text
GET /health
```

成功状态码为 `200`，返回服务名、版本和状态。健康检查本阶段只表示 Web 服务可响应，不检查未来的 MySQL、Redis 或模型服务。

### 4.4 配置

`config/product.json` 至少包含：

```json
{
  "listeners": [
    {
      "address": "127.0.0.1",
      "port": 18081
    }
  ],
  "custom_config": {
    "service_name": "cpp-ai-copilot",
    "version": "0.2.0"
  },
  "log": {
    "level": "INFO"
  }
}
```

产品版默认使用 `18081`，避免与教学版默认端口 `18080` 冲突。

### 4.5 请求日志

每个请求至少记录：

```text
HTTP 方法
请求路径
HTTP 状态码
处理耗时
```

日志不得记录密码、令牌或完整敏感请求体。虽然本阶段没有这些字段，但从骨架阶段明确该约束。

## 5. 请求流程

```text
客户端
  -> Drogon 接收 HTTP 请求
  -> 请求计时与日志处理
  -> 路由匹配
  -> HealthController
  -> ApiResponse 生成统一 JSON
  -> Drogon 返回 HTTP 响应
  -> 记录状态码和耗时
```

未知路由不进入业务控制器，由统一的 404 处理生成 `ROUTE_NOT_FOUND`。

控制器抛出未处理异常时，由统一异常处理转换为 `500` 和 `INTERNAL_ERROR`，不得把内部异常详情直接返回给客户端。

## 6. 构建策略

CMake 是产品版的主构建系统。Drogon 放入 `third_party/drogon`，由顶层 `CMakeLists.txt` 通过 `add_subdirectory` 接入。

Makefile 暂时保留，继续服务于现有教学版的快速编译和测试。产品版验证稳定后，再决定是否让 Makefile 只作为 CMake 命令的便捷包装。

Drogon 必须从完整、可识别版本的源码构建。下载完成后记录：

- Drogon 版本或提交号
- 下载来源
- 构建工具版本
- 实际启用和关闭的可选组件
- 额外安装的依赖及用途

本阶段优先关闭数据库、Redis、TLS 等尚未使用的可选组件，以减少首次构建变量。后续接入对应功能时再显式启用。

## 7. 测试

### 7.1 单元测试

`test_api_response` 验证：

- 成功响应字段完整。
- 错误响应字段完整。
- 错误码和 HTTP 状态码映射正确。
- `data` 在无数据错误响应中为 JSON `null`。

现有 `test_core` 必须继续通过，证明教学版没有被破坏。

### 7.2 集成验证

启动产品版后验证：

```text
GET /health       -> 200 + OK
GET /missing      -> 404 + ROUTE_NOT_FOUND
POST /health      -> 405 + METHOD_NOT_ALLOWED
```

同时检查日志是否包含方法、路径、状态码和耗时。

## 8. 验收标准

本阶段完成必须同时满足：

1. Drogon 源码完整下载并记录版本。
2. CMake + Ninja 能从干净构建目录生成产品版程序。
3. `cpp-ai-copilot-product` 可以从配置文件启动并监听 `127.0.0.1:18081`。
4. `/health` 返回设计中规定的统一 JSON。
5. 未知路由和错误方法返回统一错误结构。
6. 请求日志包含方法、路径、状态码和耗时。
7. `test_core` 和 `test_api_response` 全部通过。
8. 教学版仍可独立编译运行。
9. 依赖安装和构建过程写入现有过程记录文档。

## 9. 后续顺序

产品骨架验收后，按以下顺序继续：

1. Docker Compose 启动 MySQL 和 Redis。
2. 产品服务启动时完成依赖连接与健康检查。
3. 设计用户、知识库和文档元数据表。
4. 实现文档上传和状态记录。
5. 接入向量库和 Embedding。
6. 实现 RAG 问答和 SSE 流式返回。
