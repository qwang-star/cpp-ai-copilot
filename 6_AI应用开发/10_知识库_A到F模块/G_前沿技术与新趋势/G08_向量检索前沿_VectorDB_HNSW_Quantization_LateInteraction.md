# G08 向量检索前沿：VectorDB、HNSW、量化、Late Interaction

这一篇解决一个很现实的面试追问：

```text
你说你用了向量数据库，那如果数据量从 1 万 chunk 涨到 1000 万 chunk，检索又慢又不准怎么办？
```

普通 RAG 像是在图书馆里“凭感觉找相似书”。数据少的时候还能用；数据一大，问题就来了：

```text
找得慢
找不准
内存贵
权限过滤拖慢
关键词和语义互相漏
```

所以向量检索的前沿不是只背“向量库”三个字，而是要知道：

```text
索引怎么加速
量化怎么省内存
稀疏检索怎么补关键词
Late Interaction 怎么兼顾精细匹配
Rerank 怎么把候选排好
评测怎么证明真的变好了
```

---

## 1. 总览表

| 技术 | 旧方案痛点 | 新方案改进 | 优点 | 代价 | 项目里怎么用 |
|---|---|---|---|---|---|
| HNSW 调参 | 默认索引参数不一定适合业务 | 调整 M、efConstruction、efSearch | 召回和延迟可控 | 需要压测和评测 | 企业知识库 TopK 召回优化 |
| IVF / 分桶检索 | 全库搜索太慢 | 先聚类分桶，再搜相关桶 | 大规模检索快 | 可能漏召回 | 超大知识库分区搜索 |
| PQ / SQ / Binary Quantization | 向量占内存太大 | 压缩向量表示 | 降低内存和成本 | 精度可能下降 | 历史冷数据或低成本索引 |
| Sparse Vector / BM25 | 纯 dense 漏金额、编号、条款 | 关键词稀疏向量补精确词 | 召回更稳 | 需要融合分数 | 制度编号、金额、法律条款 |
| Hybrid Search | 只靠语义或只靠关键词都偏 | dense + sparse 融合 | 语义和精确匹配兼顾 | 融合策略要评测 | RAG 默认推荐方案 |
| RRF | 不同检索分数不可比 | 用排名而不是原始分数融合 | 简单稳健 | 不利用分数大小 | 向量召回和关键词召回合并 |
| Late Interaction / ColBERT | 单向量表示丢细节 | 每个 token 保留向量，查询时细粒度匹配 | 精度高 | 存储和计算更重 | 高价值问答、代码/条款检索 |
| Cross-Encoder Rerank | 初召回结果噪声多 | 对 query-doc pair 重新打分 | 最终上下文更准 | 延迟和成本增加 | Top50 变 Top5 |

---

## 2. HNSW：向量库里的“高速路 + 小路”

### 是什么

HNSW 是一种常见的近似最近邻索引。

新手可以把它想成地图：

```text
高层图：像高速路，先快速接近目标城市
低层图：像城市小路，再细找最近的门牌号
```

### 旧方案痛点

最朴素的向量检索是暴力扫全库：

```text
用户问题向量
  -> 和 1000 万个 chunk 向量逐个算相似度
  -> 排序取 TopK
```

这在数据量大时太慢。

### 新方案怎么改进

HNSW 不是每次扫全库，而是在图结构里“沿着更相似的邻居走”：

```text
先从高层图快速跳到大概区域
再进入底层图扩大搜索
最后得到近似 TopK
```

### 关键参数

```text
M：每个节点连接多少邻居，越大越准但越占内存
efConstruction：建索引时搜索多宽，越大索引质量越好但构建越慢
efSearch：查询时搜索多宽，越大召回越高但查询越慢
```

### 优点

- 查询快。
- 召回率和延迟可以通过参数平衡。
- 很多向量数据库都支持。

### 缺点

- 占内存。
- 参数不能拍脑袋，要压测。
- 删除和频繁更新会增加维护成本。

### 面试说法

```text
HNSW 适合大规模语义检索，它用多层图结构避免全量扫描。项目里我会通过 efSearch 控制查询时召回和延迟的平衡，并用 Recall@K、P95 延迟和内存占用一起评估，而不是只看单次搜索结果。
```

---

## 3. IVF：先分区，再找人

### 是什么

IVF 是 Inverted File Index。

直观理解：

```text
先把 1000 万个 chunk 按语义聚成很多桶
用户问题来了，先判断它更像哪几个桶
只在这些桶里找 TopK
```

### 旧方案痛点

HNSW 虽然快，但超大规模、低成本场景下，内存和构建成本仍然明显。

### 新方案怎么改进

