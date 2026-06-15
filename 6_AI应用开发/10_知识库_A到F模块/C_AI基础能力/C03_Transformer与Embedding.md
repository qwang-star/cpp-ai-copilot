# C03 Transformer 与 Embedding

## 1. Transformer 是什么

### 是什么

Transformer 是一种基于 Attention 的神经网络架构，现代大语言模型大多基于它。

### 为什么重要

GPT、BERT、T5、很多 Embedding 模型都来自 Transformer 架构。

### 核心思想

```text
让每个 token 在处理时，都能关注上下文中相关的 token。
```

## 2. Self-Attention

### 是什么

Self-Attention 是让序列内部的 token 彼此计算相关性。

### 直观理解

句子：

```text
小明把书放进书包，因为它很重。
```

模型需要判断“它”指的是书还是书包，Attention 能让“它”关注相关词。

### 核心计算

每个 token 会生成：

- Query。
- Key。
- Value。

通过 Query 和 Key 计算注意力权重，再加权 Value。

## 3. Multi-Head Attention

### 是什么

多个 Attention 头并行学习不同关系。

### 直观理解

一个头关注语法关系，一个头关注指代关系，一个头关注语义关系。

## 4. Position Encoding

### 是什么

Transformer 本身不天然知道 token 顺序，需要位置编码提供位置信息。

### 为什么重要

```text
“我打你”和“你打我”
```

词一样，顺序不同，意思完全不同。

## 5. Encoder

### 是什么

Encoder 负责理解输入序列，输出上下文表示。

### 代表模型

BERT。

### 适合任务

- 分类。
- 匹配。
- 向量表示。
- 信息抽取。

## 6. Decoder

### 是什么

Decoder 负责自回归生成，根据已生成 token 预测下一个 token。

### 代表模型

GPT。

### 适合任务

- 文本生成。
- 聊天。
- 代码生成。

## 7. Token

### 是什么

Token 是模型处理文本的基本单位，可以是字、词、子词或符号片段。

### 为什么重要

大模型按 token 计算：

- 上下文长度。
- 输入成本。
- 输出成本。
- 推理延迟。

### AI 场景

Prompt、历史对话、RAG chunk 都会占 token。

## 8. Tokenizer

### 是什么

Tokenizer 把原始文本转换成 token id。

### 常见方式

- BPE。
- WordPiece。
- SentencePiece。

### 常见坑

- 中文、英文、代码、符号 token 数差异大。
- 字符数不等于 token 数。
- 超长输入要裁剪。

## 9. Context Window

### 是什么

上下文窗口是模型一次能处理的最大 token 数。

### AI 场景

限制：

- 历史消息不能无限塞。
- RAG 召回资料不能无限塞。
- 工具 schema 不能无限多。

### 优化

- 摘要历史。
- 控制 TopK。
- Prompt 压缩。
- 分段处理。
- 长文档 Map-Reduce。

## 10. GPT 类模型

### 是什么

GPT 是 Decoder-only 自回归语言模型。

### 工作方式

```text
根据前面的 token 预测下一个 token
```

### 适合

- 聊天。
- 写作。
- 代码。
- 总结。

## 11. BERT 类模型

### 是什么

BERT 是 Encoder-only 双向理解模型。

### 适合

- 分类。
- 文本匹配。
- 实体识别。
- 向量表示。

## 12. Temperature

### 是什么

Temperature 控制生成随机性。

### 取值影响

- 低 temperature：更稳定、保守。
- 高 temperature：更发散、有创意。

### AI 场景

- 知识库问答：低。
- JSON 抽取：低。
- 创意写作：可高。

## 13. Top-p

### 是什么

Top-p 是核采样，从累计概率达到 p 的候选 token 中采样。

### AI 场景

用于控制生成多样性。

## 14. 预训练

### 是什么

预训练是在大规模通用数据上训练模型，获得通用语言能力。

### 结果

模型学到语法、知识、推理模式和语言表达。

## 15. 指令微调

### 是什么

指令微调用“指令-回答”数据训练模型，让模型更会遵循人类命令。

## 16. RLHF

### 是什么

RLHF 是基于人类反馈的强化学习，让模型输出更符合人类偏好。

### 掌握程度

理解概念即可。

## 17. Embedding 是什么

### 是什么

Embedding 是把文本、图片、代码等对象映射成向量。

### 直观理解

语义相近的内容，向量距离更近。

例子：

```text
怎么报销差旅费
差旅费用报销流程是什么
```

字面不同，但语义接近。

## 18. 语义相似度

### 是什么

衡量两个向量表达的语义是否接近。

常见方法：

- 余弦相似度。
- 点积。
- 欧氏距离。

## 19. 向量维度

### 是什么

Embedding 向量的长度。

例子：

```text
[0.12, -0.08, 0.33, ...]
```

### 注意

不同 Embedding 模型维度不同，向量空间也不同。

## 20. ANN 近似最近邻

### 是什么

ANN 是 Approximate Nearest Neighbor，用近似方式快速查找最相似向量。

### 为什么需要

如果有几百万 chunk，暴力计算相似度太慢。

### 常见索引

- HNSW。
- IVF。
- PQ。

## 21. Embedding 在 RAG 中的位置

离线：

```text
chunk -> embedding -> 向量库
```

在线：

```text
query -> embedding -> 相似度检索 -> TopK chunk
```

## 22. 面试回答模板

```text
Transformer 的核心是 Self-Attention，让每个 token 能关注上下文中相关 token。GPT 类模型用 Decoder 自回归生成，适合聊天和生成。Embedding 是把文本映射成向量，语义相近的文本在向量空间更接近，所以可以用于 RAG 的语义检索。
```

