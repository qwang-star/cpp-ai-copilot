# C++ 企业级 AI Copilot 项目

这个文件夹是你的秋招主项目工作区。

项目目标：

```text
用 C++ 做一个企业级 AI 知识库与流程助手系统。
它既能基于企业文档做 RAG 问答，也能通过 Tool Calling 查询报销、请假、采购等业务流程。
```

它不是一个简单的“调大模型 API”Demo，而是一个能串起后端基础、AI 应用开发、工程化和面试表达的主项目。

## 推荐阅读顺序

1. [00_项目总览/01_项目一句话.md](00_项目总览/01_项目一句话.md)
2. [01_学习规划/01_我要学的东西总纲.md](01_学习规划/01_我要学的东西总纲.md)
3. [01_学习规划/02_两个月学习与项目推进计划.md](01_学习规划/02_两个月学习与项目推进计划.md)

## 项目最终形态

```text
用户上传企业制度文档
  -> C++ 后端接收文件
  -> MySQL 记录文档和任务状态
  -> MQ 异步解析文档
  -> 文档切分 chunk
  -> 调用 Embedding
  -> 写入向量库
  -> 用户提问
  -> 鉴权和权限过滤
  -> 混合检索 + Rerank
  -> 组装 Prompt
  -> 模型网关调用 LLM
  -> SSE 流式返回答案和引用
  -> Tool Calling 查询业务流程
  -> 记录日志、成本、评测和 Bad Case
```

## 技术栈建议

```text
C++20
CMake
Drogon / oat++ / Crow，优先 Drogon
MySQL
Redis
RabbitMQ
Qdrant / Milvus / FAISS
MinIO
nlohmann/json
spdlog
jwt-cpp
libcurl / cpr
Docker Compose
```

如果时间紧，第一版可以先用：

```text
C++20 + Drogon + MySQL + Redis + Qdrant + OpenAI/兼容模型 API
```

MQ、MinIO、评测平台可以作为第二阶段增强。

