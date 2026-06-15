# image2 流程图重绘提示词清单

当前目录已经生成了 29 张可用 PNG 流程图，保存在 `../_images/`。

如果后续可以调用 image2 / image_gen，可用下面统一提示词风格，把现有 PNG/SVG 作为内容蓝本重绘成更插画化的学习海报。

## 通用风格提示词

```text
Use case: infographic-diagram
Asset type: educational knowledge-base flowchart
Primary request: create a polished Chinese learning infographic flowchart for backend AI application development.
Style/medium: clean modern educational infographic, crisp vector-like shapes, soft depth, professional but lively.
Composition/framing: 16:9 landscape, left-to-right process flow, 10 rounded step cards, clear arrows, title header, footer note.
Color palette: fresh multi-color professional palette, not dominated by one hue, white cards, subtle background.
Text: keep all Chinese text verbatim from the provided steps; text must be sharp, readable, and not distorted.
Constraints: no watermark, no decorative clutter, no tiny unreadable labels, no misspelled Chinese, no overlapping text.
```

## 每张图的主题

- `A01_network_flow.png`：一次 AI 问答请求怎样穿过网络，DNS、TCP、HTTPS、HTTP、负载均衡、RAG、模型调用、SSE。
- `A02_os_flow.png`：一份 PDF 如何被后台处理，进程、线程、系统调用、线程池、CPU/IO、锁、状态流转。
- `A03_mysql_flow.png`：文档、会话和日志如何落库，事务、索引、行锁、chunk、日志。
- `A04_redis_flow.png`：高并发问答如何省钱抗压，限流、缓存、ZSet、去重、分布式锁。
- `A05_mq_flow.png`：500 页 PDF 如何异步入库，Producer、Broker、Consumer、重试、死信。
- `A06_distributed_flow.png`：1 万人同时用 AI 系统怎么稳住，负载均衡、服务发现、限流、熔断、降级、追踪。
- `A07_algo_flow.png`：从 100 万个 chunk 找 Top5，数组、哈希、Set、堆、排序、树、图、滑窗。
- `B01_language_flow.png`：Java/Python/Go 怎样分工。
- `B02_web_flow.png`：/chat/stream 请求怎么穿过 Web 框架。
- `B03_api_flow.png`：知识库系统 API 怎么串起来。
- `B04_auth_flow.png`：财务知识库如何不越权。
- `B05_test_flow.png`：Prompt 改了怎么敢上线。
- `C01_ml_flow.png`：训练客服意图识别器。
- `C02_dl_nlp_flow.png`：简历如何结构化。
- `C03_transformer_embedding_flow.png`：语义检索为什么能懂同义问法。
- `D01_model_call_flow.png`：一次 LLM 请求的后端全链路。
- `D02_prompt_flow.png`：让模型基于资料回答。
- `D03_rag_flow.png`：从文档入库到在线问答。
- `D04_tool_agent_flow.png`：报销单状态怎么查。
- `D05_memory_multimodal_flow.png`：追问加发票图片。
- `E01_gateway_stability_flow.png`：强模型超时怎么办。
- `E02_data_vector_flow.png`：制度更新如何不答旧答案。
- `E03_eval_llmops_flow.png`：怎么证明 RAG 变好了。
- `E04_security_flow.png`：恶意 Prompt 如何防。
- `E05_deploy_cost_flow.png`：10 万次调用怎么优化。
- `F01_project_flow.png`：企业知识库问答项目怎么讲。
- `F02_project_pool_flow.png`：项目组合怎么覆盖岗位能力。
- `F03_system_design_flow.png`：系统设计题怎么按顺序回答。
- `F04_followup_flow.png`：所有追问都回到项目链路。

