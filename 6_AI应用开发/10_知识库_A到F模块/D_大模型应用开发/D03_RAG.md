# D03 RAG

## 1. RAG 是什么

### 是什么

RAG 是 Retrieval-Augmented Generation，检索增强生成。

核心思想：

```text
先检索外部知识，再让模型基于知识生成答案。
```

### 解决什么问题

- 模型知识过时。
- 企业私有知识不在模型中。
- 模型容易幻觉。
- 答案需要引用来源。

## 2. RAG 离线链路

```text
文档上传
  -> 文件存储
  -> 文档解析
  -> 文本清洗
  -> chunk 切分
  -> embedding
  -> 写向量库
  -> 保存 metadata
  -> 文档状态 ready
```

## 3. RAG 在线链路

```text
用户提问
  -> 鉴权
  -> Query Rewrite
  -> Query Embedding
  -> metadata filter
  -> 向量召回
  -> 关键词召回
  -> 合并去重
  -> Rerank
  -> 拼 Prompt
  -> LLM 生成
  -> 返回引用
```

## 4. 文档解析

### 是什么

把 PDF、Word、HTML、Markdown、Excel、图片等转成可处理文本。

### 难点

- PDF 阅读顺序。
- 表格结构。
- 页眉页脚。
- 扫描件 OCR。
- 图片和图表。

### AI 场景

解析质量决定 chunk 质量，chunk 质量决定召回质量。

## 5. 文本清洗

### 是什么

去掉无意义内容，保留有用结构。

包括：

- 去页眉页脚。
- 去水印。
- 修复换行。
- 保留标题。
- 表格转 Markdown。

## 6. Chunk

### 是什么

Chunk 是文档切分后的文本片段，是 RAG 检索的基本单位。

### 为什么重要

模型不是直接搜整篇文档，而是搜 chunk。

## 7. Chunk 大小

### 太大

- 召回不精准。
- 占 token。
- 噪声多。

### 太小

- 语义不完整。
- 上下文断裂。
- 答案缺依据。

### 策略

- 按标题切。
- 按段落切。
- 递归切分。
- 加 overlap。
- 表格特殊处理。

## 8. Query Rewrite

### 是什么

把用户问题改写成更适合检索的问题。

### 场景

多轮对话：

```text
用户：那报销呢？
改写：出差交通费如何报销？
```

### 作用

- 补全上下文。
- 扩展同义词。
- 提高召回。

## 9. 向量检索

### 是什么

把 query 转成向量，到向量库找相似 chunk。

### 优点

能处理语义相似。

### 缺点

对精确词、编号、错误码可能不如关键词检索。

## 10. 关键词检索

### 是什么

基于词面匹配的检索，如 BM25。

### 适合

- 错误码。
- 法条编号。
- 产品型号。
- 人名。

## 11. 混合检索

### 是什么

结合向量检索和关键词检索。

```text
vector topK + keyword topK -> merge -> dedup -> rerank
```

### 为什么重要

兼顾语义召回和精确匹配。

## 12. Rerank

### 是什么

重排是对初步召回结果重新排序。

### 作用

召回阶段追求“不漏”，Rerank 阶段追求“排准”。

### AI 场景

先召回 50 个候选，再重排取前 5 个进入 Prompt。

## 13. Metadata Filter

### 是什么

根据业务元数据过滤检索范围。

### 字段

- tenant_id。
- knowledge_base_id。
- owner_id。
- role。
- document_type。
- created_at。

### AI 场景

权限过滤必须在检索阶段做，而不是让模型自己判断。

## 14. 引用来源

### 是什么

答案中标明依据来自哪些文档、页码、chunk。

### 为什么重要

- 提高可信度。
- 方便用户核查。
- 方便排查 bad case。

## 15. 拒答

### 是什么

当资料不足时，模型应该明确说无法确定，而不是编。

### AI 场景

企业知识库问答必须支持拒答，否则幻觉风险高。

## 16. RAG 评测

### 检索指标

- Recall@K。
- MRR。
- NDCG。

### 生成指标

- Correctness。
- Faithfulness。
- Citation Accuracy。
- Hallucination Rate。

## 17. RAG 常见问题

### 召回不到

原因：

- 文档解析差。
- chunk 切坏。
- query 太短。
- topK 太小。
- embedding 不适合。

### 召回到了但答错

原因：

- 噪声太多。
- Prompt 不约束。
- 模型忽略资料。
- Rerank 缺失。

## 18. 面试回答模板

```text
RAG 分为离线和在线两条链路。离线把文档解析、清洗、切分、向量化并写入向量库；在线把用户问题向量化，结合权限过滤召回相关 chunk，再经过混合检索和 Rerank，把最相关材料拼入 Prompt 让大模型生成答案。RAG 能缓解幻觉，但还要靠引用、拒答和评测闭环保证效果。
```

## 19. 前沿升级：Hybrid RAG 与 GraphRAG

### 普通 RAG 的不足

普通向量 RAG 擅长语义相似，但在企业资料里容易漏：

```text
金额
条款号
报销单号
专有名词
英文缩写
```

### Hybrid RAG 怎么改进

```text
dense embedding 检索
  +
sparse / BM25 关键词检索
  +
合并去重
  +
Rerank
```

优点：

- 同义问题能召回。
- 精确关键词也能召回。
- 对制度、法律、代码、报销单号更稳。

缺点：

- 索引和融合逻辑更复杂。
- dense/sparse 权重要调。
- 必须靠评测证明变好。

### GraphRAG 怎么改进

普通 chunk RAG 是：

```text
找几段相关文本
```

GraphRAG 是：

```text
抽取实体和关系
  -> 建图
  -> 图上检索和总结
```

它更适合：

```text
跨文档关系
全局总结
流程和角色关系
制度冲突分析
```

但代价是：

```text
构建成本高
更新复杂
实体关系抽取可能出错
评测更难
```

详细对比看：

- [G03_AI应用前沿_HybridRAG_GraphRAG_MCP_StructuredOutputs.md](../G_前沿技术与新趋势/G03_AI应用前沿_HybridRAG_GraphRAG_MCP_StructuredOutputs.md)
