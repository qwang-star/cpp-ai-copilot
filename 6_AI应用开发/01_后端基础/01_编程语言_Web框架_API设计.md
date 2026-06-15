# 01 编程语言、Web 框架与 API 设计

## 1. 这一章为什么重要

后端 AI 应用开发最终还是要落成接口、服务、任务、数据库表和监控。

面试里你不能只说：

```text
我会调用大模型。
```

你要能说：

```text
我把模型调用封装成 ModelClient，通过 Chat Service 暴露同步和流式接口，通过 MQ 处理文档向量化长任务，通过 MySQL 记录会话和日志，通过 Redis 做限流和缓存。
```

## 2. 推荐语言路线

### Java 路线

适合：

- 国内后端秋招。
- 大厂业务后端。
- 微服务、数据库、中间件面试。

必须掌握：

- Java 集合。
- JVM 内存模型。
- GC。
- 多线程。
- 线程池。
- CompletableFuture。
- Spring Boot。
- Spring MVC。
- Spring IOC / AOP。
- Spring 事务。
- MyBatis。
- 参数校验。
- 全局异常处理。

AI 应用连接点：

- 用 Spring Boot 写聊天、知识库、文档管理接口。
- 用线程池或 MQ 处理文档解析、Embedding。
- 用 WebFlux / SseEmitter / ResponseBodyEmitter 做流式输出。
- 用 RestClient / WebClient / OkHttp 封装模型调用。

### Python 路线

适合：

- AI 原型。
- RAG 快速开发。
- 数据处理。
- 模型生态。

必须掌握：

- Python 基础语法。
- 类型提示。
- async / await。
- FastAPI。
- Pydantic。
- SQLAlchemy。
- Celery / Dramatiq / RQ。
- Uvicorn / Gunicorn。
- 文件处理。

AI 应用连接点：

- FastAPI 写 RAG 服务。
- Pydantic 校验结构化输出。
- Celery 做文档解析任务。
- 直接接 LangChain / LlamaIndex / 向量库 SDK。

### Go 路线

适合：

- 高并发服务。
- 云原生。
- 网关和中间件。

必须掌握：

- goroutine。
- channel。
- context。
- Gin / Echo。
- gRPC。
- 并发控制。

AI 应用连接点：

- 模型网关。
- 高并发 API 服务。
- 流式转发。

## 3. 秋招推荐表达

如果你主攻后端 AI 应用开发，推荐这样包装：

```text
我主要用 Java / Spring Boot 做后端工程，熟悉 MySQL、Redis、MQ 和分布式基础。
AI 应用部分我用 Python / FastAPI 或 Java SDK 实现 RAG、模型调用和文档处理。
我的重点不是训练模型，而是把大模型能力工程化接入业务系统。
```

## 4. API 设计原则

必须掌握：

- RESTful。
- 统一响应结构。
- 错误码。
- 参数校验。
- 幂等。
- 分页。
- 版本控制。
- OpenAPI / Swagger。
- 鉴权。
- 限流。

统一响应示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {},
  "trace_id": "trace_abc"
}
```

错误码设计：

```text
40001 参数错误
40101 未登录
40301 无权限
40401 资源不存在
40901 重复提交
42901 请求过于频繁
50001 系统错误
50201 模型服务异常
50401 模型服务超时
```

## 5. AI 应用典型 API

### 聊天

```text
POST /api/v1/chat/completions
POST /api/v1/chat/stream
GET  /api/v1/conversations/{conversation_id}
GET  /api/v1/conversations/{conversation_id}/messages
DELETE /api/v1/conversations/{conversation_id}
```

### 知识库

```text
POST /api/v1/knowledge-bases
GET  /api/v1/knowledge-bases
GET  /api/v1/knowledge-bases/{kb_id}
POST /api/v1/knowledge-bases/{kb_id}/members
POST /api/v1/knowledge-bases/{kb_id}/search
```

### 文档

```text
POST /api/v1/documents/upload
GET  /api/v1/documents/{document_id}
GET  /api/v1/documents/{document_id}/status
DELETE /api/v1/documents/{document_id}
POST /api/v1/documents/{document_id}/reindex
```

### Agent / Workflow

```text
POST /api/v1/agents/{agent_id}/runs
GET  /api/v1/runs/{run_id}
POST /api/v1/runs/{run_id}/cancel
GET  /api/v1/runs/{run_id}/steps
```

### 评测

```text
POST /api/v1/evaluations
GET  /api/v1/evaluations/{evaluation_id}
GET  /api/v1/evaluations/{evaluation_id}/results
```

## 6. 同步接口、异步接口、流式接口

### 同步接口

适合：

- 简单分类。
- 短文本抽取。
- 快速问答。

特点：

- 实现简单。
- 等待完整结果。
- 超时风险较高。

### 异步接口

适合：

- 文档解析。
- OCR。
- 批量 Embedding。
- 长报告生成。
- 离线评测。

模式：

```text
提交任务
  -> 返回 task_id
  -> 后台执行
  -> 查询任务状态
  -> 获取结果
```

### 流式接口

适合：

- 聊天。
- 长答案生成。
- 代码生成。

模式：

```text
客户端建立连接
  -> 服务端逐 token 返回
  -> 客户端实时渲染
```

## 7. 幂等设计

AI 应用常见幂等场景：

- 用户重复上传同一文档。
- 前端重复提交文档解析任务。
- MQ 重复投递 embedding 任务。
- Tool Calling 重复调用业务接口。

方案：

- idempotency_key。
- document_hash。
- task_id 唯一约束。
- 数据库唯一索引。
- Redis setnx。
- 状态机判断。

例子：

```text
同一个 document_id + chunk_version 的 embedding 任务，只允许成功写入一次。
```

## 8. 分层架构

推荐：

```text
Controller
  -> 参数校验、鉴权上下文、返回响应

Service
  -> 业务流程、事务边界

Domain
  -> 核心对象和状态机

Repository
  -> MySQL / Redis / Vector DB 访问

Client
  -> 模型、OCR、对象存储、外部系统调用

Worker
  -> MQ 消费和后台任务
```

反例：

```text
Controller 里直接拼 Prompt、查数据库、调模型、写日志。
```

问题：

- 难测试。
- 难复用。
- 难定位问题。
- 难做降级和监控。

