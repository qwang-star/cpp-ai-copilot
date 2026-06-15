# G06 Agent 工程化：Agents SDK、A2A、MCP、Guardrails

这一篇讲 Agent 从 Demo 到工程系统要补的能力。

普通 demo 是：

```text
模型自己想
模型自己调用工具
模型自己循环
最后给一个答案
```

生产系统要问：

```text
谁允许它调用工具？
它最多能调用几步？
工具参数谁校验？
失败怎么恢复？
每一步怎么追踪？
多个 Agent 怎么协作？
输出不安全怎么办？
```

所以 Agent 工程化的关键词是：

```text
编排
工具协议
安全边界
可观测性
多 Agent 协作
人工确认
```

---

## 1. 从 Tool Calling 到 Agent

Tool Calling 是：

```text
模型判断要不要调用一个工具
```

Agent 是：

```text
模型可以规划多步任务，选择工具，观察结果，再继续下一步
```

例子：

```text
用户说：帮我看看这次出差报销还差什么，并查一下我的审批进度。
```

Tool Calling 可能只做：

```text
调用 get_reimbursement_status
```

Agent 可能做：

```text
1. 检索差旅制度
2. OCR / 解析用户上传的发票
3. 对比缺失材料
4. 查询报销单审批进度
5. 生成待办建议
```

Agent 更强，但也更危险。

---

## 2. Agents SDK 思路

OpenAI Agents SDK 这类框架通常强调：

```text
Agent
Tool
Handoff
Guardrails
Tracing
```

新手理解：

```text
Agent 是办事员
Tool 是办事员能用的工具
Handoff 是把任务转交给另一个办事员
Guardrails 是安全护栏
Tracing 是全程录像
```

### 改进了什么

旧做法：

```text
每个项目自己写 Agent 循环
工具调用日志不统一
多 Agent 转交逻辑混乱
安全检查散落在代码里
```

新做法：

```text
用框架统一管理 Agent、工具、转交、护栏、追踪
```

### 优点

- Agent 结构更清楚。
- 工具调用和追踪更统一。
- 更容易做 Guardrails。
- 多 Agent handoff 更容易表达。

### 缺点

- 框架生态变化快。
- 抽象可能隐藏底层细节。
- 仍然需要后端做权限和参数校验。

### 项目里怎么用

你的 C++ 项目第一版不一定要直接集成 Agents SDK。

可以先学它的工程思想：

```text
Tool Registry
Step Limit
Tool Audit Log
Guardrails
Tracing
Human Confirmation
```

面试说法：

```text
我不会一上来让模型自由 Agent 化。生产里我会优先用固定 Workflow 和受控 Tool Calling。只有任务确实需要多步规划时，才引入 Agent，并限制最大步数、工具白名单、权限、预算和人工确认，同时记录每一步 trace。
```

---

## 3. A2A：Agent-to-Agent

### 是什么

A2A 是 Agent-to-Agent 协议思路，用来解决不同 Agent 之间怎么协作。

新手理解：

```text
一个 Agent 不是全能员工。
报销 Agent、合同 Agent、代码 Agent、客服 Agent 之间需要一种协作语言。
```

### 旧方案痛点

没有统一协作方式时：

```text
每个 Agent 的输入输出格式不同
任务状态不统一
错误处理不统一
跨 Agent 追踪困难
```

### 新方案怎么改

A2A 这类协议想标准化：

```text
Agent 能力描述
任务创建
任务状态
消息传递
结果返回
协作流程
```

### 优点

- 多 Agent 系统更容易互操作。
- 任务状态更清楚。
- 不同团队/厂商 Agent 更容易协作。

### 缺点

- 生态仍在发展。
- 简单项目不需要。
- 安全、身份、权限、审计更复杂。

### 项目里怎么用

第一版：

```text
一个企业 AI Copilot
```

后续可拆：

```text
Knowledge Agent
Reimbursement Agent
Leave Agent
Code Agent
Eval Agent
```

如果这些 Agent 要协作，再考虑 A2A。

---

## 4. MCP：模型接工具的标准插头

MCP 在 G03 已经讲过，这里从 Agent 工程角度再看。

MCP 解决的是：

```text
Agent 怎么发现工具？
工具参数 schema 怎么描述？
外部资源怎么暴露？
上下文怎么传递？
```

对 Agent 的价值：

```text
工具生态更标准
工具复用更容易
IDE、数据库、文件系统、企业系统可以用统一方式接入
```

但边界要记住：

```text
MCP 不是权限系统
MCP 不是审计系统
MCP 不是安全沙箱本身
```

项目里：

```text
V1：自己实现 Tool Calling
V2：抽象 Tool Registry
V3：把内部工具包装成 MCP Server
```

---

## 5. Guardrails

### 是什么

Guardrails 是 Agent/模型应用的安全护栏。

它可以出现在：

```text
输入前
工具调用前
模型输出前
最终返回前
```

新手理解：

```text
模型想干什么之前，先过安检。
```

### 常见 Guardrails

```text
输入安全检测
Prompt Injection 检测
敏感信息检测
工具参数校验
权限校验
输出内容审核
预算限制
最大步骤限制
人工确认
```

### 为什么 Agent 更需要 Guardrails

普通 RAG 最多答错。

Agent 可能：

```text
调用工具
查询数据库
发邮件
提交工单
修改状态
```

风险更高。

### 项目里怎么用

报销查询工具：

```text
允许自动执行
```

提交报销工具：

```text
必须 Human-in-the-loop
```

删除文档工具：

```text
默认不暴露给模型
```

面试说法：

```text
Agent 的风险比普通问答更高，因为它能调用工具影响外部系统。我会在工具调用前做参数校验、权限校验、工具白名单和预算限制；高风险操作必须让用户确认；所有工具调用都记录审计日志。Guardrails 不是一句 Prompt，而是一组后端控制点。
```

---

## 6. Agent 工程化对比表

| 能力 | Demo 做法 | 生产做法 |
|---|---|---|
| 工具调用 | 模型想调就调 | 工具白名单 + 参数校验 + 鉴权 |
| 步数 | 模型自己循环 | 最大步数和超时限制 |
| 成本 | 不统计 | token、工具调用、重试成本统计 |
| 安全 | Prompt 里提醒 | Guardrails + 后端权限 + 审计 |
| 可观测性 | 打印日志 | tracing 记录每一步 |
| 多 Agent | 手写胶水 | Handoff / A2A / MCP 思路 |
| 高风险操作 | 直接执行 | Human-in-the-loop |

---

## 7. 官方资料

- OpenAI Agents SDK：https://openai.github.io/openai-agents-python/
- OpenAI Agents SDK Tracing：https://openai.github.io/openai-agents-python/tracing/
- OpenAI Responses API 工具：https://platform.openai.com/docs/guides/tools
- Model Context Protocol：https://modelcontextprotocol.io/specification/2025-06-18/basic/index
- Agent2Agent A2A：https://a2a-protocol.org/latest/

