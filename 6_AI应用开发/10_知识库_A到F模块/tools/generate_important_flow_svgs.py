# -*- coding: utf-8 -*-
import json
from pathlib import Path
from xml.sax.saxutils import escape


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "_images"
OUT.mkdir(exist_ok=True)


def wrap_text(text: str, size: int = 12):
    return [text[i : i + size] for i in range(0, len(text), size)] or [""]


def make_svg(file_name, title, subtitle, steps, accent="#2563eb", accent2="#16a34a"):
    width, height = 1600, 980
    card_w, card_h = 310, 96
    gap_x, gap_y = 70, 74
    start_x, start_y = 95, 190
    cols = 4
    lines = [
        f"<svg xmlns='http://www.w3.org/2000/svg' width='{width}' height='{height}' viewBox='0 0 {width} {height}'>",
        "<defs>",
        "<linearGradient id='bg' x1='0' y1='0' x2='1' y2='1'><stop offset='0%' stop-color='#f8fafc'/><stop offset='55%' stop-color='#eef6ff'/><stop offset='100%' stop-color='#f7fee7'/></linearGradient>",
        f"<linearGradient id='head' x1='0' y1='0' x2='1' y2='0'><stop offset='0%' stop-color='{accent}'/><stop offset='100%' stop-color='{accent2}'/></linearGradient>",
        "<filter id='shadow' x='-20%' y='-20%' width='140%' height='150%'><feDropShadow dx='0' dy='10' stdDeviation='12' flood-color='#0f172a' flood-opacity='0.16'/></filter>",
        "<marker id='arrow' markerWidth='12' markerHeight='12' refX='10' refY='6' orient='auto'><path d='M2,2 L10,6 L2,10 Z' fill='#64748b'/></marker>",
        "</defs>",
        "<rect width='1600' height='980' rx='36' fill='url(#bg)'/>",
        "<rect x='55' y='50' width='1490' height='126' rx='26' fill='url(#head)' filter='url(#shadow)'/>",
        f"<text x='95' y='105' font-family='Microsoft YaHei, PingFang SC, Segoe UI, Arial, sans-serif' font-size='38' font-weight='800' fill='white'>{escape(title)}</text>",
        f"<text x='96' y='148' font-family='Microsoft YaHei, PingFang SC, Segoe UI, Arial, sans-serif' font-size='22' fill='white' opacity='0.92'>{escape(subtitle)}</text>",
    ]

    for i, text in enumerate(steps):
        row, col = divmod(i, cols)
        x = start_x + col * (card_w + gap_x)
        y = start_y + row * (card_h + gap_y)
        fill = "#ffffff" if i % 2 == 0 else "#fefce8"
        lines.extend(
            [
                "<g filter='url(#shadow)'>",
                f"<rect x='{x}' y='{y}' width='{card_w}' height='{card_h}' rx='18' fill='{fill}' stroke='#dbeafe' stroke-width='2'/>",
                f"<circle cx='{x + 38}' cy='{y + 48}' r='24' fill='{accent}' opacity='0.92'/>",
                f"<text x='{x + 38}' y='{y + 56}' text-anchor='middle' font-family='Segoe UI, Arial, sans-serif' font-size='17' font-weight='800' fill='white'>{i + 1:02d}</text>",
            ]
        )
        wrapped = wrap_text(text, 11)[:2]
        if len(wrapped) == 1:
            lines.append(
                f"<text x='{x + 76}' y='{y + 56}' font-family='Microsoft YaHei, PingFang SC, Segoe UI, Arial, sans-serif' font-size='23' font-weight='700' fill='#0f172a'>{escape(wrapped[0])}</text>"
            )
        else:
            lines.append(
                f"<text x='{x + 76}' y='{y + 42}' font-family='Microsoft YaHei, PingFang SC, Segoe UI, Arial, sans-serif' font-size='21' font-weight='700' fill='#0f172a'>{escape(wrapped[0])}</text>"
            )
            lines.append(
                f"<text x='{x + 76}' y='{y + 72}' font-family='Microsoft YaHei, PingFang SC, Segoe UI, Arial, sans-serif' font-size='21' font-weight='700' fill='#0f172a'>{escape(wrapped[1])}</text>"
            )
        lines.append("</g>")

        if i < len(steps) - 1:
            next_row, next_col = divmod(i + 1, cols)
            if next_row == row:
                x1, y1 = x + card_w + 10, y + card_h / 2
                x2, y2 = x + card_w + gap_x - 12, y1
                lines.append(
                    f"<line x1='{x1}' y1='{y1}' x2='{x2}' y2='{y2}' stroke='#64748b' stroke-width='4' stroke-linecap='round' marker-end='url(#arrow)'/>"
                )
            else:
                x1, y1 = x + card_w / 2, y + card_h + 14
                x2 = start_x + next_col * (card_w + gap_x) + card_w / 2
                y2 = start_y + next_row * (card_h + gap_y) - 16
                lines.append(
                    f"<path d='M {x1} {y1} C {x1} {y1 + 38}, {x2} {y2 - 38}, {x2} {y2}' fill='none' stroke='#64748b' stroke-width='4' stroke-linecap='round' marker-end='url(#arrow)'/>"
                )

    lines.extend(
        [
            "<rect x='85' y='870' width='1430' height='60' rx='18' fill='#0f172a' opacity='0.86'/>",
            "<text x='110' y='907' font-family='Microsoft YaHei, PingFang SC, Segoe UI, Arial, sans-serif' font-size='22' font-weight='700' fill='#f8fafc'>阅读方式：先顺着流程图走一遍业务，再回到同名知识点文件查定义、机制、坑点和面试回答。</text>",
            "</svg>",
        ]
    )
    (OUT / file_name).write_text("\n".join(lines), encoding="utf-8")