IVF 先做粗粒度聚类：

```text
全库向量
  -> 聚类中心
  -> 每条向量进入某个桶

查询时
  -> 找最近的 nprobe 个桶
  -> 桶内搜索
```

### 优点

- 大规模检索速度快。
- 和 PQ 组合后能显著省内存。

### 缺点

- 如果分桶错了，可能漏掉真正相关的结果。
- nprobe 越大越准，但越慢。

### 项目里怎么用

企业知识库第一阶段不用急着上 IVF。等数据量到百万级、千万级 chunk，再考虑：

```text
热知识库：HNSW，高召回
冷历史库：IVF + PQ，低成本
```

---

## 4. 量化：给向量“压缩行李箱”

### 是什么

量化是把高精度向量压缩成更低精度表示。

比如：

```text
float32 -> float16
float32 -> int8
原始向量 -> PQ code
原始向量 -> binary vector
```

### 旧方案痛点

假设一个 chunk 向量是 1536 维 float32：

```text
1536 * 4 bytes = 6144 bytes
1000 万条约 61GB
```

还没算索引结构、metadata、复制副本。

### 新方案怎么改进

量化把每个向量变小：

```text
SQ：标量量化，逐个维度压缩
PQ：产品量化，把向量切段后用码本表示
Binary Quantization：变成二进制表示，极致压缩
```

### 优点

- 内存和磁盘成本下降。
- 缓存命中更好。
- 有些场景查询更快。

### 缺点

- 相似度会有误差。
- 精确召回可能下降。
- 不同模型和数据集效果差异大。

### 面试说法

```text
量化不是免费加速。它的核心收益是降低向量存储和内存成本，但会牺牲一部分相似度精度。我会在冷数据、低价值知识库或候选召回阶段使用量化，并通过 Recall@K 和人工 bad case 评测确认损失可接受。
```

---

## 5. Sparse Vector 和 BM25：别让金额、编号丢了

### 是什么

Dense vector 擅长语义相似：

```text
“报销机票” ≈ “差旅交通费用”
```

Sparse vector / BM25 擅长关键词精确匹配：

```text
“第 13 条”
“5000 元”
“HR-2025-009”
```

### 旧方案痛点

纯向量检索可能觉得下面两句话很像：

```text
单笔报销不得超过 5000 元
单笔报销不得超过 8000 元
```

但企业制度里，5000 和 8000 的差异就是生死线。

### 新方案怎么改进

Hybrid Search 把 dense 和 sparse 合起来：

```text
用户问题
  -> dense embedding 语义召回
  -> sparse/BM25 关键词召回
  -> 融合排序
  -> Rerank
```

### 优点

- 语义问题不会漏。
- 金额、编号、专有名词更稳。
- 企业知识库非常适合。

### 缺点

- 两路分数不可直接比较。
- 融合权重要调。
- 需要评测集验证。

### 面试说法

```text
企业 RAG 不能只靠 dense embedding。制度、合同、代码文档里有大量金额、条款号、错误码、函数名，纯语义检索容易漏。我会用 dense + sparse 的混合检索，再用 Rerank 控制最终进入 Prompt 的上下文质量。
```

---

## 6. RRF：按名次融合，而不是按分数吵架

### 是什么

RRF 是 Reciprocal Rank Fusion。

它不强行比较“向量分数 0.78”和“BM25 分数 12.6”谁更大，而是看排名：

```text
某文档在向量检索排第 2
某文档在 BM25 检索排第 5
它在两个榜单都靠前，就应该加分
```

### 旧方案痛点

不同检索器分数尺度不同：

```text
向量相似度：0 到 1
BM25：可能是 3、10、25
```

硬加权容易失真。

### 新方案怎么改进

RRF 使用排名分数：

```text
score = 1 / (k + rank)
```

排名越靠前，分数越高。

### 优点

- 简单。
- 对不同检索器分数尺度不敏感。
- 很适合第一版混合检索融合。

### 缺点

- 不利用原始分数强弱。
- 对业务权重控制不够细。

---

## 7. Late Interaction / ColBERT：别把整篇文章压成一个点

### 是什么

普通 embedding 常把一个 chunk 压成一个向量。

Late Interaction 的思路是：

```text
不要急着把所有 token 信息揉成一个点
保留 token 级向量
查询时让 query token 和 document token 做细粒度匹配
```

ColBERT 是这个方向的代表。

### 旧方案痛点

单向量像把一整段制度压成一个坐标点。压缩太狠时，细节会丢。

比如：

```text
“试用期员工是否可以报销跨城市培训住宿费？”
```

这里同时有：

```text
试用期员工
跨城市培训
住宿费
报销
```

