# B02 Web 框架

## 1. Web 框架是什么

### 是什么

Web 框架帮助开发者处理 HTTP 请求、路由、参数、响应、异常、依赖注入和中间件。

### AI 场景

后端 AI 应用需要暴露：

- 聊天接口。
- 流式接口。
- 文档上传接口。
- 知识库管理接口。
- 任务状态接口。
- 评测接口。

## 2. Spring Boot

### 是什么

Spring Boot 是 Java 生态主流后端框架，用自动配置简化 Spring 应用开发。

### 必会知识点

- 自动配置。
- Starter。
- IOC。
- AOP。
- Spring MVC。
- Bean 生命周期。
- 事务管理。
- 参数校验。
- 全局异常。
- 拦截器。
- 过滤器。

## 3. IOC

### 是什么

IOC 是控制反转，对象创建和依赖管理交给 Spring 容器。

### 为什么重要

让业务代码不直接 new 依赖，而是通过注入组织模块。

### AI 场景

```text
ChatService
  -> RagService
  -> ModelClient
  -> ConversationRepository
```

这些依赖由容器管理，便于替换模型客户端、Mock 测试和配置化。

## 4. AOP

### 是什么

AOP 是面向切面编程，把横切逻辑从业务代码中抽出。

### 常见用途

- 日志。
- 事务。
- 权限。
- 限流。
- 监控。

### AI 场景

模型调用接口可以用 AOP 记录：

- trace_id。
- 耗时。
- token。
- 成本。
- 错误码。

## 5. Spring MVC 请求流程

### 核心流程

```text
请求
  -> DispatcherServlet
  -> HandlerMapping
  -> HandlerAdapter
  -> Controller
  -> Service
  -> 返回 ModelAndView / ResponseBody
  -> 异常处理 / 消息转换
```

### AI 场景

聊天请求进入 Controller 后，通常调用 ChatService，再调 RAG、ModelGateway，最后返回普通 JSON 或 SSE。

## 6. Bean 生命周期

### 是什么

Spring Bean 从创建到销毁的过程。

大致：

```text
实例化
  -> 属性注入
  -> Aware 回调
  -> BeanPostProcessor 前置
  -> 初始化
  -> BeanPostProcessor 后置
  -> 使用
  -> 销毁
```

### AI 场景

模型客户端、连接池、向量库客户端可以在 Bean 初始化时创建，在销毁时关闭连接。

## 7. Spring 事务

### 是什么

Spring 用 `@Transactional` 管理数据库事务。

### AI 场景

- 创建文档记录和任务记录。
- 更新文档状态。
- 保存会话和消息。
- 扣减用户额度。

### 常见坑

- 同类方法内部调用事务不生效。
- 异常被捕获不回滚。
- 非 public 方法事务不生效。
- 长事务包含远程模型调用。

重点：

```text
不要在数据库事务里调用慢模型 API。
```

## 8. FastAPI

### 是什么

FastAPI 是 Python 的现代 Web 框架，基于类型提示和 Pydantic，适合 AI 应用快速开发。

### 必会知识点

- 路由。
- Pydantic Model。
- Depends 依赖注入。
- async endpoint。
- BackgroundTasks。
- StreamingResponse。
- 中间件。
- 异常处理。

## 9. Pydantic

### 是什么

Pydantic 用于数据校验和序列化。

### AI 场景

- 请求参数校验。
- 模型结构化输出校验。
- Tool Calling 参数校验。
- 评测结果格式校验。

## 10. FastAPI 流式输出

### 是什么

FastAPI 可以用 StreamingResponse 返回流式内容。

### AI 场景

模型生成 token 时逐步 yield 给前端。

注意：

- 处理客户端断开。
- 保存完整响应。
- 设置正确 media type。

## 11. Gin

### 是什么

Gin 是 Go 语言常见 Web 框架。

### AI 场景

- 高并发模型网关。
- API 服务。
- 流式转发。

## 12. 框架选型

```text
企业后端主系统：Spring Boot
AI 原型和 RAG 服务：FastAPI
高并发网关：Go / Gin
```

面试表达：

```text
我会根据团队技术栈选择框架。如果是国内后端系统，Spring Boot 更贴合业务和微服务生态；如果是快速验证 AI Pipeline，FastAPI 和 Python 生态效率更高。
```

如果主语言是 C++，可以补充：

```text
如果项目强调 C++ 技术栈，我会优先考虑 Drogon 这类 C++ Web 框架来承载 API Server。普通 REST 和 SSE 接口用 Web 框架开发效率更高；如果后续实时连接规模很大，可以评估 uWebSockets；如果要写更底层的模型网关或自定义协议，再深入 Boost.Asio。
```

## 13. 前沿升级：Gateway API、Envoy、OpenFeature

Web 框架负责应用内部接口，但生产系统还要考虑入口网关和流量治理。

演进路线：

```text
本地开发：直接启动 Web 服务
单机部署：Nginx 反向代理
Kubernetes：Ingress
更复杂入口治理：Gateway API
云原生 L7 代理：Envoy
AI 策略灰度：OpenFeature / Feature Flag
```

详细对比看：

- [G05_云原生服务治理_GatewayAPI_Envoy_Istio_OpenFeature.md](../G_前沿技术与新趋势/G05_云原生服务治理_GatewayAPI_Envoy_Istio_OpenFeature.md)
