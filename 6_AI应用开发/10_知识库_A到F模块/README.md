# 后端 AI 应用开发知识库 A-G 模块

## 1. 这个目录解决什么问题

前面的资料偏“路线图”和“面试地图”，告诉你要学什么、怎么学。

这个目录偏“知识库”，目标是：

```text
不会哪个知识点，就能进来查：
它是什么、为什么重要、核心机制是什么、在后端 AI 应用里怎么用、面试怎么回答。
```

## 1.1 先从哪里看

如果你不知道从哪开始：

```text
想按知识点查：看 01_关键词速查索引.md
想先看图理解业务流程：看 02_流程图总览画廊.md
想按 A-G 系统复习：看 00_总索引.md
想确认当前完整度：看 03_完成度审计表.md
```

- [00_总索引.md](00_总索引.md)
- [01_关键词速查索引.md](01_关键词速查索引.md)
- [02_流程图总览画廊.md](02_流程图总览画廊.md)
- [03_完成度审计表.md](03_完成度审计表.md)

## 2. A-G 总模块

```text
A. 计算机与后端基础
   -> 网络、操作系统、数据库、Redis、MQ、分布式、算法

B. 后端开发工程能力
   -> 编程语言、Web 框架、API 设计、认证鉴权、测试、工程规范

C. AI 基础能力
   -> 机器学习、深度学习、NLP、Transformer、Embedding

D. 大模型应用开发
   -> 模型调用、Prompt、RAG、Tool Calling、Agent、Workflow、多轮记忆、多模态

E. AI 系统工程化
   -> 数据工程、向量库、模型网关、评测、成本、稳定性、安全、部署、LLMOps

F. 项目与面试表达
   -> 企业知识库问答、智能客服、文档解析、项目包装、系统设计题、八股题库

G. 前沿技术与新趋势
   -> QUIC、HTTP/3、WebTransport、io_uring、eBPF、OpenTelemetry、Hybrid RAG、GraphRAG、MCP、Structured Outputs、Prompt Caching、C++20 Coroutine、Boost.Asio、Drogon、uWebSockets、Gateway API、Envoy、Istio Ambient Mesh、OpenFeature、Agents SDK、A2A、Guardrails、OWASP LLM Top 10、NIST AI RMF、SLSA、Sigstore、向量检索调优、Late Interaction、vLLM、TensorRT-LLM、Triton、SGLang、WebRTC、Realtime API、WebGPU、端侧 AI、PII、ABAC、Confidential Computing
```

## 2.1 重要串联例子和流程图

每个知识文件旁边都有一个同目录的：

```text
！重要！一个例子串起来xxx.md
```

这些文件不是单独背概念，而是用一个具体业务场景把整章知识串起来。

每个重要文件顶部都嵌入了一张 PNG 流程图，图片统一保存在：

```text
_images/
```

当前有两类图片资产：

```text
_images/*.png  -> Markdown 默认引用的主图
_images/*.svg  -> 可编辑源图
```

另外准备了 image2 重绘提示词清单：

```text
tools/image2_prompts.md
tools/image2_batch_prompts.jsonl
tools/image2_batch_prompts_preview.md
tools/README_image2重绘说明.md
```

如果后续 image2 / image_gen 工具可用，可以用这些 PNG/SVG 作为内容蓝本，再按提示词重绘成更插画化的学习海报。

## 3. 每个知识点的固定结构

每个知识点尽量按这个模板写：

```text
是什么
为什么重要
核心机制
后端 AI 应用场景
常见坑
面试回答
关联知识
```

这样不是散装笔记，而是一套能查、能背、能串项目的知识库。

## 4. 当前文件

```text
00_总索引.md
A_计算机与后端基础/
  A01_网络.md
  A02_操作系统.md
  A03_数据库MySQL.md
  A04_Redis缓存.md
  A05_消息队列.md
  A06_分布式系统.md
  A07_数据结构与算法.md
B_后端开发工程能力/
  B01_编程语言.md
  B02_Web框架.md
  B03_API设计.md
  B04_认证鉴权.md
  B05_测试与工程规范.md
C_AI基础能力/
D_大模型应用开发/
E_AI系统工程化/
F_项目与面试表达/
G_前沿技术与新趋势/
```

## 5. 学习方法

第一遍：按 A 到 G 顺序读，建立骨架。

第二遍：每个知识点用自己的话复述“是什么 + AI 场景怎么用”。

第三遍：把 D/E/F/G 和自己的项目绑定，形成面试表达。G 模块重点回答“新技术为什么出现、比旧方案改进了什么、代价是什么”。
