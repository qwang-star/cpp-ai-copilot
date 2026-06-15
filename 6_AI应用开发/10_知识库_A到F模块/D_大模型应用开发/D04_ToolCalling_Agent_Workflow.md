# D04 Tool Calling、Agent、Workflow

## 1. Tool Calling 是什么

### 是什么

Tool Calling 让模型根据用户意图选择后端工具，并生成调用参数。

核心分工：

```text
模型：理解意图，生成参数
后端：校验参数，鉴权，执行工具
```

## 2. 工具 Schema

### 是什么

工具 Schema 描述工具名称、用途、参数结构。

示例：

```json
{
  "name": "query_order",
  "description": "查询当前用户有权限访问的订单",
  "parameters": {
    "type": "object",
    "properties": {
      "order_id": {"type": "string"}
    },
    "required": ["order_id"]
  }
}
```

## 3. Tool Calling 链路

```text
用户输入
  -> 模型判断需要工具
  -> 生成 tool_name + arguments
  -> 后端 JSON 校验
  -> 后端鉴权
  -> 执行业务工具
  -> 返回 tool_result
  -> 模型生成最终回答
```

## 4. 工具参数校验

### 为什么重要

模型生成的参数不可信。

校验：

- JSON 是否合法。
- 必填字段。
- 类型。
- 枚举。
- 长度。
- 资源是否存在。

## 5. 工具鉴权

### 原则

```text
模型不能决定用户有没有权限。
```

后端必须检查：

- 当前用户。
- 当前租户。
- 工具权限。
- 资源权限。
- 操作风险。

## 6. 为什么不能让模型直接写 SQL

原因：

- 可能 SQL 错误。
- 可能越权。
- 可能慢查询。
- 可能修改数据。
- 可能被注入。

正确：

```text
封装安全工具，工具内部执行受控查询。
```

## 7. Agent 是什么

### 是什么

Agent 是能围绕目标进行规划、调用工具、观察结果并继续行动的系统。

基本循环：

```text
Goal
  -> Plan
  -> Action
  -> Observation
  -> Next Action
  -> Final
```

## 8. Agent 核心能力

- Planning：规划。
- Tool Use：工具调用。
- Memory：记忆。
- Reflection：反思。
- Observation：观察。
- Termination：终止。

## 9. Agent 风险

- 步数失控。
- 成本失控。
- 延迟不可控。
- 工具误用。
- 不可复现。
- 调试困难。
- 权限边界复杂。

## 10. Agent 工程约束

必须设置：

- max_steps。
- max_tokens。
- max_cost。
- timeout。
- allowed_tools。
- human_confirm。
- trace_log。

## 11. Workflow 是什么

### 是什么

Workflow 是预定义流程编排。

例子：

```text
输入
  -> 意图分类
  -> RAG
  -> Tool Calling
  -> 校验
  -> 输出
```

## 12. Workflow 优点

- 可控。
- 可测试。
- 可观测。
- 适合生产。

## 13. Agent vs Workflow

```text
Agent：灵活，自主，但不稳定
Workflow：固定，可控，但灵活性低
```

生产建议：

```text
关键业务用 Workflow，局部复杂任务引入 Agent。
```

## 14. Human-in-the-loop

### 是什么

高风险操作需要人工确认。

### 场景

- 删除数据。
- 发邮件。
- 退款。
- 审批。
- 执行脚本。

## 15. 面试回答模板

```text
Tool Calling 中模型只负责选择工具和生成参数，后端必须做参数校验、鉴权和业务规则校验。Agent 比普通工具调用更灵活，但成本和行为更难控制，所以生产系统通常用 Workflow 固定关键链路，在局部任务中引入 Agent，并设置最大步数、工具白名单、预算和人工确认。
```

