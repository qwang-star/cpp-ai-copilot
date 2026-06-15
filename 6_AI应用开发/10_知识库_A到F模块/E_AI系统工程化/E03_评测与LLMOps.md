# E03 评测与 LLMOps

## 1. AI 评测是什么

### 是什么

AI 评测是用指标和样本判断系统效果，而不是靠感觉。

### 为什么重要

Prompt、模型、chunk、检索策略改动都会影响结果。没有评测，就不知道变好还是变坏。

## 2. Golden Dataset

### 是什么

固定高质量评测集。

每条样本包含：

```text
question
expected_answer
relevant_chunk_ids
must_have_points
forbidden_points
category
difficulty
```

### 来源

- 真实用户问题。
- 客服工单。
- 业务专家整理。
- bad case。

## 3. Recall@K

### 是什么

正确资料是否出现在前 K 个召回结果中。

### 公式直觉

```text
如果正确 chunk 在 Top5 中，Recall@5 命中。
```

### AI 场景

RAG 检索阶段核心指标。

## 4. Precision@K

### 是什么

TopK 中有多少结果是相关的。

### AI 场景

召回结果太多噪声，会影响生成答案。

## 5. MRR

### 是什么

Mean Reciprocal Rank，关注第一个正确结果排在第几位。

### AI 场景

正确 chunk 排得越靠前，越容易被模型利用。

## 6. NDCG

### 是什么

衡量排序质量，相关性越高的结果排前面越好。

## 7. Answer Correctness

### 是什么

答案是否正确。

### 评估方式

- 人工评估。
- 规则匹配。
- LLM-as-a-Judge。

## 8. Faithfulness

### 是什么

答案是否忠于提供的参考资料。

### 为什么重要

答案即使看起来正确，如果不是基于资料，也可能是幻觉。

## 9. Citation Accuracy

### 是什么

引用是否真的支持答案。

### AI 场景

企业知识库问答必须可追溯。

## 10. Hallucination Rate

### 是什么

幻觉率，模型编造或输出无依据内容的比例。

### 降低方法

- RAG。
- 拒答。
- 引用。
- 低 temperature。
- 答案校验。

## 11. LLM-as-a-Judge

### 是什么

使用大模型作为裁判评价答案。

### 优点

- 成本低于人工。
- 可大规模评测。

### 风险

- 裁判模型也会错。
- 评分标准漂移。
- 需要固定 prompt 和模型版本。

## 12. A/B Test

### 是什么

把流量分给两个版本，对比线上效果。

### AI 场景

对比：

- Prompt A vs Prompt B。
- 模型 A vs 模型 B。
- 检索策略 A vs B。

## 13. LLMOps 是什么

### 是什么

LLMOps 是大模型应用的生命周期管理。

包括：

- Prompt 管理。
- 模型管理。
- 数据集管理。
- 评测流水线。
- 灰度发布。
- 监控告警。
- 成本分析。
- bad case 闭环。

## 14. Bad Case 闭环

### 流程

```text
线上错误
  -> 收集日志
  -> 分析原因
  -> 标注样本
  -> 加入评测集
  -> 优化策略
  -> 回归测试
  -> 灰度上线
```

## 15. 面试回答模板

```text
我会把 RAG 评测拆成检索和生成两层。检索看 Recall@K、MRR、NDCG；生成看答案正确性、忠实性、引用准确率和幻觉率。上线前用 Golden Dataset 做离线评测，上线后结合用户反馈和 A/B Test，并把 bad case 加回评测集形成闭环。
```

## 16. 前沿升级：OpenTelemetry 与 AI 可观测性

### 为什么传统日志不够

AI 应用一次请求很长：

```text
Auth
  -> Query Rewrite
  -> Embedding
  -> Vector Search
  -> Rerank
  -> Prompt Build
  -> Model Gateway
  -> LLM Provider
  -> SSE Stream
```

只看一行日志：

```text
request cost 12s
```

你不知道慢在哪里。

### OpenTelemetry 怎么改进

OpenTelemetry 把一次请求拆成 trace 和 span：

```text
chat.request
  auth.check
  rag.embedding
  rag.vector_search
  rag.rerank
  prompt.build
  model.call
  sse.stream
```

同时记录：

```text
metrics：QPS、P95、错误率、token 成本、缓存命中率
logs：具体错误和上下文
```

优点：

- 跨服务链路清楚。
- 能定位 AI 请求慢在哪一段。
- 厂商中立，适合接 Prometheus/Grafana/Jaeger 等生态。

代价：

- 接入成本更高。
- 要设计 trace_id、span、采样率。
- 高流量下要控制观测数据成本。

详细对比看：

- [G02_高性能后端_io_uring_eBPF_OpenTelemetry.md](../G_前沿技术与新趋势/G02_高性能后端_io_uring_eBPF_OpenTelemetry.md)
