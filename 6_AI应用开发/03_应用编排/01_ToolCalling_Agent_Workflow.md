# 07 Tool Calling、Agent 与 Workflow

## 1. Tool Calling 是什么

Tool Calling 让模型可以调用后端提供的工具。

核心分工：

```text
模型负责理解意图和生成参数。
后端负责校验参数、鉴权、执行真实操作。
```

例子：

```text
用户：帮我查一下订单 123 的物流。
模型：判断需要调用 query_order_shipping(order_id=123)。
后端：检查用户是否有权限查订单 123，然后调用订单系统。
模型：根据工具返回结果组织自然语言答案。
```

## 2. Tool Schema

一个工具需要定义：

- name：工具名。
- description：什么时候使用。
- parameters：参数 JSON Schema。
- required：必填字段。
- return：返回结构。
- permission：权限要求。
- timeout：超时时间。

示例：

```json
{
  "name": "search_knowledge_base",
  "description": "Search documents in a knowledge base that the user can access.",
  "parameters": {
    "type": "object",
    "properties": {
      "knowledge_base_id": {"type": "string"},
      "query": {"type": "string"},
      "top_k": {"type": "integer"}
    },
    "required": ["knowledge_base_id", "query"],
    "additionalProperties": false
  }
}
```

## 3. Tool Calling 链路

```text
用户输入
  -> 模型判断是否需要工具
  -> 返回 tool_name 和 arguments
  -> 后端解析 arguments
  -> 参数校验
  -> 鉴权
  -> 执行工具
  -> 返回 tool_result
  -> 模型基于结果生成最终回答
```

## 4. 安全原则

必须记住：

```text
模型只能建议调用工具，不能绕过后端权限。
```

防护：

- 工具白名单。
- 参数 schema 校验。
- 用户权限校验。
- 业务规则校验。
- 高风险操作二次确认。
- 工具超时。
- 工具调用日志。
- 结果脱敏。

不要做：

- 让模型直接拼 SQL 执行。
- 让模型持有数据库账号。
- 让模型决定用户权限。
- 让模型直接调用高危操作。

## 5. Agent 是什么

Agent 是能围绕目标自主规划、选择工具、执行多步任务的系统。

结构：

```text
Goal
  -> Plan
  -> Action
  -> Observation
  -> Reflection
  -> Next Action
  -> Final Answer
```

能力：

- 拆任务。
- 选工具。
- 多步执行。
- 根据结果修正计划。
- 记忆上下文。

风险：

- 调用步数失控。
- 成本不可控。
- 工具误用。
- 结果不可复现。
- 调试困难。
- 权限边界复杂。

## 6. Workflow 是什么

Workflow 是预先定义好的流程编排。

例子：

```text
用户输入
  -> 意图分类
  -> 如果是知识问答，走 RAG
  -> 如果是订单查询，走 Tool Calling
  -> 如果置信度低，转人工
  -> 输出结果
```

特点：

- 可控。
- 可测试。
- 可观测。
- 适合生产业务。

## 7. Agent 和 Workflow 的区别

```text
Workflow
  -> 路线提前设计好
  -> 稳定可控
  -> 适合核心业务

Agent
  -> 模型动态决定下一步
  -> 灵活但不可控
  -> 适合探索性任务
```

面试表达：

```text
生产系统里我会优先使用 Workflow，把关键路径固定下来。
对于需要开放探索的部分，再局部引入 Agent，并设置最大步数、工具白名单、预算限制和人工确认。
```

## 8. Agent 工程约束

必须掌握：

- max_steps：最大步数。
- max_tokens：最大 token。
- max_cost：最大成本。
- timeout：总超时。
- allowed_tools：允许工具列表。
- human_confirm：高风险操作人工确认。
- state_store：状态持久化。
- trace_log：每一步日志。

## 9. 适合 Agent 的场景

- 多源资料调研。
- 复杂报表生成。
- 代码仓库问答。
- 运维排障辅助。
- 数据分析助手。

不适合：

- 支付、删除、审批通过等高风险动作。
- 规则非常固定的流程。
- 强一致性核心交易。
- 对可复现性要求极高的任务。

## 10. 高频面试题

### 为什么不能让模型直接查数据库？

回答：

- 模型可能生成错误 SQL。
- 可能越权读取数据。
- 可能造成慢查询或破坏数据。
- 应该通过后端封装好的工具访问，工具内部做鉴权、参数校验、限流和审计。

### Agent 为什么难落地？

回答：

- 自主性带来不确定性。
- 成本、延迟、工具调用次数难控制。
- 调试和复现困难。
- 企业生产场景通常要用 Workflow 限制边界。

