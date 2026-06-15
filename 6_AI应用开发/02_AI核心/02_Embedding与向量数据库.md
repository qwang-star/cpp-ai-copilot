# 05 Embedding 与向量数据库

## 1. Embedding 是什么

Embedding 是把文本、图片、音频、代码等对象映射成向量。

直观理解：

```text
语义相近的内容，在向量空间里距离更近。
```

例子：

```text
“怎么申请报销”
“报销流程是什么”
“费用报销需要哪些材料”
```

这些句子字面不同，但语义接近，Embedding 后应该能互相召回。

## 2. 为什么不用关键词搜索就够了

关键词搜索擅长：

- 精确词匹配。
- 编号、名称、错误码、法律条款。
- 用户输入和文档用词一致的场景。

向量检索擅长：

- 同义表达。
- 语义相似。
- 用户问法和文档写法不同的场景。

生产系统常用：

```text
关键词检索 + 向量检索 + Rerank = 混合检索
```

## 3. 相似度计算

必须掌握：

- Cosine Similarity：余弦相似度。
- Dot Product：点积。
- Euclidean Distance：欧氏距离。

面试表达：

```text
余弦相似度关注方向是否相近，常用于文本语义相似度。
点积会同时受方向和向量长度影响。
实际使用时要看 embedding 模型和向量数据库推荐的距离度量。
```

## 4. 向量数据库

向量数据库负责存储向量，并快速做 TopK 相似度搜索。

常见组件：

- Collection：集合。
- Point / Entity：一条向量记录。
- Vector：向量。
- Payload / Metadata：业务元数据。
- Index：索引。
- TopK Search：最近邻查询。
- Filter：元数据过滤。
- Upsert：插入或更新。
- Delete：删除。

常见产品：

- Milvus。
- Qdrant。
- Weaviate。
- Elasticsearch / OpenSearch vector search。
- pgvector。
- FAISS。

## 5. Metadata 为什么重要

向量本身只表达语义，不能表达业务权限。

metadata 示例：

```json
{
  "chunk_id": "chunk_123",
  "document_id": "doc_456",
  "knowledge_base_id": "kb_001",
  "tenant_id": "tenant_a",
  "owner_id": "user_007",
  "department": "finance",
  "source": "员工报销制度.pdf",
  "page": 12,
  "created_at": "2026-06-01"
}
```

用途：

- 权限过滤。
- 知识库过滤。
- 文档类型过滤。
- 时间过滤。
- 返回引用来源。
- 删除或重建索引。

## 6. 向量索引

必须了解：

- 暴力搜索：准确但慢。
- HNSW：图索引，常见，召回速度快。
- IVF：聚类分桶，先找桶再搜索。
- PQ：压缩向量，降低内存但可能损失精度。

面试不必手写算法，但要知道：

```text
向量检索通常是近似最近邻搜索，用少量准确率换取巨大性能提升。
```

## 7. 文档到向量的完整链路

```text
原始文档
  -> 文本解析
  -> 清洗
  -> 切分 chunk
  -> 生成 embedding
  -> 写入向量库
  -> 保存 metadata
  -> MySQL 记录 chunk 与文档关系
```

在线检索：

```text
用户问题
  -> query embedding
  -> metadata filter
  -> TopK search
  -> 取回 chunk
  -> rerank
  -> 拼入 Prompt
```

## 8. 常见工程问题

### 问题 1：Embedding 模型换了怎么办？

回答：

- 不同模型向量维度和向量空间不同。
- 旧向量不能直接和新向量混用。
- 通常要新建 collection 或版本字段，重新生成索引。

### 问题 2：删除文档如何同步删除向量？

回答：

- MySQL 中 document 状态标记 deleted。
- 向量库按 document_id 删除对应 chunk。
- 删除失败要有补偿任务。
- 保留操作日志，支持重试。

### 问题 3：TopK 取多少合适？

回答：

- TopK 太小可能漏召回。
- TopK 太大引入噪声、增加 token 成本。
- 通常先召回较多候选，再 rerank 取少量高质量 chunk 进入 Prompt。

## 9. 面试高频题

### Embedding 和关键词搜索区别？

关键词搜索基于词面匹配，适合精确查询；Embedding 基于语义相似，适合同义问法和模糊表达。生产 RAG 常用混合检索提高召回和精度。

### 为什么向量库还要 metadata？

向量只负责语义相似，metadata 负责业务过滤、权限、来源、删除、引用和审计。

