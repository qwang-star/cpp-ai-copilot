# G03 AI 应用前沿：Hybrid RAG、GraphRAG、MCP、Structured Outputs、Prompt Caching

这一篇讲 AI 应用开发里最值得补进知识库的前沿技术。

主线很简单：

```text
普通 RAG 能跑
但生产 RAG 要更准、更稳、更便宜、更可控、更容易接工具
```

所以出现了：

```text
Hybrid Search：让检索更稳
GraphRAG：让长文档和跨文档关系更清楚
Structured Outputs：让模型输出更可控
Prompt Caching：让长 Prompt 更便宜、更快
MCP：让 Agent 接工具更标准化
```

---

## 1. 普通 RAG 的问题

普通 RAG 通常是：

```text
文档切 chunk
  -> chunk embedding
  -> 用户问题 embedding
  -> 向量库 TopK
  -> 拼 Prompt
  -> LLM 回答
```

这个方案能解决：

```text
大模型不知道企业私有知识
回答没有资料依据
长文档不能整篇塞进上下文
```

但它也有问题：

```text
纯向量检索可能漏掉精确关键词
chunk 之间关系断裂
长文档全局总结不稳
模型输出 JSON 不稳定
工具越来越多，接入越来越乱
Prompt 很长，成本和延迟高
```

前沿技术就是围绕这些问题继续改。

---

## 2. Hybrid Search：dense + sparse

### 它是什么

Hybrid Search 是把两类检索结合起来：

```text
dense vector：语义检索
sparse vector / BM25：关键词检索
```

新手理解：

```text
向量检索像找“意思相近”
关键词检索像找“字面命中”
Hybrid Search 是两种一起找，再融合结果
```

### 旧方案痛点

纯向量检索擅长：

```text
“出差回来交哪些材料”
≈ “差旅报销需要什么凭证”
```

但它可能不擅长：

```text
第 3.2.1 条
金额 200 元
BX2026001
项目编号 AI-COPILOT-01
```

企业制度里恰好很多都是：

```text
编号
金额
条款
专有名词
缩写
```

所以纯向量 RAG 容易漏。

### 新方案怎么改

Hybrid Search 做：

```text
用户问题
  -> dense embedding
  -> dense 检索 TopK

用户问题
  -> sparse / BM25
  -> 关键词检索 TopK

合并
  -> 去重
  -> 融合分数
  -> Rerank
```

### 优点

```text
语义召回更好
关键词召回更稳
对金额、编号、条款更友好
适合企业知识库、法律、制度、代码等场景
```

### 缺点

```text
系统更复杂
需要调 dense/sparse 权重
结果融合策略要评测
索引和存储成本更高
```

### 项目里怎么用

你的 C++ AI Copilot 可以这样讲：

```text
第一版先做 dense embedding 检索。
第二版增加关键词检索。
第三版使用 dense + sparse 的 Hybrid Search，再接 Rerank。
```

面试说法：

```text
企业制度问答不能只靠向量检索，因为金额、条款号、报销单号这类精确词很重要。我会用 dense 向量召回语义相近内容，用 sparse/BM25 召回精确关键词内容，然后合并去重再 Rerank。这样比单纯向量检索更稳。
```

---

## 3. GraphRAG：从“找片段”到“理解关系”

### 它是什么

GraphRAG 是把图结构和 RAG 结合。

Microsoft Research 对 GraphRAG 的描述是：

```text
结合文本抽取、网络分析、LLM prompting 和总结，让系统更丰富地理解文本数据集。
```

新手理解：

```text
普通 RAG 是从书里找几段话。
GraphRAG 是先把书里的人、部门、制度、流程、关系画成地图，再基于地图回答。
```

### 旧方案痛点

普通 chunk RAG 擅长局部问题：

```text
差旅报销需要哪些材料？
```

但面对全局问题就比较弱：

```text
公司报销制度里，哪些部门审批链路最长？
采购制度和报销制度有哪些冲突？
某个项目从申请到报销涉及哪些角色？
```

因为这些答案可能散在多个文档、多个 chunk 里。

### 新方案怎么改