单向量可能只抓住“报销”。

### 新方案怎么改进

Late Interaction 保留更多细节：

```text
query token vectors
document token vectors
  -> token 级最大相似度
  -> 聚合为文档分数
```

### 优点

- 精细匹配能力强。
- 对长文本、代码、法律条款更友好。

### 缺点

- 存储更大。
- 计算更重。
- 工程复杂度高于普通向量检索。

### 项目里怎么用

第一版不建议上来就用 Late Interaction。

更稳的路线：

```text
第一版：dense + BM25 + Rerank
第二版：对高价值知识库尝试 multivector / ColBERT
第三版：用评测集决定是否保留
```

---

## 8. Rerank：初筛后再请专家排序

### 是什么

Rerank 是精排。

可以把检索链路想成招聘：

```text
初召回：HR 从 1000 份简历里捞 50 份
Rerank：技术面试官从 50 份里挑 5 份
```

### 常见方案

```text
Cross-Encoder Reranker：query 和 chunk 一起输入模型打分
LLM Rerank：让大模型判断相关性
规则加权：标题匹配、时间、权限、文档类型加权
```

### 优点

- 显著提升最终上下文质量。
- 能减少无关 chunk 进入 Prompt。
- 对回答准确率影响很大。

### 缺点

- 增加延迟。
- 增加成本。
- 候选太多会拖慢。

### 项目落地

```text
召回 Top50
  -> metadata 权限过滤
  -> Rerank Top5
  -> 拼 Prompt
```

如果延迟太高：

```text
减少候选数
缓存高频 query 的 rerank 结果
按知识库等级决定是否 rerank
用 Feature Flag 灰度打开
```

---

## 9. 向量检索的线上指标

不能只说“我感觉更准了”。

要看：

```text
Recall@K：标准答案相关 chunk 有没有被召回
MRR：第一个正确 chunk 排得靠不靠前
NDCG：多个相关 chunk 的整体排序质量
P50/P95/P99 latency：检索延迟
QPS：吞吐
memory per million vectors：每百万向量内存
index build time：索引构建时间
filter hit ratio：权限过滤命中情况
bad case rate：人工坏例比例
```

### 面试说法

```text
我不会只看一次问答是否答对，而是会用固定评测集评估召回链路。比如同一批问题分别跑纯向量、BM25、Hybrid Search、Hybrid + Rerank，对比 Recall@K、MRR、NDCG 和 P95 延迟，最后决定是否上线。
```

---

## 10. 在 C++ 企业 AI Copilot 里的推荐路线

```text
阶段 1：普通向量检索 + metadata filter
阶段 2：Hybrid Search = dense + BM25/sparse
阶段 3：Rerank TopN
阶段 4：HNSW 参数压测
阶段 5：冷数据量化或分区
阶段 6：高价值场景试 Late Interaction
```

你的 C++ 后端不一定自己实现这些算法。

更合理的架构是：

```text
C++ API Server
  -> Retrieval Service / VectorDB Client
  -> Qdrant / Milvus / Elasticsearch / OpenSearch
  -> Rerank Service
  -> Model Gateway
```

C++ 负责：

```text
鉴权
请求编排
超时重试
结果融合
日志和 trace
统一响应
```

向量库负责：

```text
索引
近似搜索
过滤
存储
分片和副本
```

---

## 11. 面试总回答模板

```text
向量数据库不是把 embedding 存进去就结束了。数据量上来后，我会从召回、延迟和成本三条线优化。召回上用 dense + sparse 的 Hybrid Search 解决金额、条款号、专有名词漏召回，再用 Rerank 控制进入 Prompt 的 TopK；性能上通过 HNSW 的 efSearch、分片和过滤策略平衡 P95 延迟；成本上可以对冷数据使用 PQ、SQ 或 Binary Quantization。每次改动都要用 Recall@K、MRR、NDCG 和 P95 延迟评测，而不是只凭主观感觉。
```

---

## 12. 官方资料入口

- Qdrant Hybrid Queries：https://qdrant.tech/documentation/search/hybrid-queries/
- Qdrant Quantization：https://qdrant.tech/documentation/guides/quantization/
- Qdrant Multivectors：https://qdrant.tech/documentation/concepts/vectors/#multivectors
- Milvus Hybrid Search：https://milvus.io/docs/multi-vector-search.md
- Milvus Full Text Search：https://milvus.io/docs/full-text-search.md
- Elasticsearch Reciprocal Rank Fusion：https://www.elastic.co/guide/en/elasticsearch/reference/current/rrf.html
- ColBERT 论文：https://arxiv.org/abs/2004.12832
