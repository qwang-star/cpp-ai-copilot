# image2 批处理提示词预览

## 01_A01_network_flow

- Source: `../_images/A01_network_flow.png`
- Output: `../_images/image2_A01_network_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A01 网络：一次 AI 问答请求怎样穿过网络
Subtitle: 从 DNS、TCP、HTTPS、HTTP 到 SSE 流式返回
Exact step order: 用户提问 -> DNS解析 -> TCP握手 -> TLS加密 -> HTTP请求 -> 负载均衡 -> RAG检索 -> 模型调用 -> SSE流式 -> 断开释放
Reference image path: ../_images/A01_network_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 02_A02_os_flow

- Source: `../_images/A02_os_flow.png`
- Output: `../_images/image2_A02_os_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A02 操作系统：一份 PDF 如何被后台处理
Subtitle: 从进程、线程、系统调用到 Worker 线程池
Exact step order: 启动进程 -> 请求线程 -> 流式读文件 -> 系统调用 -> 写入存储 -> 投递MQ -> Worker线程池 -> CPU解析 -> IO调模型 -> 锁防重复 -> 状态ready
Reference image path: ../_images/A02_os_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 03_A03_mysql_flow

- Source: `../_images/A03_mysql_flow.png`
- Output: `../_images/image2_A03_mysql_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A03 MySQL：文档、会话和日志如何落库
Subtitle: 从事务、索引、锁、MVCC 到日志表治理
Exact step order: 上传文档 -> 开启事务 -> 写document -> 写task -> Worker加锁 -> 写chunk -> 向量chunk_id -> 会话消息 -> 调用日志 -> 慢查询优化
Reference image path: ../_images/A03_mysql_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 04_A04_redis_flow

- Source: `../_images/A04_redis_flow.png`
- Output: `../_images/image2_A04_redis_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A04 Redis：高并发问答如何省钱抗压
Subtitle: 缓存、限流、排行榜、分布式锁一次串起来
Exact step order: 用户提问 -> 额度限流 -> 查问答缓存 -> 缓存命中 -> 未命中RAG -> 写缓存TTL -> 热门ZSet -> 文件去重 -> 分布式锁 -> 释放锁
Reference image path: ../_images/A04_redis_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 05_A05_mq_flow

- Source: `../_images/A05_mq_flow.png`
- Output: `../_images/image2_A05_mq_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A05 消息队列：500页PDF如何异步入库
Subtitle: Producer、Topic、Consumer、重试、死信、幂等
Exact step order: 上传PDF -> 写uploaded -> 发送parse -> Broker持久化 -> Parse消费 -> 切chunk -> 发送embed -> Embedding消费 -> 写向量库 -> 失败重试 -> 死信收口
Reference image path: ../_images/A05_mq_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 06_A06_distributed_flow

- Source: `../_images/A06_distributed_flow.png`
- Output: `../_images/image2_A06_distributed_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A06 分布式：1万人同时用AI系统怎么稳住
Subtitle: 注册发现、负载均衡、限流、熔断、降级、追踪
Exact step order: 用户高峰 -> 负载均衡 -> 服务发现 -> RAG集群 -> 模型网关 -> Redis限流 -> 熔断检测 -> 降级兜底 -> trace追踪 -> 灰度配置
Reference image path: ../_images/A06_distributed_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 07_A07_algo_flow

- Source: `../_images/A07_algo_flow.png`
- Output: `../_images/image2_A07_algo_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: A07 算法：从100万个chunk找Top5
Subtitle: 数组、哈希、Set、堆、排序、树、图、滑窗
Exact step order: 问题token -> 向量数组 -> 召回候选 -> Hash映射 -> Set去重 -> 堆TopK -> Rerank排序 -> 树状文档 -> DAG流程 -> 滑窗裁剪
Reference image path: ../_images/A07_algo_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 08_B01_language_flow

- Source: `../_images/B01_language_flow.png`
- Output: `../_images/image2_B01_language_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: B01 编程语言：Java/Python/Go怎样分工
Subtitle: 企业后端、AI Pipeline、高并发网关各司其职
Exact step order: 业务主系统 -> Java后端 -> 集合与线程池 -> JVM内存 -> AI原型 -> Python脚本 -> async调用 -> Go网关 -> 统一API -> 项目交付
Reference image path: ../_images/B01_language_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 09_B02_web_flow

