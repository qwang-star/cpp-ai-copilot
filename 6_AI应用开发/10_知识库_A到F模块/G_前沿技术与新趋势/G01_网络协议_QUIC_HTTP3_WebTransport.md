# G01 网络协议前沿：QUIC v1、HTTP/3、TLS 1.3、WebTransport

先把故事讲顺。

你做了一个企业 AI Copilot，用户在手机上问：

```text
我的报销单审批到哪了？
```

后端要做：

```text
鉴权
RAG 检索
Tool Calling 查报销单
调用大模型
流式返回答案
```

如果用户在地铁、电梯、跨国网络里访问，网络会抖、会丢包、会切 Wi-Fi/5G。这时传统 TCP + HTTP/2 的一些问题就会露出来，QUIC 和 HTTP/3 就是为这类现代网络体验做改进的。

---

## 1. 先把旧技术链路说清楚

传统 HTTPS 请求大概是：

```text
DNS
  -> TCP 三次握手
  -> TLS 握手
  -> HTTP/1.1 或 HTTP/2 请求
```

这里分三层：

```text
TCP：负责可靠传输
TLS：负责加密和身份认证
HTTP：负责请求/响应语义
```

HTTP/2 已经比 HTTP/1.1 先进很多，因为它支持：

```text
多路复用
二进制帧
Header 压缩
单连接承载多个请求
```

但 HTTP/2 仍然跑在 TCP 上。

这就带来一个核心问题：

```text
TCP 层有队头阻塞
```

## 2. 队头阻塞是什么

你可以把 TCP 想成一条单车道快递传送带。

HTTP/2 在上面放了很多小包：

```text
stream A 的包
stream B 的包
stream C 的包
```

HTTP/2 自己知道这些包属于不同 stream，但 TCP 不知道。TCP 只看到一串有顺序的字节：

```text
第 1 块
第 2 块
第 3 块
第 4 块
```

如果第 2 块丢了，TCP 为了保证字节流顺序，会让后面的第 3 块、第 4 块都等。

于是就出现：

```text
一个包丢了
  -> 整个 TCP 连接上的多个 HTTP/2 stream 都受影响
```

这就是 HTTP/3 想改的关键点之一。

---

## 3. QUIC v1 是什么

QUIC v1 是 RFC 9000 定义的传输协议。

新手理解：

```text
QUIC = 用 UDP 做底座，自己在用户态实现可靠传输、多路复用、拥塞控制、加密握手和连接迁移。
```

注意：

```text
QUIC 不是“UDP 所以不可靠”
QUIC 是“基于 UDP，但自己实现可靠传输”
```

为什么用 UDP？

因为 TCP 在操作系统内核里，升级很慢。QUIC 放在用户态，更容易迭代和部署。

QUIC 主要改进：

```text
1. 集成 TLS 1.3 握手。
2. 支持 0-RTT，老用户重连时可以更快发数据。
3. 每个 stream 独立处理丢包，减少 TCP 队头阻塞。
4. 用 Connection ID 支持连接迁移，手机从 Wi-Fi 切到 5G 不一定要重连。
5. 用户态实现，迭代速度比改 TCP 内核协议快。
```

---

## 4. HTTP/3 是什么

HTTP/3 是 RFC 9114 定义的 HTTP 语义到 QUIC 的映射。

你可以这样理解：

```text
HTTP/1.1：HTTP 语义跑在 TCP 上
HTTP/2：HTTP 语义 + 多路复用，仍然跑在 TCP 上
HTTP/3：HTTP 语义基本保留，但底层跑在 QUIC 上
```

HTTP/3 没有让 GET、POST、Header、状态码这些东西消失。

它主要改的是：

```text
底层传输从 TCP 换成 QUIC
```

所以面试里不要说：

```text
HTTP/3 是全新的 HTTP 语义
```

要说：

```text
HTTP/3 保留 HTTP 语义，但把传输层换成 QUIC，从而改善连接建立、队头阻塞和连接迁移问题。
```

---

## 5. TLS 1.3 在这里干什么

TLS 1.3 是 RFC 8446 定义的安全协议版本。

传统理解：

```text
TCP 先建连接
TLS 再做加密握手
HTTP 最后发请求
```

QUIC 的做法是：

```text
把 TLS 1.3 握手集成进 QUIC 握手
```

好处：

```text
减少握手往返
更快建立安全连接
默认加密更多传输元数据
```

但 0-RTT 有风险：

```text
0-RTT 数据可能被重放
```

所以它适合：

```text
幂等请求
读取类请求
可容忍重放的场景
```

不适合：

```text
支付
提交订单
修改权限
扣费
```

在你的 AI 项目里：

