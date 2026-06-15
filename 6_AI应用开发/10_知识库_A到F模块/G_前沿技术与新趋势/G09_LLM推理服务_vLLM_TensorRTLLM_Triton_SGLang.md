# G09 LLM 推理服务前沿：vLLM、TensorRT-LLM、Triton、SGLang

这一篇解决面试里非常容易追问的问题：

```text
你会调用大模型 API，那如果公司要私有化部署，怎么把模型服务做得又快又省？
```

普通后端同学容易只说：

```text
把模型加载起来，开个 HTTP 接口。
```

但真正的 LLM 推理服务像一个高铁站：

```text
请求不断进站
每个请求长度不同
有人只问一句，有人塞一整本制度
GPU 是昂贵轨道
KV Cache 是座位和行李
调度不好，GPU 就空等或者爆显存
```

所以这一篇重点看：

```text
PagedAttention
Continuous Batching
KV Cache / Prefix Cache
Speculative Decoding
Quantization
TensorRT-LLM
Triton
SGLang
Prefill/Decode 分离
```

---

## 1. 总览表

| 技术 | 旧方案痛点 | 新方案改进 | 优点 | 代价 | 项目里怎么用 |
|---|---|---|---|---|---|
| vLLM | 简单推理服务 GPU 利用率低 | PagedAttention、连续批处理、OpenAI 兼容服务 | 吞吐高，接入方便 | 版本变化快，调参复杂 | 私有化模型服务候选 |
| PagedAttention | KV Cache 连续分配浪费显存 | 像操作系统分页一样管理 KV Cache | 显存利用率高 | 调试理解成本高 | 支撑长上下文和高并发 |
| Continuous Batching | 静态 batch 等最慢请求 | 动态加入/移出请求 | GPU 更忙，吞吐更高 | 可能影响单请求延迟 | 高并发聊天服务 |
| Prefix Cache | 相同长前缀重复计算 | 复用 system prompt、工具 schema、固定模板 | 降低首 token 延迟和成本 | 前缀必须稳定 | 企业知识库固定系统提示词 |
| Speculative Decoding | 大模型逐 token 生成慢 | 小模型先草稿，大模型验证 | 降低生成延迟 | 需要合适 draft 模型 | 低延迟问答 |
| Quantization | FP16/BF16 显存贵 | INT8/INT4/FP8/GGUF 等压缩 | 降低显存和成本 | 可能损失效果 | 低成本部署和边缘部署 |
| TensorRT-LLM | 通用框架不够极致 | 面向 NVIDIA GPU 优化 LLM 推理 | 性能强 | 生态和硬件绑定更深 | 生产高性能推理 |
| Triton Inference Server | 每个模型服务方式不统一 | 统一部署、动态批处理、ensemble | 模型服务工程化强 | 配置复杂 | Embedding/Rerank/LLM 统一服务 |
| SGLang | Agent/结构化生成调度复杂 | RadixAttention、结构化输出、推理编排 | 适合复杂 LLM 程序 | 生态变化快 | Agent 和结构化生成实验 |
| Prefill/Decode 分离 | 长 prompt 和生成 token 混在一起抢资源 | 把预填充和解码分开调度 | 资源利用更细 | 架构复杂 | 大规模生产集群 |

---

## 2. LLM 推理分两段：Prefill 和 Decode

### 是什么

一次大模型回答可以拆成两段：

```text
Prefill：读完用户输入、system prompt、RAG chunk，建立 KV Cache
Decode：一个 token 一个 token 生成答案
```

### 新手理解

Prefill 像老师先读完试卷和材料。

Decode 像老师开始逐字写答案。

### 旧方案痛点

普通推理服务把所有请求混在一起处理：

```text
长 prompt 请求
短 prompt 请求
长答案请求
短答案请求
```

大家挤在同一条队伍里，GPU 调度容易乱。

### 新方案怎么改进

