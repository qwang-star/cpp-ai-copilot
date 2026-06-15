# A03 数据库 MySQL

## 1. MySQL 在 AI 应用中的位置

### 是什么

MySQL 是关系型数据库，负责存储结构化业务数据。

### 在后端 AI 应用中存什么

- 用户。
- 租户。
- 知识库。
- 文档元数据。
- chunk 元数据。
- 会话。
- 消息。
- 模型调用日志。
- 工具调用日志。
- 评测结果。

### 不适合存什么

- 大文件原文：更适合对象存储。
- 高维向量相似度检索：更适合向量数据库。
- 极高频临时缓存：更适合 Redis。

## 2. InnoDB

### 是什么

InnoDB 是 MySQL 默认存储引擎，支持事务、行锁、崩溃恢复和 MVCC。

### 为什么重要

后端业务系统大多使用 InnoDB，因为它在一致性和并发方面更适合线上业务。

### AI 场景

文档状态更新、会话消息写入、用户额度扣减都需要事务保证一致性。

## 3. B+ 树索引

### 是什么

B+ 树是 MySQL InnoDB 常用索引结构。

特点：

- 多叉平衡树。
- 非叶子节点只存索引键。
- 叶子节点存数据或主键。
- 叶子节点之间有链表，适合范围查询。

### 为什么不用普通二叉树

磁盘 IO 很贵。B+ 树层高低，一次查询需要的磁盘 IO 少。

### 面试回答

```text
MySQL 使用 B+ 树主要是因为它层高低，能减少磁盘 IO；叶子节点有序并通过链表相连，范围查询效率高；非叶子节点只存 key，可以容纳更多索引项。
```

## 4. 聚簇索引

### 是什么

InnoDB 中，聚簇索引的叶子节点存整行数据。

通常主键索引就是聚簇索引。

### AI 场景

document 表按 document_id 查询时，如果 document_id 是主键，可以直接通过聚簇索引找到整行。

### 常见建议

- 主键尽量短。
- 主键尽量有序。
- 避免频繁更新主键。

## 5. 二级索引

### 是什么

二级索引的叶子节点存的是主键值，不是整行数据。

查询流程：

```text
二级索引找到主键
  -> 回表查聚簇索引
  -> 得到整行数据
```

### 常见问题：回表

如果查询字段不在二级索引中，需要回表，增加 IO。

### AI 场景

查询某知识库下文档：

```sql
select id, name, status
from document
where knowledge_base_id = ?
order by created_at desc
limit 20;
```

可以建联合索引：

```text
(knowledge_base_id, created_at)
```

## 6. 最左前缀原则

### 是什么

联合索引从最左列开始连续匹配才能充分使用。

索引：

```text
(tenant_id, knowledge_base_id, created_at)
```

可以命中：

- tenant_id
- tenant_id + knowledge_base_id
- tenant_id + knowledge_base_id + created_at

不容易命中：

- knowledge_base_id 单独查询。

### AI 场景

多租户系统常用：

```text
(tenant_id, user_id, created_at)
(tenant_id, knowledge_base_id, status)
```

因为几乎所有查询都要先限定租户。

## 7. 覆盖索引

### 是什么

查询需要的字段都在索引里，不需要回表。

### 优点

- 减少 IO。
- 提高查询速度。

### AI 场景

模型调用日志列表页只展示：

```text
id, model_name, latency_ms, cost, created_at
```

可以针对列表查询设计覆盖索引，但不要盲目把所有字段都塞进索引。

## 8. 索引下推

### 是什么

索引下推是 MySQL 在存储引擎层尽量用索引条件过滤数据，减少回表次数。

### 面试回答

```text
索引下推可以在遍历索引时先判断部分 where 条件，过滤不满足条件的记录，再回表，减少回表开销。
```

## 9. explain

### 是什么

explain 用来查看 SQL 执行计划。

重点字段：

- type。
- possible_keys。
- key。
- rows。
- Extra。

### 常见 type

从好到差大致：

