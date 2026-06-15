# G02 高性能后端前沿：io_uring、eBPF、OpenTelemetry

这一篇讲三个现代后端工程里越来越重要的技术：

```text
io_uring：更高效地做 Linux 异步 IO
eBPF：不改业务代码也能在内核层观测和扩展
OpenTelemetry：统一 traces、metrics、logs 的可观测性标准
```

你可以把它们放进同一个故事里：

```text
用户说 AI 问答很慢
  -> OpenTelemetry 告诉你慢在哪条链路
  -> eBPF 告诉你内核、网络、系统调用层是不是有问题
  -> io_uring 是你在高性能 IO 场景下可以考虑的优化手段
```

---

## 1. 旧后端怎么做 IO

传统 Linux 后端常见 IO 模型：

```text
阻塞 IO
非阻塞 IO
select / poll
epoll
线程池
```

epoll 已经很强，它解决了：

```text
用少量线程管理大量连接
```

但在高性能文件 IO、网络 IO、存储设备延迟越来越低的场景下，仍然会遇到：

```text
系统调用次数多
用户态/内核态切换成本
提交 IO 和等待完成分散
异步文件 IO 能力不统一
```

这就是 io_uring 出现的背景。

---

## 2. io_uring 是什么

新手理解：

```text
io_uring 像用户态和内核态之间放了两个共享队列。
你把要做的 IO 任务放进提交队列，内核做完后把结果放进完成队列。
```

两个核心队列：

```text
SQ：Submission Queue，提交队列
CQ：Completion Queue，完成队列
```

传统方式像：

```text
我每次都跑去窗口说：帮我读一下这个文件。
窗口处理完，我再跑去问：好了没？
```

io_uring 像：

```text
我把一批任务放进托盘。
内核处理完以后，把结果放进另一个托盘。
```

优势：

```text
减少系统调用
支持批量提交
统一文件和网络等异步 IO
适合高并发、高吞吐、低延迟场景
```

代价：

```text
Linux 平台相关
编程复杂度更高
调试和安全边界更复杂
不是普通业务系统第一天就要用
```

在你的 C++ AI Copilot 项目里：

```text
第一版用普通 socket/框架即可。
如果未来有大量文件上传、向量索引文件读写、日志高吞吐落盘，再考虑 io_uring。
```

面试说法：

```text
我知道 epoll 适合大量网络连接管理，而 io_uring 更进一步，把 IO 提交和完成通过共享环形队列组织起来，减少系统调用和上下文切换，适合高性能异步文件和网络 IO。不过它是 Linux 相关能力，复杂度较高，我会在普通业务链路稳定后再针对高 IO 热点评估使用。
```

---

## 3. eBPF 是什么

新手理解：

```text
eBPF 像给 Linux 内核装了一套安全的“插件机制”。
你可以在不改内核源码、不重启系统的情况下，把小程序挂到内核事件上。
```

它可以挂在：

```text
网络包处理
系统调用
函数入口/出口
性能事件
安全检查点
```

为什么有用？

传统排查系统问题常常只能看应用日志：

```text
业务说模型请求慢
日志只告诉你“请求用了 5 秒”
但不知道慢在 DNS、TCP、TLS、网卡、内核队列、系统调用还是下游服务
```

eBPF 可以更靠近底层：

```text
看网络包延迟
看系统调用耗时
看进程和容器行为
看 TCP 重传
看内核丢包
```

优势：

```text
不改业务代码
观测粒度深
性能开销相对可控
适合网络、安全、性能分析
```

代价：

```text
需要 Linux 内核支持
需要理解内核事件
安全和权限要求高
学习曲线陡
生产使用要谨慎
```

在你的项目里怎么讲：

```text
如果线上用户反馈 AI 回答慢，我会先用 OpenTelemetry 看应用链路。如果发现不是业务代码慢，而是网络或系统调用层异常，可以用 eBPF 工具进一步观察 TCP 重传、连接延迟、系统调用耗时等底层指标。
```

---

## 4. OpenTelemetry 是什么