生产系统会区分：

```text
Prefill-heavy：输入长，适合高吞吐矩阵计算
Decode-heavy：逐 token 生成，受 KV Cache 和调度影响大
```

高级架构会考虑 Prefill/Decode 分离。

### 面试说法

```text
LLM 推理不是一个普通 HTTP 函数。它通常分为 prefill 和 decode 两个阶段，前者处理输入上下文并建立 KV Cache，后者逐 token 生成。优化首 token 延迟主要看 prefill，优化 tokens/s 和并发主要看 decode 调度和 KV Cache 管理。
```

---

## 3. KV Cache：模型的“草稿纸”

### 是什么

Transformer 生成 token 时，会缓存历史 token 的 Key/Value。

这样下一个 token 不必重新计算全部历史。

### 旧方案痛点

上下文越长，KV Cache 越大。

如果管理粗糙，就会出现：

```text
显存碎片
并发上不去
长上下文请求挤爆 GPU
```

### 新方案怎么改进

现代推理引擎会精细管理 KV Cache：

```text
分页管理
缓存复用
按请求动态释放
跨请求共享相同前缀
```

---

## 4. PagedAttention：把操作系统分页思想搬进 GPU 显存

### 是什么

PagedAttention 是 vLLM 的核心思想之一。

它借鉴操作系统虚拟内存分页：

```text
不用给每个请求提前分一大块连续显存
而是把 KV Cache 切成 block
按需分配和映射
```

### 旧方案痛点

传统 KV Cache 管理像给每个人订一整排座位：

```text
有人只坐 2 个座位
有人需要 200 个座位
提前分配会浪费
中途变化会碎片化
```

### 新方案怎么改进

PagedAttention 像把座位拆成小格子：

```text
请求需要多少，就拿多少 block
释放时归还 block
不同请求可以更灵活地共享显存
```

### 优点

- 提高显存利用率。
- 支持更多并发请求。
- 长上下文场景更友好。

### 缺点

- 引擎内部复杂。
- 问题定位比普通服务更难。
- 仍然需要监控显存、KV Cache 命中和 OOM。

---

## 5. Continuous Batching：别等一车人都到齐才发车

### 是什么

传统 batch 是静态的：

```text
凑齐一批请求
一起推理
等这批全部结束
再处理下一批
```

Continuous Batching 是动态的：

```text
某个请求生成结束，立刻把新请求塞进 batch
```

### 旧方案痛点

LLM 请求长短差异很大。

如果静态 batch 里有一个超长回答，其他短请求也要等。

### 新方案怎么改进

Continuous Batching 像地铁：

```text
有人下车
新人马上上车
车一直保持较高载客率
```

### 优点

- GPU 利用率更高。
- 系统吞吐更高。
- 适合高并发聊天。

### 缺点

- 调度更复杂。
- 单请求延迟可能受队列策略影响。
- 需要合理设置最大 batch、最大 token、超时。

---

## 6. Prefix Cache / Prompt Cache：不要反复读同一本说明书

### 是什么

很多请求前缀是重复的：

```text
system prompt
安全策略
输出 JSON schema
工具列表
企业知识库固定模板
```

Prefix Cache 会复用这些稳定前缀的计算结果。

### 旧方案痛点

每个请求都重新处理长 system prompt：

```text
贵
慢
浪费 GPU
```

### 新方案怎么改进

把稳定内容放前面：

```text
固定前缀：system prompt + tools schema + 输出格式
动态后缀：用户问题 + RAG chunk
```

这样更容易命中缓存。

### 项目落地

在 C++ AI Copilot 里，Prompt 模板要注意顺序：

```text
稳定内容放前面
用户问题和检索结果放后面
Prompt 版本变化要记录
监控 cache hit ratio
```

---

## 7. Speculative Decoding：先让小模型打草稿

### 是什么

Speculative Decoding 是推测解码。

流程：

