# 04 Prompt 工程与结构化输出

## 1. Prompt 是什么

Prompt 是给模型的任务说明书。

一个好的 Prompt 不只是“问一句话”，而是要告诉模型：

- 你是谁。
- 要完成什么任务。
- 输入是什么。
- 输出格式是什么。
- 有哪些约束。
- 遇到异常怎么办。
- 有没有示例。

## 2. Prompt 基本结构

```text
角色
  -> 你是一个企业知识库问答助手

任务
  -> 根据给定资料回答用户问题

上下文
  -> 这里是检索到的文档片段

约束
  -> 只能基于资料回答，不知道就说不知道

输出格式
  -> JSON / Markdown / 固定字段

示例
  -> 给 1-3 个输入输出样例
```

## 3. 常见 Prompt 技巧

必须掌握：

- Role Prompting：角色设定。
- Instruction Prompting：明确任务。
- Context Prompting：提供上下文。
- Few-shot：给示例。
- Chain-of-Thought 可了解，但不要在面试中承诺暴露推理过程。
- Step-by-step：要求分步骤完成。
- Output Constraint：限制格式。
- Self-check：让模型检查是否符合要求。
- Refusal Rule：资料不足时拒答。

## 4. RAG Prompt 模板

```text
你是企业知识库问答助手。
请只基于【参考资料】回答【用户问题】。
如果参考资料中没有答案，请回答“根据当前资料无法确定”。
回答必须简洁、准确，并给出引用编号。

【参考资料】
[1] ...
[2] ...
[3] ...

【用户问题】
...

【输出要求】
- 先给直接答案
- 再列出依据
- 不要编造资料中不存在的信息
```

## 5. 信息抽取 Prompt 模板

```text
请从下面文本中抽取结构化信息。
如果字段不存在，填 null。
不要输出 JSON 之外的内容。

字段：
- name: string
- phone: string | null
- email: string | null
- education: array
- work_experience: array

文本：
...
```

## 6. 结构化输出

为什么需要结构化输出：

- 后端要解析。
- 数据要入库。
- 下游流程要使用。
- 自动化测试更方便。

常见方式：

- 明确要求输出 JSON。
- 使用 JSON Schema。
- 使用模型平台的 Structured Outputs。
- 使用 Function Calling。
- 输出后用程序校验和修复。

工程原则：

```text
模型输出不可信，必须校验。
```

校验内容：

- 是否合法 JSON。
- 必填字段是否存在。
- 字段类型是否正确。
- 枚举值是否合法。
- 数值范围是否合理。
- 是否包含敏感信息。

## 7. Prompt 版本管理

为什么要版本管理：

- Prompt 改动会影响线上效果。
- 需要回滚。
- 需要对比不同版本的准确率和成本。
- 需要知道一次模型调用用了哪个 Prompt。

推荐记录：

```text
prompt_id
prompt_name
version
template
variables
owner
created_at
evaluation_score
status
```

模型调用日志中记录：

```text
prompt_version
model_name
input_tokens
output_tokens
latency_ms
cost
trace_id
```

## 8. Prompt 常见坑

### 坑 1：把权限写在 Prompt 里

错误：

```text
请不要回答用户无权限的信息。
```

问题：

- 模型无法可靠判断真实权限。
- 用户可以 Prompt 注入绕过。

正确做法：

- 后端先做鉴权。
- 检索阶段只召回用户有权限的资料。
- 工具调用前再次鉴权。

### 坑 2：输出格式靠自觉

错误：

```text
请输出 JSON。
```

正确做法：

- 给 schema。
- 低 temperature。
- 使用结构化输出能力。
- 程序端校验。
- 失败后修复或重试。

### 坑 3：Prompt 越长越好

问题：

- 成本高。
- 延迟高。
- 重点被稀释。
- 上下文窗口被占满。

正确做法：

- 模板短而清晰。
- RAG 材料按相关性筛选。
- 工具按场景加载。

## 9. 面试高频题

### 如何让模型稳定输出 JSON？

回答要点：

- 低 temperature。
- 明确 JSON Schema。
- 使用结构化输出或 Function Calling。
- 不允许输出解释性文字。
- 后端做 JSON parse 和 schema 校验。
- 失败时重试或走修复链路。

### Prompt 注入是什么？

回答要点：

- 用户输入恶意指令，让模型忽略原本系统指令或泄露信息。
- 防护不能只靠 Prompt。
- 权限、工具调用、敏感数据过滤必须放在后端。