- Source: `../_images/B02_web_flow.png`
- Output: `../_images/image2_B02_web_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: B02 Web框架：/chat/stream请求怎么走
Subtitle: Filter、Controller、Service、Client、SSE、异常处理
Exact step order: HTTP请求 -> Filter追踪 -> 鉴权拦截 -> Controller -> 参数校验 -> Service编排 -> RAG服务 -> 模型Client -> SSE返回 -> 异常处理
Reference image path: ../_images/B02_web_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 10_B03_api_flow

- Source: `../_images/B03_api_flow.png`
- Output: `../_images/image2_B03_api_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: B03 API设计：知识库系统接口怎么串
Subtitle: 资源、统一响应、错误码、异步任务、流式接口
Exact step order: 创建知识库 -> 上传文档 -> 返回task_id -> 查询状态 -> ready判断 -> 发起问答 -> SSE流式 -> 历史消息 -> 统一错误 -> 版本控制
Reference image path: ../_images/B03_api_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 11_B04_auth_flow

- Source: `../_images/B04_auth_flow.png`
- Output: `../_images/image2_B04_auth_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: B04 认证鉴权：财务知识库如何不越权
Subtitle: 认证身份、RBAC/ABAC、metadata filter、工具鉴权
Exact step order: 用户登录 -> 解析JWT -> 查询角色 -> 判断ACL -> 构造filter -> 权限内检索 -> 引用复核 -> 工具白名单 -> 资源鉴权 -> 审计日志
Reference image path: ../_images/B04_auth_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 12_B05_test_flow

- Source: `../_images/B05_test_flow.png`
- Output: `../_images/image2_B05_test_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: B05 测试工程：Prompt改了怎么敢上线
Subtitle: 单测、Mock、接口、召回、回归、安全、压测
Exact step order: 修改Prompt -> 单元测试 -> Mock模型 -> 接口测试 -> 召回评测 -> Prompt回归 -> 安全样例 -> 压测成本 -> 灰度发布 -> BadCase闭环
Reference image path: ../_images/B05_test_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 13_C01_ml_flow

- Source: `../_images/C01_ml_flow.png`
- Output: `../_images/image2_C01_ml_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: C01 机器学习：训练客服意图识别器
Subtitle: 监督学习、分类、数据集、过拟合、Precision/Recall
Exact step order: 收集问题 -> 人工标注 -> 划分数据集 -> 训练分类器 -> 验证调参 -> 测试评估 -> 看P/R/F1 -> 补badcase -> 上线分类 -> 分流处理
Reference image path: ../_images/C01_ml_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 14_C02_dl_nlp_flow

- Source: `../_images/C02_dl_nlp_flow.png`
- Output: `../_images/image2_C02_dl_nlp_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: C02 深度学习与NLP：简历如何结构化
Subtitle: 分词、向量、神经网络、NER、摘要、GPU
Exact step order: 简历文本 -> Tokenizer -> 向量表示 -> 前向传播 -> 实体识别 -> JSON输出 -> 损失函数 -> 反向传播 -> 优化器 -> GPU推理
Reference image path: ../_images/C02_dl_nlp_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 15_C03_transformer_embedding_flow