```text
小模型快速生成几个候选 token
大模型一次性验证这些 token
验证通过就接受
不通过就回退
```

### 旧方案痛点

大模型一个 token 一个 token 生成，延迟高。

### 新方案怎么改进

让小模型像助教先写草稿，大模型像老师批改。

如果草稿经常正确，就能加速。

### 优点

- 降低生成延迟。
- 对用户感知的流式输出更友好。

### 缺点

- 需要合适的 draft 模型。
- 草稿命中率不高时收益有限。
- 系统复杂度上升。

### 面试说法

```text
Speculative Decoding 的核心是用小模型提前生成候选 token，再由大模型验证，从而减少大模型逐 token 解码次数。它适合对延迟敏感的生成场景，但需要评估 draft 模型质量和实际加速比。
```

---

## 8. Quantization：让模型变轻，但别把脑子压坏

### 是什么

量化是把模型权重或 KV Cache 从高精度变低精度。

常见形式：

```text
FP16 / BF16
FP8
INT8
INT4
GGUF
Quantized KV Cache
```

### 旧方案痛点

大模型显存占用高：

```text
模型权重占显存
KV Cache 占显存
batch 越大、上下文越长，显存压力越大
```

### 新方案怎么改进

量化降低每个参数或 cache 元素的位宽。

### 优点

- 更低显存。
- 更低部署成本。
- 小模型甚至可以 CPU/边缘设备运行。

### 缺点

- 可能损失效果。
- 不同硬件支持不同。
- 需要评测困惑度、任务准确率和真实业务回答质量。

### 项目里怎么用

```text
核心问答：优先质量，用 FP16/BF16 或高质量量化
低风险分类/改写：可以用更小模型或 INT8
边缘部署：考虑 GGUF/INT4
```

---

## 9. vLLM：私有化 LLM 服务的高频候选

### 是什么

vLLM 是常见的高吞吐 LLM 推理服务框架。

它提供：

```text
OpenAI-compatible server
PagedAttention
continuous batching
prefix caching
quantization
speculative decoding
structured outputs
multimodal inputs
metrics
```

### 项目里怎么接

C++ 后端不直接操纵 GPU。

更合理：

```text
C++ Model Gateway
  -> HTTP 调用 vLLM OpenAI-compatible endpoint
  -> 统一超时、重试、熔断、限流、日志
```

### 面试说法

```text
如果项目从第三方 API 演进到私有化部署，我会让 C++ 后端保留模型网关抽象，底层可以接 vLLM 这类 OpenAI 兼容推理服务。这样业务层不依赖具体模型服务，后续可以替换为 TensorRT-LLM、Triton 或云厂商模型。
```

---

## 10. TensorRT-LLM：更贴近 NVIDIA GPU 的极致优化

### 是什么

TensorRT-LLM 是 NVIDIA 面向 LLM 推理优化的工具链。

它关注：

```text
GPU kernel 优化
量化
并行
KV Cache
高吞吐推理
```

### 优点

- 在 NVIDIA GPU 上性能强。
- 适合生产级高性能服务。

### 缺点

- 学习和部署复杂。
- 和硬件、CUDA、驱动、模型结构关系更紧。
- 不适合秋招项目第一阶段就硬上。

### 面试定位

你不用把它说成“我已经精通”，可以这样说：

```text
我第一阶段会用 vLLM 快速私有化部署；如果生产环境对性能和成本要求更高，可以评估 TensorRT-LLM 这类更贴近 NVIDIA GPU 的推理优化方案。
```

---

## 11. Triton Inference Server：统一模型服务入口

### 是什么

Triton 是 NVIDIA 的模型推理服务框架。

它适合把不同模型统一服务化：

```text
Embedding 模型
Rerank 模型
分类模型
OCR 相关模型
LLM
```

### 核心能力

```text
dynamic batching
model repository
ensemble
metrics
多框架后端
```