GraphRAG 大致做：

```text
文档
  -> 抽取实体
  -> 抽取关系
  -> 构建知识图谱
  -> 社区/主题聚类
  -> 图上检索 + 文本检索
  -> LLM 总结回答
```

### 优点

```text
更适合跨文档、全局总结、关系推理
能解释实体之间的关系
适合组织制度、法律、科研、复杂业务流程
```

### 缺点

```text
构建成本高
实体和关系抽取可能出错
更新链路复杂
不适合所有简单问答
评测更难
```

### 项目里怎么用

第一版不用直接做 GraphRAG。

你可以这样设计扩展：

```text
V1：普通 RAG
V2：Hybrid Search + Rerank
V3：对企业制度抽取部门、流程、角色、审批关系，做轻量 GraphRAG
```

面试说法：

```text
普通 RAG 更适合局部事实问答，而 GraphRAG 更适合跨文档、跨实体关系的问题。我的项目第一版会先用 Hybrid RAG 做稳定问答，如果后续要回答“哪些制度互相冲突”“某个流程涉及哪些角色”这种全局问题，可以对文档抽取实体和关系，构建轻量知识图谱，再结合图检索和文本检索。
```

---

## 4. Structured Outputs：让模型别再“差不多 JSON”

### 它是什么

Structured Outputs 是让模型按 JSON Schema 这类结构约束输出。

新手理解：

```text
以前你只是跟模型说“请输出 JSON”
现在你把字段、类型、结构都规定好，让模型按 schema 输出
```

### 旧方案痛点

普通 Prompt：

```text
请输出 JSON，包含 answer 和 citations。
```

模型可能输出：

```text
这是你要的 JSON：
{
  answer: ...
}
```

问题：

```text
不是合法 JSON
字段缺失
类型不对
多输出解释文本
数组格式不稳定
```

### 新方案怎么改

使用结构化输出：

```text
answer: string
citations: array
confidence: number
need_tool_call: boolean
tool_name: string
tool_args: object
```

让后端更容易解析和校验。

### 优点

```text
后端解析稳定
适合 Tool Calling 参数
适合信息抽取
适合评测结果输出
减少“看起来像 JSON 但解析失败”
```

### 缺点

```text
不同模型厂商支持不完全一致
JSON Schema 子集可能有限
复杂 schema 仍然要测试
业务后端仍要做二次校验
```

### 项目里怎么用

在 C++ AI Copilot 中：

```text
RAG 回答输出 answer + citations
Tool Calling 输出 tool_name + arguments
文档解析输出结构化字段
评测输出 correctness + faithfulness + reason
```

面试说法：

```text
生产里不能只靠“请输出 JSON”的 Prompt。我会使用 Structured Outputs 或工具 schema 约束模型输出结构，同时后端仍然做 JSON 解析、字段校验和权限校验。模型输出结构化只是第一层保证，可信执行一定在后端。
```

---

## 5. Prompt Caching：让长 Prompt 更便宜更快

### 它是什么

Prompt Caching 是对重复 Prompt 前缀做缓存，从而降低延迟和成本。

OpenAI 文档里提到：

```text
静态内容放在 prompt 前面，动态内容放后面，有助于命中缓存。
```

新手理解：

```text
每次考试卷前半部分的说明都一样，就别每次重新读一遍。
```

### 旧方案痛点

AI Copilot 的 Prompt 里有很多固定内容：

```text
系统角色
安全规则
输出格式
工具说明
Few-shot 示例
RAG 回答规范
```

每次都完整处理，会导致：

```text
输入 token 成本高
首 token 延迟高
长上下文更慢
```

### 新方案怎么改

把 Prompt 组织成：

```text
稳定前缀
  system rules
  tool schemas
  output schema
  few-shot examples

动态后缀
  user question
  retrieved chunks
  conversation state
```

这样更容易命中缓存。

### 优点

```text
降低延迟
降低成本
适合长 system prompt 和大量固定工具 schema
不一定需要改业务逻辑
```

### 缺点