OpenTelemetry，简称 OTel，是厂商中立的可观测性框架。

官方文档把它定位为：

```text
用于生成、收集、处理、导出 traces、metrics、logs 的可观测性框架。
```

它在 2026 年 5 月成为 CNCF Graduated 项目，说明它已经是云原生可观测性的成熟标准之一。

三个核心信号：

```text
Trace：一次请求经过哪些服务，每段花了多久。
Metric：某个指标随时间怎么变化，比如 QPS、P95 延迟、错误率。
Log：具体事件和上下文，比如某次模型调用失败。
```

---

## 5. 为什么 AI 应用更需要 OpenTelemetry

普通后端一次请求可能是：

```text
API -> MySQL -> Redis -> 返回
```

AI Copilot 一次请求可能是：

```text
API Gateway
  -> Auth
  -> Chat Service
  -> Query Rewrite
  -> Embedding API
  -> Vector DB
  -> Rerank
  -> Prompt Build
  -> Model Gateway
  -> LLM Provider
  -> SSE Stream
  -> Message Store
```

如果用户说：

```text
为什么这次回答等了 12 秒？
```

你不能只看一行日志。

你要知道：

```text
Embedding 花了多久
向量检索花了多久
Rerank 花了多久
首 token 延迟多久
总 token 生成多久
哪个模型 provider 慢
是否命中 Prompt Cache
是否发生重试
```

这就是 OTel 的价值。

---

## 6. OpenTelemetry vs 传统日志

| 对比点 | 传统日志 | OpenTelemetry |
|---|---|---|
| 主要问题 | 记录发生了什么 | 记录请求完整路径和指标 |
| 数据类型 | 主要是 log | traces + metrics + logs |
| 跨服务关联 | 靠 trace_id 手动串 | 标准 trace/span 模型 |
| 厂商绑定 | 容易和某个平台绑定 | 厂商中立 |
| AI 应用价值 | 看错误文本 | 看模型调用、RAG、向量库完整耗时 |
| 代价 | 简单 | 接入和采样策略更复杂 |

---

## 7. 在 C++ AI Copilot 里怎么落地

第一阶段先做：

```text
日志
请求耗时
模型调用耗时
token 统计
错误码
```

第二阶段升级：

```text
trace_id
span_id
每个模块打点
导出 OpenTelemetry
接入 Collector
接 Prometheus/Grafana/Jaeger
```

一次聊天请求可以拆成这些 span：

```text
chat.request
  auth.check
  rag.query_embedding
  rag.vector_search
  rag.keyword_search
  rag.rerank
  prompt.build
  model_gateway.call
  sse.stream
  message.save
```

你面试可以这样讲：

```text
AI 应用的慢不只是数据库慢，可能是 embedding、向量库、rerank、模型首 token、模型生成速度、SSE 传输任何一段慢。所以我会用 trace 把一次用户问题拆成多个 span，用 metrics 统计 P95 延迟、错误率、token 成本、缓存命中率，用 logs 记录具体错误。这样优化不是凭感觉，而是知道瓶颈在哪。
```

---

## 8. 三者怎么组合

```text
OpenTelemetry：应用层全链路可观测，回答“慢在哪个服务/模块”
eBPF：系统和内核层可观测，回答“是不是网络、内核、容器、系统调用慢”
io_uring：高性能 IO 优化手段，回答“热点 IO 能不能更高效”
```

不要把它们混成一类。

它们的关系更像：

```text
OTel 是仪表盘
eBPF 是显微镜
io_uring 是发动机改造方案
```

---

## 9. 官方资料

- OpenTelemetry 文档：https://opentelemetry.io/docs/
- OpenTelemetry CNCF Graduated 公告：https://opentelemetry.io/blog/2026/otel-graduates/
- eBPF 官方介绍：https://ebpf.io/what-is-ebpf/
- Linux io_uring zero copy Rx 文档：https://docs.kernel.org/networking/iou-zcrx.html
- Linux Kernel 文档入口：https://docs.kernel.org/