DIAGRAMS = [
    ("A01_network_flow.svg", "A01 网络：一次 AI 问答请求怎样穿过网络", "从 DNS、TCP、HTTPS、HTTP 到 SSE 流式返回", ["用户提问", "DNS解析", "TCP握手", "TLS加密", "HTTP请求", "负载均衡", "RAG检索", "模型调用", "SSE流式", "断开释放"], "#2563eb", "#06b6d4"),
    ("A02_os_flow.svg", "A02 操作系统：一份 PDF 如何被后台处理", "从进程、线程、系统调用到 Worker 线程池", ["启动进程", "请求线程", "流式读文件", "系统调用", "写入存储", "投递MQ", "Worker线程池", "CPU解析", "IO调模型", "锁防重复", "状态ready"], "#7c3aed", "#22c55e"),
    ("A03_mysql_flow.svg", "A03 MySQL：文档、会话和日志如何落库", "从事务、索引、锁、MVCC 到日志表治理", ["上传文档", "开启事务", "写document", "写task", "Worker加锁", "写chunk", "向量chunk_id", "会话消息", "调用日志", "慢查询优化"], "#0f766e", "#f59e0b"),
    ("A04_redis_flow.svg", "A04 Redis：高并发问答如何省钱抗压", "缓存、限流、排行榜、分布式锁一次串起来", ["用户提问", "额度限流", "查问答缓存", "缓存命中", "未命中RAG", "写缓存TTL", "热门ZSet", "文件去重", "分布式锁", "释放锁"], "#dc2626", "#f97316"),
    ("A05_mq_flow.svg", "A05 消息队列：500页PDF如何异步入库", "Producer、Topic、Consumer、重试、死信、幂等", ["上传PDF", "写uploaded", "发送parse", "Broker持久化", "Parse消费", "切chunk", "发送embed", "Embedding消费", "写向量库", "失败重试", "死信收口"], "#9333ea", "#2563eb"),
    ("A06_distributed_flow.svg", "A06 分布式：1万人同时用AI系统怎么稳住", "注册发现、负载均衡、限流、熔断、降级、追踪", ["用户高峰", "负载均衡", "服务发现", "RAG集群", "模型网关", "Redis限流", "熔断检测", "降级兜底", "trace追踪", "灰度配置"], "#1d4ed8", "#16a34a"),
    ("A07_algo_flow.svg", "A07 算法：从100万个chunk找Top5", "数组、哈希、Set、堆、排序、树、图、滑窗", ["问题token", "向量数组", "召回候选", "Hash映射", "Set去重", "堆TopK", "Rerank排序", "树状文档", "DAG流程", "滑窗裁剪"], "#0891b2", "#84cc16"),
    ("B01_language_flow.svg", "B01 编程语言：Java/Python/Go怎样分工", "企业后端、AI Pipeline、高并发网关各司其职", ["业务主系统", "Java后端", "集合与线程池", "JVM内存", "AI原型", "Python脚本", "async调用", "Go网关", "统一API", "项目交付"], "#ea580c", "#2563eb"),
    ("B02_web_flow.svg", "B02 Web框架：/chat/stream请求怎么走", "Filter、Controller、Service、Client、SSE、异常处理", ["HTTP请求", "Filter追踪", "鉴权拦截", "Controller", "参数校验", "Service编排", "RAG服务", "模型Client", "SSE返回", "异常处理"], "#4f46e5", "#14b8a6"),
    ("B03_api_flow.svg", "B03 API设计：知识库系统接口怎么串", "资源、统一响应、错误码、异步任务、流式接口", ["创建知识库", "上传文档", "返回task_id", "查询状态", "ready判断", "发起问答", "SSE流式", "历史消息", "统一错误", "版本控制"], "#0ea5e9", "#22c55e"),
    ("B04_auth_flow.svg", "B04 认证鉴权：财务知识库如何不越权", "认证身份、RBAC/ABAC、metadata filter、工具鉴权", ["用户登录", "解析JWT", "查询角色", "判断ACL", "构造filter", "权限内检索", "引用复核", "工具白名单", "资源鉴权", "审计日志"], "#be123c", "#7c3aed"),
    ("B05_test_flow.svg", "B05 测试工程：Prompt改了怎么敢上线", "单测、Mock、接口、召回、回归、安全、压测", ["修改Prompt", "单元测试", "Mock模型", "接口测试", "召回评测", "Prompt回归", "安全样例", "压测成本", "灰度发布", "BadCase闭环"], "#475569", "#16a34a"),
    ("C01_ml_flow.svg", "C01 机器学习：训练客服意图识别器", "监督学习、分类、数据集、过拟合、Precision/Recall", ["收集问题", "人工标注", "划分数据集", "训练分类器", "验证调参", "测试评估", "看P/R/F1", "补badcase", "上线分类", "分流处理"], "#7c2d12", "#ca8a04"),
    ("C02_dl_nlp_flow.svg", "C02 深度学习与NLP：简历如何结构化", "分词、向量、神经网络、NER、摘要、GPU", ["简历文本", "Tokenizer", "向量表示", "前向传播", "实体识别", "JSON输出", "损失函数", "反向传播", "优化器", "GPU推理"], "#6d28d9", "#0891b2"),
    ("C03_transformer_embedding_flow.svg", "C03 Transformer与Embedding：语义检索为什么能懂同义问法", "Token、Attention、Embedding、相似度、ANN、RAG", ["用户问题", "Tokenizer", "位置编码", "SelfAttention", "Query向量", "Chunk向量", "相似度", "ANN TopK", "拼Prompt", "GPT生成"], "#1e40af", "#65a30d"),
    ("D01_model_call_flow.svg", "D01 模型调用：一次LLM请求的后端全链路", "messages、参数、超时、流式、token、日志", ["用户问题", "参数校验", "组装messages", "设置参数", "ModelClient", "首token超时", "接收delta", "SSE转发", "统计token", "保存日志"], "#2563eb", "#f97316"),
    ("D02_prompt_flow.svg", "D02 Prompt工程：让模型基于资料回答", "角色、任务、上下文、约束、格式、版本、校验", ["用户问题", "检索资料", "填模板", "角色任务", "约束拒答", "输出格式", "调用模型", "校验引用", "记录版本", "返回答案"], "#9333ea", "#14b8a6"),
    ("D03_rag_flow.svg", "D03 RAG：从文档入库到在线问答", "解析、chunk、Embedding、检索、Rerank、生成、引用", ["上传文档", "解析清洗", "切Chunk", "Embedding", "写向量库", "用户提问", "Query改写", "混合检索", "Rerank", "LLM引用回答"], "#0f766e", "#2563eb"),
    ("D04_tool_agent_flow.svg", "D04 Tool/Agent/Workflow：报销单状态怎么查", "意图分类、工具调用、鉴权、Workflow、Agent约束", ["用户询问", "意图分类", "选择工具", "生成参数", "Schema校验", "后端鉴权", "调用业务API", "返回结果", "模型整理", "人工确认"], "#c2410c", "#4f46e5"),
    ("D05_memory_multimodal_flow.svg", "D05 多轮记忆与多模态：追问加发票图片", "会话、历史摘要、OCR、结构化抽取、多模态RAG", ["第一轮问题", "保存会话", "RAG回答", "用户追问", "解析上下文", "上传图片", "OCR识别", "结构化抽取", "检索制度", "判断返回"], "#be185d", "#0891b2"),
    ("E01_gateway_stability_flow.svg", "E01 模型网关与稳定性：强模型超时怎么办", "路由、配额、重试、熔断、降级、成本观测", ["业务请求", "模型网关", "配额检查", "模型路由", "健康判断", "调用模型", "超时重试", "熔断降级", "返回兜底", "记录成本"], "#1d4ed8", "#dc2626"),
    ("E02_data_vector_flow.svg", "E02 数据工程与向量库：制度更新如何不答旧答案", "版本、清洗、metadata、索引、灰度、删除旧向量", ["新版文档", "计算版本", "解析清洗", "切Chunk", "写metadata", "生成向量", "新索引", "灰度切换", "删除旧向量", "质量检查"], "#047857", "#f59e0b"),
    ("E03_eval_llmops_flow.svg", "E03 评测与LLMOps：怎么证明RAG变好了", "Golden Dataset、Recall@K、Judge、灰度、BadCase闭环", ["收集问题", "构建评测集", "标注答案", "跑旧策略", "跑新策略", "检索指标", "生成评估", "成本延迟", "灰度上线", "BadCase回流"], "#7c3aed", "#16a34a"),
    ("E04_security_flow.svg", "E04 安全合规：恶意Prompt如何防", "输入检测、权限过滤、工具鉴权、脱敏、审计", ["恶意输入", "风险检测", "认证身份", "ACL查询", "filter检索", "工具白名单", "资源鉴权", "输出脱敏", "拒答/返回", "审计日志"], "#b91c1c", "#334155"),
    ("E05_deploy_cost_flow.svg", "E05 部署推理与成本：10万次调用怎么优化", "API/自部署、推理服务、GPU、缓存、模型分级、灰度", ["调用增长", "分析日志", "成本延迟", "API评估", "自部署评估", "推理服务", "Docker/K8s", "模型分级", "缓存压缩", "灰度回滚"], "#0e7490", "#84cc16"),
    ("F01_project_flow.svg", "F01 主项目：企业知识库问答怎么讲", "背景、入库、问答、工程难点、评测闭环", ["项目背景", "创建知识库", "上传文档", "异步入库", "向量ready", "用户提问", "混合检索", "模型生成", "流式引用", "反馈优化"], "#2563eb", "#22c55e"),
    ("F02_project_pool_flow.svg", "F02 备选项目库：项目组合怎么覆盖岗位能力", "知识库、客服、文档解析、代码问答各有侧重", ["项目池", "知识库问答", "智能客服", "文档解析", "代码问答", "RAG能力", "Tool能力", "OCR抽取", "代码检索", "差异化表达"], "#9333ea", "#f59e0b"),
    ("F03_system_design_flow.svg", "F03 系统设计：面试题怎么按顺序回答", "需求澄清、架构、链路、存储、稳定性、安全、评测", ["需求澄清", "总体架构", "入库链路", "问答链路", "存储设计", "高并发", "稳定性", "安全权限", "评测监控", "扩展优化"], "#1e3a8a", "#0f766e"),
    ("F04_followup_flow.svg", "F04 高频追问：所有追问都回到项目链路", "MQ、RAG、权限、模型网关、成本、评测一图串起", ["项目追问", "文档异步", "MQ幂等", "RAG不准", "权限过滤", "模型超时", "成本优化", "效果评测", "安全问题", "总结回链路"], "#9f1239", "#2563eb"),
]


if __name__ == "__main__":
    for item in DIAGRAMS:
        make_svg(*item)
    config = []
    for file_name, title, subtitle, steps, accent, accent2 in DIAGRAMS:
        config.append(
            {
                "file": file_name.replace(".svg", ".png"),
                "title": title,
                "subtitle": subtitle,
                "steps": steps,
                "accent": accent,
                "accent2": accent2,
            }
        )
    (ROOT / "tools" / "diagram_config.json").write_text(
        json.dumps(
            {
                "footer": "阅读方式：先顺着流程图走一遍业务，再回到同名知识点文件查定义、机制、坑点和面试回答。",
                "diagrams": config,
            },
            ensure_ascii=True,
            indent=2,
        ),
        encoding="utf-8",
    )
    print(f"Generated {len(DIAGRAMS)} SVG flow diagrams in {OUT}")