### 项目里怎么用

在企业 AI Copilot 中，可以把非 LLM 的模型服务也统一起来：

```text
C++ API Server
  -> Embedding Service
  -> Rerank Service
  -> OCR/Classifier Service
  -> Triton
```

### 代价

- 配置和部署复杂。
- 小项目可能用不上。
- LLM 专用优化不一定比专用 LLM 引擎方便。

---

## 12. SGLang：面向复杂 LLM 程序的推理编排

### 是什么

SGLang 关注复杂 LLM 应用的执行和推理优化。

常被提到的能力包括：

```text
RadixAttention
structured output
parallel / constrained decoding
server runtime
```

### 旧方案痛点

复杂 Agent 或结构化生成里，很多请求共享前缀和中间状态。

如果每次都从头算，浪费明显。

### 新方案怎么改进

SGLang 试图让 LLM 程序更容易表达，并让运行时复用计算。

### 面试定位

```text
SGLang 这类框架说明 LLM 应用正在从“单次 prompt 调用”演进到“可调度、可复用、可约束的 LLM 程序”。秋招项目可以了解思想，不一定第一版落地。
```

---

## 13. 生产级指标

LLM 推理服务要看：

```text
TTFT：Time To First Token，首 token 延迟
TPOT：Time Per Output Token，每个输出 token 时间
tokens/s：生成吞吐
request throughput：请求吞吐
GPU utilization：GPU 利用率
KV Cache usage：KV Cache 使用率
prefix cache hit ratio：前缀缓存命中率
OOM rate：显存溢出率
timeout rate：超时率
cost per 1k tokens：单位 token 成本
```

### 面试说法

```text
模型慢不能只说“换更快的模型”。我会拆成首 token 延迟和生成速度来看：首 token 主要受输入长度、RAG chunk、prefill 和缓存影响；生成速度主要受 decode 调度、batching、KV Cache 和模型大小影响。生产上会监控 TTFT、TPOT、tokens/s、GPU 利用率和错误率。
```

---

## 14. C++ 企业 AI Copilot 推荐路线

```text
阶段 1：第三方 API，先跑通业务
阶段 2：模型网关抽象，统一日志、限流、超时
阶段 3：本地小模型或 vLLM，验证私有化
阶段 4：Embedding/Rerank 服务化
阶段 5：压测 TTFT、tokens/s、GPU 利用率
阶段 6：评估量化、prefix cache、speculative decoding
阶段 7：大规模生产再考虑 TensorRT-LLM、Triton、Prefill/Decode 分离
```

---

## 15. 面试总回答模板

```text
如果公司要从调用第三方 API 演进到私有化部署，我会先保留模型网关抽象，让 C++ 业务层不绑定具体推理引擎。底层可以接 vLLM 这类 OpenAI 兼容服务，利用 PagedAttention、Continuous Batching、Prefix Cache 提升吞吐和显存利用率；如果对 NVIDIA GPU 性能要求更高，再评估 TensorRT-LLM；如果要统一 Embedding、Rerank、分类等模型服务，可以考虑 Triton。优化时我会拆 TTFT、tokens/s、GPU 利用率、KV Cache 和成本，而不是笼统说“模型慢”。
```

---

## 16. 官方资料入口

- vLLM 文档：https://docs.vllm.ai/en/latest/
- vLLM Automatic Prefix Caching：https://docs.vllm.ai/en/latest/features/automatic_prefix_caching/
- vLLM Quantization：https://docs.vllm.ai/en/latest/features/quantization/
- vLLM Speculative Decoding：https://docs.vllm.ai/en/latest/features/spec_decode/
- NVIDIA TensorRT-LLM：https://nvidia.github.io/TensorRT-LLM/
- NVIDIA Triton Inference Server：https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/
- Triton Dynamic Batching：https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/user_guide/batcher.html
- SGLang 文档：https://docs.sglang.ai/