```text
system / const / eq_ref / ref / range / index / ALL
```

### AI 场景

日志表、消息表、文档表数据增长快，要用 explain 查慢 SQL 是否走索引。

## 10. 事务 ACID

### 是什么

事务的四个特性：

- Atomicity：原子性。
- Consistency：一致性。
- Isolation：隔离性。
- Durability：持久性。

### AI 场景

文档上传：

```text
写 document 记录
写权限关系
写任务记录
投递 MQ
```

这些操作需要考虑一致性。如果 MQ 投递失败，要有本地消息表或补偿机制。

## 11. 隔离级别

### 四种隔离级别

- 读未提交。
- 读已提交。
- 可重复读。
- 串行化。

### 解决的问题

- 脏读。
- 不可重复读。
- 幻读。

### MySQL 默认

InnoDB 默认可重复读。

## 12. MVCC

### 是什么

MVCC 是多版本并发控制，让读写可以并发，提高性能。

核心组成：

- undo log。
- read view。
- 事务 id。
- 隐藏字段。

### 原理直觉

读操作不一定读最新版本，而是读当前事务可见的版本。

### 面试回答

```text
MVCC 通过 undo log 保存历史版本，并通过 Read View 判断哪个版本对当前事务可见。这样普通快照读不需要加锁，可以提高并发性能。
```

## 13. 锁

### 常见锁

- 表锁。
- 行锁。
- 记录锁。
- 间隙锁。
- 临键锁。
- 意向锁。

### AI 场景

- 用户额度扣减要防止并发超扣。
- 文档状态流转要防止重复处理。
- 任务领取要避免多个 Worker 同时处理。

### 例子

```sql
select * from document_task
where id = ?
for update;
```

用于锁定任务记录。

## 14. redo log

### 是什么

redo log 是重做日志，保证事务持久性。

### 作用

事务提交后，即使数据页还没刷盘，只要 redo log 持久化，崩溃后也能恢复。

## 15. undo log

### 是什么

undo log 是回滚日志。

作用：

- 事务回滚。
- MVCC 历史版本。

## 16. binlog

### 是什么

binlog 是 MySQL Server 层的逻辑日志，记录数据变更。

用途：

- 主从复制。
- 数据恢复。
- CDC 数据同步。

### AI 场景

文档表变更后，可以通过 binlog / CDC 触发索引更新或同步到搜索系统。

## 17. 主从复制

### 是什么

主库写 binlog，从库拉取并重放，实现数据复制。

### 用途

- 读写分离。
- 备份。
- 容灾。

### 常见问题

- 主从延迟。
- 读到旧数据。

### AI 场景

用户刚上传文档后马上查询状态，如果读从库可能读不到最新状态，要根据一致性要求读主库。

## 18. 分库分表

### 是什么

当单表或单库数据量太大时，把数据拆到多个表或库。

拆分方式：

- 垂直拆分。
- 水平拆分。

### AI 场景

模型调用日志、消息记录、评测结果可能增长很快，可以按租户或时间分表。

## 19. 慢查询优化

### 常见原因

- 没有索引。
- 索引失效。
- 返回数据太多。
- 排序 filesort。
- 深分页。
- 大字段回表。

### 优化方法

- explain 分析。
- 建合适索引。
- 避免 select *。
- 分页优化。
- 冷热分离。
- 归档历史日志。

## 20. AI 项目常见表设计

```text
knowledge_base
  id
  tenant_id
  name
  description
  created_by
  created_at

document
  id
  tenant_id
  knowledge_base_id
  name
  file_url
  status
  content_hash
  chunk_version
  created_at

document_chunk
  id
  tenant_id
  document_id
  chunk_index
  content
  token_count
  page
  section_title

conversation
  id
  tenant_id
  user_id
  title
  created_at

message
  id
  conversation_id
  role
  content
  created_at

model_call_log
  id
  trace_id
  user_id
  model_name
  prompt_version
  input_tokens
  output_tokens
  latency_ms
  cost
  created_at
```

