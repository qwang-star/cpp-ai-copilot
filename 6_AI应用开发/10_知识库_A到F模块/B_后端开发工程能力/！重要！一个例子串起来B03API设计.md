# ！重要！一个例子串起来 B03 API 设计

![B03 API设计流程图](../_images/B03_api_flow.png)

## 场景：设计知识库问答系统的一组接口

系统要支持：

```text
创建知识库
上传文档
查询文档状态
发起问答
查看历史会话
```

这能串起 API 设计。

<!-- BEGIN_EXAMPLE_TERMS -->
## 读之前先把这篇的名词说清楚

这一篇把 API 设计想成系统对外签合同：路径、参数、状态码、错误码都要清楚，不然前端、后端、测试会互相猜。

后面如果你看到这些词，先不要急着背定义。你可以按下面这个顺序理解：

```text
它是什么 -> 在这个例子里负责什么 -> 面试时怎么说
```

### 1. REST

**新手理解**：REST 是把接口围绕资源来设计的一种风格。

**在这个例子里**：知识库、文档、会话、消息都可以当资源设计接口。

**面试说法**：REST 常用 URL 表示资源，用 HTTP 方法表示动作。

### 2. Resource

**新手理解**：Resource 就是系统里可以被管理的东西。

**在这个例子里**：`knowledge-bases`、`documents`、`conversations` 都是资源。

**面试说法**：资源建模决定 API 是否清晰。

### 3. HTTP Method

**新手理解**：Method 是请求动作，比如 GET 查、POST 建、PUT 改、DELETE 删。

**在这个例子里**：提问用 POST，因为它提交内容并产生模型调用记录。

**面试说法**：方法语义要和接口行为匹配。

### 4. 状态码

**新手理解**：状态码是服务端给客户端的第一层结果信号。

**在这个例子里**：没登录是 401，没权限是 403，参数错是 400，服务错是 500。

**面试说法**：状态码用于表达请求处理结果，要避免全都返回 200。

### 5. 幂等

**新手理解**：幂等是同一个请求重复执行，结果不会乱。

**在这个例子里**：重复上传回调、重复消费消息、重复重试创建索引时都要考虑。

**面试说法**：GET/PUT/DELETE 通常应设计为幂等，POST 可通过幂等键兜底。

### 6. 分页

**新手理解**：分页就是列表别一次全返回。

**在这个例子里**：查询文档列表、会话列表时要传 page/page_size 或 cursor。

**面试说法**：分页能保护数据库和网络传输，cursor 更适合大数据量滚动查询。

### 7. 版本号

**新手理解**：版本号是接口演进时的缓冲垫。

**在这个例子里**：`/api/v1/chat/stream` 将来变更时可以新增 v2。

**面试说法**：API 版本管理能减少兼容性风险。

### 8. 错误码

**新手理解**：错误码比一句错误文本更稳定。

**在这个例子里**：`KB_FORBIDDEN` 比“你不能看这个知识库”更适合前端判断。

**面试说法**：业务错误码用于客户端处理和问题排查。

### 9. OpenAPI

**新手理解**：OpenAPI 是接口说明书，可以生成文档和客户端。

**在这个例子里**：后端定义 chat API 后，前端和测试可以按文档联调。

**面试说法**：OpenAPI/Swagger 能降低协作成本。

<!-- END_EXAMPLE_TERMS -->

## 0. 总流程图

```mermaid
flowchart TD
    A[POST /knowledge-bases 创建知识库] --> B[POST /documents/upload 上传文档]
    B --> C[返回 task_id/document_id]
    C --> D[GET /documents/{id}/status 查询状态]
    D --> E{status=ready?}
    E -- 否 --> D
    E -- 是 --> F[POST /chat/stream 发起问答]
    F --> G[SSE 流式返回答案]
    G --> H[GET /conversations/{id}/messages 查看历史]
    F --> I[错误码/trace_id/统一响应]
```

---

## 1. RESTful：把东西抽象成资源

资源：

```text
knowledge-bases
documents
conversations
messages
evaluations
```

接口：

```text
POST /knowledge-bases
GET /knowledge-bases/{id}
POST /documents/upload
GET /documents/{id}/status
```

---

## 2. 统一响应

普通 JSON 接口返回：

```json
{
  "code": 0,
  "message": "ok",
  "data": {},
  "trace_id": "trace_123"
}
```

好处：

```text
前端统一处理
日志可追踪
错误可定位
```

---

## 3. 错误码

具体业务对应具体错误：

```text
40001 参数错误
40101 未登录
40301 无权限
40401 文档不存在
40901 文档还未 ready
42901 请求过多
50401 模型超时
```

---

## 4. 参数校验

上传文档要校验：

```text
文件大小
文件类型
kb_id 是否存在
用户是否有权限
```

聊天接口要校验：

```text
question 非空
question 长度
top_k 范围
conversation_id 是否属于用户
```

---

## 5. 同步接口和异步接口

创建知识库很快：

```text
同步返回
```

文档解析很慢：

```text
异步返回 document_id/task_id
```

不要让用户上传后等 2 分钟。

---

## 6. 异步任务状态

文档状态：

```text
uploaded
parsing
embedding
ready
failed
```

接口：

```text
GET /documents/{id}/status
```

---

## 7. 流式接口

聊天接口用：

```text
POST /chat/stream
```

返回：

```text
event: message
data: {"delta":"根据"}

event: done
data: {}
```

这不是普通 JSON 一次性返回。

---

## 8. 幂等设计

用户重复点击上传，不能创建多个重复任务。

方案：

```text
Idempotency-Key
file_hash
唯一索引
Redis setnx
```

---

## 9. 分页

历史消息很多：

```text
GET /conversations/{id}/messages?cursor=xxx&limit=20
```

比深 offset 更适合大数据量。

---

## 10. API 版本

用：

```text
/api/v1
```

后续返回结构变了，可以开：

```text
/api/v2
```

---

## 11. 面试总结版

```text
我会把知识库、文档、会话、消息都设计成资源。文档上传是长任务，接口返回 document_id，再通过 status 查询；聊天是流式接口，用 SSE 返回 token delta。所有接口有统一响应、错误码、参数校验和 trace_id。重复提交用幂等 key 或文件 hash 防止重复任务。
```

