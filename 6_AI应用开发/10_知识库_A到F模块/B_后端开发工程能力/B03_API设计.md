# B03 API 设计

## 1. API 是什么

### 是什么

API 是系统对外暴露的能力边界。

后端 AI 应用的 API 要支持：

- 普通请求。
- 长任务。
- 流式响应。
- 文件上传。
- 权限控制。
- 任务查询。

## 2. RESTful

### 是什么

RESTful 是以资源为中心的 API 风格。

资源例子：

- knowledge-bases。
- documents。
- conversations。
- messages。
- agents。
- evaluations。

### 示例

```text
POST /api/v1/knowledge-bases
GET  /api/v1/knowledge-bases/{id}
POST /api/v1/documents/upload
GET  /api/v1/documents/{id}/status
```

## 3. 统一响应

### 是什么

统一响应让前端和调用方用一致方式处理结果。

示例：

```json
{
  "code": 0,
  "message": "ok",
  "data": {},
  "trace_id": "trace_123"
}
```

### AI 场景

模型调用失败、向量库超时、文档处理中等都需要统一错误结构。

## 4. 错误码

### 常见错误

```text
40001 参数错误
40101 未登录
40301 无权限
40401 资源不存在
40901 状态冲突
42901 请求过于频繁
50001 系统错误
50201 模型服务错误
50401 模型服务超时
```

### AI 场景

- RAG 没有召回内容：业务码。
- 模型输出格式错误：业务码。
- 用户 token 额度不足：业务码。

## 5. 参数校验

### 是什么

在进入业务逻辑前检查参数是否合法。

校验：

- 必填。
- 类型。
- 长度。
- 枚举。
- 范围。
- 文件大小。
- 文件类型。

### AI 场景

用户输入 prompt 长度要限制，上传文件大小要限制，top_k 不能无限大。

## 6. 幂等设计

### 是什么

同一请求执行多次，最终效果一致。

### 场景

- 文档上传重复提交。
- 支付或扣费。
- 创建任务。
- Tool Calling 执行业务动作。

### 方案

- Idempotency-Key。
- 唯一索引。
- Redis setnx。
- 任务状态机。
- 请求哈希。

### AI 场景

```text
POST /documents/{id}/reindex
```

重复调用不应创建多个相同重建任务。

## 7. 分页

### 是什么

大量数据不能一次返回，需要分页。

方式：

- offset 分页。
- cursor 分页。

### AI 场景

- 会话列表。
- 消息列表。
- 文档列表。
- 调用日志。
- 评测结果。

### 深分页问题

offset 很大时性能差。

优化：

- cursor。
- 根据 id / created_at 续查。

## 8. 文件上传

### 方式

- multipart 上传。
- 分片上传。
- 直传对象存储。

### AI 场景

文档、图片、音频、视频都可能上传。

注意：

- 大小限制。
- 类型校验。
- 病毒扫描。
- 文件 hash 去重。
- 对象存储。
- 异步解析。

## 9. 同步接口

### 适合

- 短文本分类。
- 小任务抽取。
- 简单问答。

### 不适合

- 大文件解析。
- 长报告生成。
- 批量评测。

## 10. 异步任务接口

### 模式

```text
POST /tasks
  -> 返回 task_id

GET /tasks/{task_id}
  -> 查询状态
```

状态：

```text
pending
running
success
failed
canceled
```

### AI 场景

- 文档解析。
- OCR。
- Embedding。
- 离线评测。

## 11. 流式接口

### 是什么

服务端边生成边返回。

### AI 场景

```text
POST /chat/stream
```

返回 SSE：

```text
event: message
data: {"delta":"你好"}

event: done
data: {}
```

## 12. API 版本控制

### 为什么需要

接口变更不能影响旧客户端。

方式：

- URL：`/api/v1/...`
- Header。

### AI 场景

Prompt、模型、返回字段都可能迭代，API 版本要稳定。

## 13. OpenAPI / Swagger

### 是什么

自动生成接口文档和调试页面。

### AI 场景

前后端协作、测试、外部接入都需要清晰 API 文档。

## 14. AI 应用 API 清单

```text
POST /api/v1/chat/completions
POST /api/v1/chat/stream
GET  /api/v1/conversations/{id}/messages
POST /api/v1/knowledge-bases
POST /api/v1/knowledge-bases/{id}/search
POST /api/v1/documents/upload
GET  /api/v1/documents/{id}/status
POST /api/v1/documents/{id}/reindex
POST /api/v1/agents/{id}/runs
GET  /api/v1/runs/{id}
POST /api/v1/evaluations
```