- Source: `../_images/C03_transformer_embedding_flow.png`
- Output: `../_images/image2_C03_transformer_embedding_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: C03 Transformer与Embedding：语义检索为什么能懂同义问法
Subtitle: Token、Attention、Embedding、相似度、ANN、RAG
Exact step order: 用户问题 -> Tokenizer -> 位置编码 -> SelfAttention -> Query向量 -> Chunk向量 -> 相似度 -> ANN TopK -> 拼Prompt -> GPT生成
Reference image path: ../_images/C03_transformer_embedding_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 16_D01_model_call_flow

- Source: `../_images/D01_model_call_flow.png`
- Output: `../_images/image2_D01_model_call_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: D01 模型调用：一次LLM请求的后端全链路
Subtitle: messages、参数、超时、流式、token、日志
Exact step order: 用户问题 -> 参数校验 -> 组装messages -> 设置参数 -> ModelClient -> 首token超时 -> 接收delta -> SSE转发 -> 统计token -> 保存日志
Reference image path: ../_images/D01_model_call_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 17_D02_prompt_flow

- Source: `../_images/D02_prompt_flow.png`
- Output: `../_images/image2_D02_prompt_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: D02 Prompt工程：让模型基于资料回答
Subtitle: 角色、任务、上下文、约束、格式、版本、校验
Exact step order: 用户问题 -> 检索资料 -> 填模板 -> 角色任务 -> 约束拒答 -> 输出格式 -> 调用模型 -> 校验引用 -> 记录版本 -> 返回答案
Reference image path: ../_images/D02_prompt_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 18_D03_rag_flow

- Source: `../_images/D03_rag_flow.png`
- Output: `../_images/image2_D03_rag_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: D03 RAG：从文档入库到在线问答
Subtitle: 解析、chunk、Embedding、检索、Rerank、生成、引用
Exact step order: 上传文档 -> 解析清洗 -> 切Chunk -> Embedding -> 写向量库 -> 用户提问 -> Query改写 -> 混合检索 -> Rerank -> LLM引用回答
Reference image path: ../_images/D03_rag_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 19_D04_tool_agent_flow

- Source: `../_images/D04_tool_agent_flow.png`
- Output: `../_images/image2_D04_tool_agent_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: D04 Tool/Agent/Workflow：报销单状态怎么查
Subtitle: 意图分类、工具调用、鉴权、Workflow、Agent约束
Exact step order: 用户询问 -> 意图分类 -> 选择工具 -> 生成参数 -> Schema校验 -> 后端鉴权 -> 调用业务API -> 返回结果 -> 模型整理 -> 人工确认
Reference image path: ../_images/D04_tool_agent_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 20_D05_memory_multimodal_flow

- Source: `../_images/D05_memory_multimodal_flow.png`
- Output: `../_images/image2_D05_memory_multimodal_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: D05 多轮记忆与多模态：追问加发票图片
Subtitle: 会话、历史摘要、OCR、结构化抽取、多模态RAG
Exact step order: 第一轮问题 -> 保存会话 -> RAG回答 -> 用户追问 -> 解析上下文 -> 上传图片 -> OCR识别 -> 结构化抽取 -> 检索制度 -> 判断返回
Reference image path: ../_images/D05_memory_multimodal_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 21_E01_gateway_stability_flow

- Source: `../_images/E01_gateway_stability_flow.png`
- Output: `../_images/image2_E01_gateway_stability_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: E01 模型网关与稳定性：强模型超时怎么办
Subtitle: 路由、配额、重试、熔断、降级、成本观测
Exact step order: 业务请求 -> 模型网关 -> 配额检查 -> 模型路由 -> 健康判断 -> 调用模型 -> 超时重试 -> 熔断降级 -> 返回兜底 -> 记录成本
Reference image path: ../_images/E01_gateway_stability_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 22_E02_data_vector_flow

- Source: `../_images/E02_data_vector_flow.png`
- Output: `../_images/image2_E02_data_vector_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: E02 数据工程与向量库：制度更新如何不答旧答案
Subtitle: 版本、清洗、metadata、索引、灰度、删除旧向量
Exact step order: 新版文档 -> 计算版本 -> 解析清洗 -> 切Chunk -> 写metadata -> 生成向量 -> 新索引 -> 灰度切换 -> 删除旧向量 -> 质量检查
Reference image path: ../_images/E02_data_vector_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 23_E03_eval_llmops_flow

