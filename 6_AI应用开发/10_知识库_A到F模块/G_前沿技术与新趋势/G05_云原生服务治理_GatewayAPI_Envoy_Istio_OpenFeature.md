# G05 云原生服务治理：Gateway API、Envoy、Istio Ambient Mesh、OpenFeature

这一篇讲 AI 后端上线之后会遇到的问题：

```text
服务越来越多
模型调用越来越贵
检索策略经常变
灰度和回滚越来越重要
网络流量要治理
功能开关要可控
```

基础知识里我们讲：

```text
Nginx
负载均衡
Kubernetes
Ingress
限流
熔断
灰度
```

前沿一点的云原生服务治理会继续往这些方向演进：

```text
Gateway API
Envoy
Istio Ambient Mesh
OpenFeature
```

---

## 1. 从 Ingress 到 Gateway API

### 旧方案：Ingress

Kubernetes 里常用 Ingress 暴露 HTTP 服务。

新手理解：

```text
Ingress 像 Kubernetes 集群门口的入口规则。
```

它能做：

```text
域名路由
路径路由
TLS 终止
```

但随着系统复杂，Ingress 的问题会变明显：

```text
表达能力有限
不同 Ingress Controller 注解不统一
网关能力靠各家 annotation 扩展
跨团队职责边界不清
```

### 新方案：Gateway API

Gateway API 是 Kubernetes 生态里更现代的入口流量 API。

它把角色拆得更清楚：

```text
GatewayClass：基础设施提供什么网关能力
Gateway：某个网关实例
HTTPRoute：具体路由规则
```

新手理解：

```text
Ingress 像一张简单门禁规则表。
Gateway API 像把“谁提供网关、谁管理网关、谁配置业务路由”分清楚。
```

### 优点

- 表达能力比 Ingress 更强。
- 跨团队职责更清楚。
- 更适合多租户、多网关、多协议场景。
- Kubernetes 官方生态方向。

### 缺点

- 生态仍在演进。
- 需要网关实现支持。
- 对新手比 Ingress 概念更多。

### AI Copilot 里怎么用

你可以这样讲：

```text
第一版项目用 Docker Compose 或简单 Nginx。
如果上 Kubernetes，早期可用 Ingress。
当服务拆成 Chat、RAG、Model Gateway、Tool Service，多团队管理入口流量时，可以考虑 Gateway API。
```

---

## 2. Envoy

### 是什么

Envoy 是高性能 L7 代理。

它常用于：

```text
API Gateway
Service Mesh sidecar
流量治理
负载均衡
TLS 终止
可观测性
HTTP/2 / HTTP/3 支持
```

新手理解：

```text
Envoy 像一个很强的智能交通岗，能看懂 HTTP/gRPC 等应用层流量，并做路由、重试、限流、观测。
```

### 相比 Nginx 的理解

Nginx 很成熟，适合：

```text
反向代理
静态资源
基础负载均衡
```

Envoy 更偏云原生服务间通信：

```text
动态配置
服务发现
gRPC
细粒度流量治理
服务网格
可观测性
```

不是谁绝对替代谁，而是场景不同。

### AI Copilot 里怎么用

```text
客户端 -> Envoy/Gateway -> Chat Service
Chat Service -> Model Gateway
Model Gateway -> LLM Provider
```

Envoy 可以帮你做：

```text
路由
限流
重试
超时
熔断
HTTP/2 / HTTP/3 支持
指标上报
```

面试说法：

```text
模型调用链路里超时、重试、限流和观测很重要，这些可以在业务模型网关里做，也可以部分下沉到 Envoy 这类 L7 代理。Envoy 更适合云原生动态流量治理，但配置和排查复杂度也更高。
```

---

## 3. Istio Ambient Mesh

### 旧 Service Mesh：Sidecar 模式

传统 Istio 常见模式是：

```text
每个业务 Pod 旁边注入一个 Envoy sidecar
```

好处：

```text
服务间 mTLS
流量治理
可观测性
熔断重试
```

问题：

```text
每个 Pod 多一个 sidecar，资源成本高
注入和升级复杂
应用和代理生命周期绑定
对新手不友好
```