```text
缓存命中依赖精确前缀
动态内容放前面会破坏命中
不同厂商策略不同
缓存不是永久的
敏感数据仍要遵守供应商和合规策略
```

### 项目里怎么用

在 C++ AI Copilot 中：

```text
固定 system prompt 放最前面
工具 schema 放前面
RAG 动态 chunk 放后面
用户问题放最后
记录 cached_tokens / input_tokens / output_tokens
```

面试说法：

```text
如果系统 Prompt、工具说明和输出格式很长，我会把这些稳定内容放在 Prompt 前缀，把用户问题和检索 chunk 放在后面，以提高 Prompt Cache 命中率。这样可以降低首 token 延迟和输入 token 成本。但缓存不能替代业务缓存，也不能假设所有请求都命中，所以还要做成本监控。
```

---

## 6. MCP：让 Agent 接工具更标准

### 它是什么

MCP 是 Model Context Protocol，用来标准化模型/Agent 和外部工具、数据源之间的连接方式。

新手理解：

```text
以前每个工具都要单独接一套 API。
MCP 想把“模型怎么发现工具、怎么调用工具、怎么拿上下文”标准化。
```

### 旧方案痛点

没有统一协议时：

```text
数据库一套接法
文件系统一套接法
Git 一套接法
内部工单系统一套接法
IDE 插件一套接法
```

Agent 接工具会变成：

```text
工具越多，胶水代码越多
权限和审计越乱
上下文格式不统一
```

### 新方案怎么改

MCP 提供一种标准接口，让工具暴露：

```text
工具列表
参数 schema
资源
上下文
调用协议
```

### 优点

```text
工具接入更标准
Agent 生态更容易复用
适合 IDE、企业系统、数据库、知识库接入
减少重复胶水代码
```

### 缺点

```text
仍是快速演进生态
安全边界要自己设计清楚
工具权限、审计、沙箱非常关键
不能因为有 MCP 就放松后端鉴权
```

### 项目里怎么用

你的项目第一版不用上 MCP。

你可以这样演进：

```text
V1：自己定义 Tool Calling schema
V2：封装内部 Tool Registry
V3：把部分工具以 MCP Server 形式暴露
```

例如：

```text
知识库检索 MCP Server
报销系统 MCP Server
请假系统 MCP Server
代码仓库 MCP Server
```

面试说法：

```text
我的项目第一版会先实现受控 Tool Calling，因为这样权限和参数校验最清楚。如果后续工具越来越多，可以考虑 MCP，把工具发现、参数 schema 和上下文访问标准化。但 MCP 只是接入协议，安全仍然要靠后端鉴权、参数校验、最小权限和审计日志。
```

---

## 7. 五个前沿技术怎么串成一条项目链路

```text
用户提问
  -> Prompt Caching 复用稳定系统提示词和工具 schema
  -> Hybrid Search 同时召回语义相关和关键词命中的 chunk
  -> Rerank 精排
  -> 如果是全局关系问题，走 GraphRAG 扩展
  -> Structured Outputs 约束模型输出 answer/citations/tool_call
  -> Tool Calling 或 MCP 接入业务系统
  -> 后端鉴权、执行、审计
```

这就是现代 AI 应用从 demo 到生产的演进：

```text
能答
  -> 答准
  -> 答稳
  -> 答得便宜
  -> 能接工具
  -> 能治理
```

---

## 8. 官方资料

- OpenAI Structured Outputs：https://platform.openai.com/docs/guides/structured-outputs
- OpenAI Prompt Caching：https://platform.openai.com/docs/guides/prompt-caching
- Model Context Protocol：https://modelcontextprotocol.io/specification/2025-06-18/basic/index
- Microsoft GraphRAG：https://www.microsoft.com/en-us/research/project/graphrag/
- Qdrant Sparse / Hybrid Search：https://qdrant.tech/articles/sparse-vectors/
- Qdrant Vectors：https://qdrant.tech/documentation/manage-data/vectors/
- Milvus Multi-Vector Hybrid Search：https://milvus.io/docs/multi-vector-search.md
- pgvector README：https://github.com/pgvector/pgvector/blob/master/README.md