```text
普通知识库查询可以考虑 0-RTT
Tool Calling 里提交报销、修改审批状态不应该直接用 0-RTT 早期数据
```

---

## 6. HTTP/2 vs HTTP/3 对比

| 对比点 | HTTP/2 | HTTP/3 |
|---|---|---|
| 底层传输 | TCP | QUIC over UDP |
| 加密 | 常见是 TLS over TCP | QUIC 集成 TLS 1.3 |
| 多路复用 | 有，但受 TCP 队头阻塞影响 | 有，stream 级别更独立 |
| 丢包影响 | 一个 TCP 包丢失会拖住连接上的多个 stream | 一个 QUIC stream 丢包不必阻塞其他 stream |
| 连接迁移 | IP/端口变了通常要重连 | Connection ID 支持迁移 |
| 弱网体验 | 较容易受丢包影响 | 弱网和移动网络更友好 |
| 部署复杂度 | 生态成熟 | 需要 UDP 可达、代理/CDN/网关支持 |
| 排查难度 | 工具成熟 | 加密更多，抓包和排查更复杂 |

一句话：

```text
HTTP/3 不是为了替代所有 HTTP/2，而是为了在弱网、移动端、多 stream、高延迟场景下改善体验。
```

---

## 7. WebTransport 是什么

WebTransport 是 W3C 的 Web API，让浏览器可以使用类似 QUIC 的能力：

```text
可靠 stream
不可靠 datagram
低延迟双向通信
```

你可以把它和 WebSocket 对比：

| 对比点 | WebSocket | WebTransport |
|---|---|---|
| 通信方式 | 双向可靠字节流/消息 | 可靠 stream + 不可靠 datagram |
| 底层 | 通常基于 TCP/TLS | 面向 HTTP/3/QUIC，也可有 HTTP/2 fallback |
| 适合场景 | 聊天、通知、协作 | 实时游戏、音视频、低延迟多模态、部分数据可丢 |
| 成熟度 | 成熟、兼容好 | 更新，基础设施支持要评估 |
| 复杂度 | 相对简单 | 更复杂，协议和服务端支持要求高 |

在企业 AI Copilot 里：

```text
普通聊天流式输出：SSE 就够。
双向聊天和协作：WebSocket 可考虑。
低延迟多模态，比如语音、屏幕共享、实时标注：WebTransport 更有想象空间。
```

---

## 8. QUIC / HTTP/3 的缺点和坑

不要只说优点。

### 1. UDP 可能被网络设备限制

很多企业网络、防火墙、老代理对 UDP 支持不如 TCP 稳。

所以真实部署通常要：

```text
HTTP/3 优先
失败 fallback 到 HTTP/2
```

### 2. 排查更难

QUIC 默认加密更多内容，传统 TCP 抓包经验不完全适用。

### 3. 服务端和网关支持成本

你要确认：

```text
CDN 支不支持 HTTP/3
负载均衡支不支持 QUIC
反向代理支不支持 UDP 转发
监控系统能不能识别 HTTP/3 指标
```

### 4. 0-RTT 有重放风险

必须区分：

```text
幂等读请求
非幂等写请求
```

---

## 9. 在 C++ AI Copilot 项目里怎么讲

当前你的 C++ 项目第一版是：

```text
Winsock + HTTP/1.1 最小服务
```

这是学习底层链路用的。

真实生产化可以这样演进：

```text
第 1 阶段：HTTP/1.1 + SSE，先跑通聊天流式输出
第 2 阶段：接入成熟 Web 框架和反向代理，比如 Drogon / Nginx / Envoy
第 3 阶段：网关/CDN 开启 HTTP/2
第 4 阶段：移动端和弱网场景启用 HTTP/3 / QUIC
第 5 阶段：多模态实时交互探索 WebTransport
```

面试里可以这样说：

```text
我当前项目用 HTTP/1.1 + SSE 实现流式输出，因为它简单、兼容性好、足够支撑知识库问答。但我了解后续演进方向：如果移动端弱网和跨地域访问明显增加，可以在网关层支持 HTTP/3。HTTP/3 的核心是把 HTTP 语义映射到 QUIC 上，减少 TCP 队头阻塞，并通过 Connection ID 支持连接迁移。不过它也带来 UDP 可达性、代理支持和排查复杂度，所以我会做 HTTP/3 优先、HTTP/2 fallback，而不是盲目替换。
```

---

## 10. 官方资料

- QUIC v1 RFC 9000：https://www.rfc-editor.org/rfc/rfc9000
- HTTP/3 RFC 9114：https://www.rfc-editor.org/rfc/rfc9114
- TLS 1.3 RFC 8446：https://www.rfc-editor.org/rfc/rfc8446
- WebTransport W3C：https://www.w3.org/TR/webtransport/