### Ambient Mesh 怎么改

Ambient Mesh 尝试减少 sidecar 负担。

核心思路：

```text
把一部分能力下沉到节点级 ztunnel
需要 L7 能力时再走 waypoint proxy
```

新手理解：

```text
以前每个员工旁边都配一个保镖。
Ambient Mesh 更像楼层有统一安保，需要精细检查时再去专门检查点。
```

### 优点

- 降低 sidecar 注入成本。
- 对应用侵入更低。
- mTLS 和基础流量治理更易统一。

### 缺点

- 新架构复杂，需要理解 ztunnel/waypoint。
- 生态和经验还在积累。
- 并不是所有场景都需要 Service Mesh。

### AI Copilot 里怎么用

如果你的服务拆成很多：

```text
Chat Service
RAG Service
Model Gateway
Embedding Worker
Tool Service
Eval Service
```

服务间通信多、安全要求高，可以考虑 Service Mesh。

但第一版项目不需要。

面试说法：

```text
Service Mesh 适合服务很多、服务间治理复杂的场景。传统 sidecar 模式能力强但资源和运维成本高，Ambient Mesh 尝试降低 sidecar 注入成本。我的项目早期不会上 Mesh，等服务拆分和安全治理复杂后再评估。
```

---

## 4. OpenFeature

### 是什么

OpenFeature 是功能开关的开放标准。

新手理解：

```text
Feature Flag 像远程开关，不用重新发版就能打开或关闭某个功能。
```

AI 应用特别需要功能开关，因为你经常改：

```text
Prompt
模型版本
Rerank 策略
Hybrid Search 权重
是否启用 GraphRAG
是否启用某个 Tool
是否走强模型
```

### 旧方案痛点

把开关写死在代码里：

```text
改一次策略就要发版
出问题回滚慢
不能按用户/租户灰度
不能 A/B Test
```

### 新方案怎么改

功能开关可以按条件生效：

```text
租户
用户
部门
流量百分比
环境
模型版本
```

例如：

```text
5% 用户启用 Rerank v2
财务租户启用强模型
内部员工启用 GraphRAG 测试
```

### 优点

- 支持灰度。
- 支持快速回滚。
- 支持 A/B Test。
- 降低 AI 策略变更风险。

### 缺点

- 开关太多会变成配置地狱。
- 要管理开关生命周期。
- 要记录每次请求命中了哪个策略，否则评测和排查会乱。

### 项目里怎么用

你可以设计：

```text
feature_rerank_v2
feature_hybrid_search
feature_graph_rag
feature_strong_model
feature_prompt_v3
```

并在日志里记录：

```text
request_id
user_id
tenant_id
feature_flags
prompt_version
model_name
retrieval_strategy
```

面试说法：

```text
AI 应用变化的不只是代码，还有 Prompt、模型、检索策略和工具开关。我会用 feature flag 做灰度和回滚，比如只给 5% 用户启用新的 Rerank 策略，并在日志和评测里记录命中的策略版本，避免上线后无法定位效果变化。
```

---

## 5. 怎么串回项目

```text
用户请求
  -> Gateway API / Envoy 入口
  -> Chat Service
  -> Feature Flag 决定是否启用 Hybrid Search / Rerank v2
  -> RAG Service
  -> Model Gateway
  -> Envoy / Mesh 管理服务间超时、重试、mTLS
  -> OpenTelemetry 记录链路
```

第一版不需要全上。

学习顺序：

```text
Docker Compose
  -> Nginx / 基础反向代理
  -> Kubernetes 基础
  -> Ingress
  -> Gateway API
  -> Envoy
  -> Feature Flag
  -> Service Mesh
```

---

## 6. 官方资料

- Kubernetes Gateway API：https://gateway-api.sigs.k8s.io/
- Envoy：https://www.envoyproxy.io/docs/envoy/latest/
- Envoy HTTP/3 概览：https://www.envoyproxy.io/docs/envoy/latest/intro/arch_overview/http/http3
- Istio Ambient Mesh：https://istio.io/latest/docs/ambient/
- OpenFeature：https://openfeature.dev/docs/reference/intro/