- Source: `../_images/E03_eval_llmops_flow.png`
- Output: `../_images/image2_E03_eval_llmops_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: E03 评测与LLMOps：怎么证明RAG变好了
Subtitle: Golden Dataset、Recall@K、Judge、灰度、BadCase闭环
Exact step order: 收集问题 -> 构建评测集 -> 标注答案 -> 跑旧策略 -> 跑新策略 -> 检索指标 -> 生成评估 -> 成本延迟 -> 灰度上线 -> BadCase回流
Reference image path: ../_images/E03_eval_llmops_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 24_E04_security_flow

- Source: `../_images/E04_security_flow.png`
- Output: `../_images/image2_E04_security_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: E04 安全合规：恶意Prompt如何防
Subtitle: 输入检测、权限过滤、工具鉴权、脱敏、审计
Exact step order: 恶意输入 -> 风险检测 -> 认证身份 -> ACL查询 -> filter检索 -> 工具白名单 -> 资源鉴权 -> 输出脱敏 -> 拒答/返回 -> 审计日志
Reference image path: ../_images/E04_security_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 25_E05_deploy_cost_flow

- Source: `../_images/E05_deploy_cost_flow.png`
- Output: `../_images/image2_E05_deploy_cost_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: E05 部署推理与成本：10万次调用怎么优化
Subtitle: API/自部署、推理服务、GPU、缓存、模型分级、灰度
Exact step order: 调用增长 -> 分析日志 -> 成本延迟 -> API评估 -> 自部署评估 -> 推理服务 -> Docker/K8s -> 模型分级 -> 缓存压缩 -> 灰度回滚
Reference image path: ../_images/E05_deploy_cost_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 26_F01_project_flow

- Source: `../_images/F01_project_flow.png`
- Output: `../_images/image2_F01_project_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: F01 主项目：企业知识库问答怎么讲
Subtitle: 背景、入库、问答、工程难点、评测闭环
Exact step order: 项目背景 -> 创建知识库 -> 上传文档 -> 异步入库 -> 向量ready -> 用户提问 -> 混合检索 -> 模型生成 -> 流式引用 -> 反馈优化
Reference image path: ../_images/F01_project_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 27_F02_project_pool_flow

- Source: `../_images/F02_project_pool_flow.png`
- Output: `../_images/image2_F02_project_pool_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: F02 备选项目库：项目组合怎么覆盖岗位能力
Subtitle: 知识库、客服、文档解析、代码问答各有侧重
Exact step order: 项目池 -> 知识库问答 -> 智能客服 -> 文档解析 -> 代码问答 -> RAG能力 -> Tool能力 -> OCR抽取 -> 代码检索 -> 差异化表达
Reference image path: ../_images/F02_project_pool_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 28_F03_system_design_flow

- Source: `../_images/F03_system_design_flow.png`
- Output: `../_images/image2_F03_system_design_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: F03 系统设计：面试题怎么按顺序回答
Subtitle: 需求澄清、架构、链路、存储、稳定性、安全、评测
Exact step order: 需求澄清 -> 总体架构 -> 入库链路 -> 问答链路 -> 存储设计 -> 高并发 -> 稳定性 -> 安全权限 -> 评测监控 -> 扩展优化
Reference image path: ../_images/F03_system_design_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```

## 29_F04_followup_flow

- Source: `../_images/F04_followup_flow.png`
- Output: `../_images/image2_F04_followup_flow.png`

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.

Title: F04 高频追问：所有追问都回到项目链路
Subtitle: MQ、RAG、权限、模型网关、成本、评测一图串起
Exact step order: 项目追问 -> 文档异步 -> MQ幂等 -> RAG不准 -> 权限过滤 -> 模型超时 -> 成本优化 -> 效果评测 -> 安全问题 -> 总结回链路
Reference image path: ../_images/F04_followup_flow.png
Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.
```
