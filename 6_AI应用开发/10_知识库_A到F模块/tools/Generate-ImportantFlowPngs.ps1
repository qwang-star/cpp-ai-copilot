$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$Root = Split-Path -Parent $PSScriptRoot
$OutDir = Join-Path $Root "_images"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

function New-Color($hex) {
    $h = $hex.TrimStart("#")
    return [System.Drawing.Color]::FromArgb(
        [Convert]::ToInt32($h.Substring(0, 2), 16),
        [Convert]::ToInt32($h.Substring(2, 2), 16),
        [Convert]::ToInt32($h.Substring(4, 2), 16)
    )
}

function Draw-RoundedRect($g, $brush, [float]$x, [float]$y, [float]$w, [float]$h, [float]$r) {
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $d = $r * 2
    $path.AddArc($x, $y, $d, $d, 180, 90)
    $path.AddArc($x + $w - $d, $y, $d, $d, 270, 90)
    $path.AddArc($x + $w - $d, $y + $h - $d, $d, $d, 0, 90)
    $path.AddArc($x, $y + $h - $d, $d, $d, 90, 90)
    $path.CloseFigure()
    $g.FillPath($brush, $path)
    $path.Dispose()
}

function Draw-WrappedText($g, [string]$text, $font, $brush, [float]$x, [float]$y, [float]$maxWidth, [float]$lineHeight) {
    $chars = $text.ToCharArray()
    $line = ""
    $lines = New-Object System.Collections.Generic.List[string]
    foreach ($ch in $chars) {
        $try = $line + $ch
        if ($g.MeasureString($try, $font).Width -gt $maxWidth -and $line.Length -gt 0) {
            $lines.Add($line)
            $line = [string]$ch
        } else {
            $line = $try
        }
    }
    if ($line.Length -gt 0) { $lines.Add($line) }
    for ($i = 0; $i -lt [Math]::Min(2, $lines.Count); $i++) {
        $g.DrawString($lines[$i], $font, $brush, $x, $y + $i * $lineHeight)
    }
}

function New-FlowPng {
    param(
        [string]$FileName,
        [string]$Title,
        [string]$Subtitle,
        [string[]]$Steps,
        [string]$Accent = "#2563eb",
        [string]$Accent2 = "#16a34a"
    )

    $w = 1600
    $h = 980
    $bmp = New-Object System.Drawing.Bitmap $w, $h
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::ClearTypeGridFit

    $bg = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        (New-Object System.Drawing.Rectangle 0,0,$w,$h),
        (New-Color "#f8fafc"),
        (New-Color "#ecfeff"),
        [System.Drawing.Drawing2D.LinearGradientMode]::ForwardDiagonal
    )
    $g.FillRectangle($bg, 0, 0, $w, $h)

    $headBrush = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
        (New-Object System.Drawing.Rectangle 55,50,1490,126),
        (New-Color $Accent),
        (New-Color $Accent2),
        [System.Drawing.Drawing2D.LinearGradientMode]::Horizontal
    )
    Draw-RoundedRect $g $headBrush 55 50 1490 126 26

    $titleFont = New-Object System.Drawing.Font("Microsoft YaHei", 30, [System.Drawing.FontStyle]::Bold)
    $subFont = New-Object System.Drawing.Font("Microsoft YaHei", 17, [System.Drawing.FontStyle]::Regular)
    $white = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::White)
    $g.DrawString($Title, $titleFont, $white, 95, 85)
    $g.DrawString($Subtitle, $subFont, $white, 98, 132)

    $cardW = 310
    $cardH = 96
    $gapX = 70
    $gapY = 74
    $startX = 95
    $startY = 205
    $cols = 4
    $accentBrush = New-Object System.Drawing.SolidBrush (New-Color $Accent)
    $textBrush = New-Object System.Drawing.SolidBrush (New-Color "#0f172a")
    $mutedPen = New-Object System.Drawing.Pen (New-Color "#64748b"), 4
    $cardFont = New-Object System.Drawing.Font("Microsoft YaHei", 18, [System.Drawing.FontStyle]::Bold)
    $numFont = New-Object System.Drawing.Font("Segoe UI", 13, [System.Drawing.FontStyle]::Bold)

    for ($i = 0; $i -lt $Steps.Count; $i++) {
        $row = [Math]::Floor($i / $cols)
        $col = $i % $cols
        $x = $startX + $col * ($cardW + $gapX)
        $y = $startY + $row * ($cardH + $gapY)
        $cardBrush = New-Object System.Drawing.SolidBrush (if ($i % 2 -eq 0) { [System.Drawing.Color]::White } else { (New-Color "#fefce8") })
        Draw-RoundedRect $g $cardBrush $x $y $cardW $cardH 18
        $borderPen = New-Object System.Drawing.Pen (New-Color "#bfdbfe"), 2
        $g.DrawRectangle($borderPen, $x + 2, $y + 2, $cardW - 4, $cardH - 4)
        $g.FillEllipse($accentBrush, $x + 16, $y + 24, 48, 48)
        $num = "{0:00}" -f ($i + 1)
        $numSize = $g.MeasureString($num, $numFont)
        $g.DrawString($num, $numFont, $white, $x + 40 - $numSize.Width / 2, $y + 38)
        Draw-WrappedText $g $Steps[$i] $cardFont $textBrush ($x + 78) ($y + 28) 210 28

        if ($i -lt $Steps.Count - 1) {
            $nextRow = [Math]::Floor(($i + 1) / $cols)
            if ($nextRow -eq $row) {
                $x1 = $x + $cardW + 10
                $y1 = $y + $cardH / 2
                $x2 = $x + $cardW + $gapX - 16
                $g.DrawLine($mutedPen, $x1, $y1, $x2, $y1)
                $g.FillPolygon((New-Object System.Drawing.SolidBrush (New-Color "#64748b")), @(
                    (New-Object System.Drawing.PointF($x2, $y1)),
                    (New-Object System.Drawing.PointF($x2 - 12, $y1 - 8)),
                    (New-Object System.Drawing.PointF($x2 - 12, $y1 + 8))
                ))
            }
        }
    }

    $footBrush = New-Object System.Drawing.SolidBrush (New-Color "#0f172a")
    Draw-RoundedRect $g $footBrush 85 870 1430 60 18
    $footFont = New-Object System.Drawing.Font("Microsoft YaHei", 17, [System.Drawing.FontStyle]::Bold)
    $g.DrawString("阅读方式：先顺着流程图走一遍业务，再回到同名知识点文件查定义、机制、坑点和面试回答。", $footFont, $white, 110, 887)

    $path = Join-Path $OutDir $FileName
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $bmp.Dispose()
}

$diagrams = @(
    @{File="A01_network_flow.png"; Title="A01 网络：一次 AI 问答请求怎样穿过网络"; Sub="从 DNS、TCP、HTTPS、HTTP 到 SSE 流式返回"; Accent="#2563eb"; Accent2="#06b6d4"; Steps=@("用户提问","DNS解析","TCP握手","TLS加密","HTTP请求","负载均衡","RAG检索","模型调用","SSE流式","断开释放")},
    @{File="A02_os_flow.png"; Title="A02 操作系统：一份 PDF 如何被后台处理"; Sub="从进程、线程、系统调用到 Worker 线程池"; Accent="#7c3aed"; Accent2="#22c55e"; Steps=@("启动进程","请求线程","流式读文件","系统调用","写入存储","投递MQ","Worker线程池","CPU解析","IO调模型","锁防重复","状态ready")},
    @{File="A03_mysql_flow.png"; Title="A03 MySQL：文档、会话和日志如何落库"; Sub="从事务、索引、锁、MVCC 到日志表治理"; Accent="#0f766e"; Accent2="#f59e0b"; Steps=@("上传文档","开启事务","写document","写task","Worker加锁","写chunk","向量chunk_id","会话消息","调用日志","慢查询优化")},
    @{File="A04_redis_flow.png"; Title="A04 Redis：高并发问答如何省钱抗压"; Sub="缓存、限流、排行榜、分布式锁一次串起来"; Accent="#dc2626"; Accent2="#f97316"; Steps=@("用户提问","额度限流","查问答缓存","缓存命中","未命中RAG","写缓存TTL","热门ZSet","文件去重","分布式锁","释放锁")},
    @{File="A05_mq_flow.png"; Title="A05 消息队列：500页PDF如何异步入库"; Sub="Producer、Topic、Consumer、重试、死信、幂等"; Accent="#9333ea"; Accent2="#2563eb"; Steps=@("上传PDF","写uploaded","发送parse","Broker持久化","Parse消费","切chunk","发送embed","Embedding消费","写向量库","失败重试","死信收口")},
    @{File="A06_distributed_flow.png"; Title="A06 分布式：1万人同时用AI系统怎么稳住"; Sub="注册发现、负载均衡、限流、熔断、降级、追踪"; Accent="#1d4ed8"; Accent2="#16a34a"; Steps=@("用户高峰","负载均衡","服务发现","RAG集群","模型网关","Redis限流","熔断检测","降级兜底","trace追踪","灰度配置")},
    @{File="A07_algo_flow.png"; Title="A07 算法：从100万个chunk找Top5"; Sub="数组、哈希、Set、堆、排序、树、图、滑窗"; Accent="#0891b2"; Accent2="#84cc16"; Steps=@("问题token","向量数组","召回候选","Hash映射","Set去重","堆TopK","Rerank排序","树状文档","DAG流程","滑窗裁剪")},
    @{File="B01_language_flow.png"; Title="B01 编程语言：Java/Python/Go怎样分工"; Sub="企业后端、AI Pipeline、高并发网关各司其职"; Accent="#ea580c"; Accent2="#2563eb"; Steps=@("业务主系统","Java后端","集合与线程池","JVM内存","AI原型","Python脚本","async调用","Go网关","统一API","项目交付")},
    @{File="B02_web_flow.png"; Title="B02 Web框架：/chat/stream请求怎么走"; Sub="Filter、Controller、Service、Client、SSE、异常处理"; Accent="#4f46e5"; Accent2="#14b8a6"; Steps=@("HTTP请求","Filter追踪","鉴权拦截","Controller","参数校验","Service编排","RAG服务","模型Client","SSE返回","异常处理")},
    @{File="B03_api_flow.png"; Title="B03 API设计：知识库系统接口怎么串"; Sub="资源、统一响应、错误码、异步任务、流式接口"; Accent="#0ea5e9"; Accent2="#22c55e"; Steps=@("创建知识库","上传文档","返回task_id","查询状态","ready判断","发起问答","SSE流式","历史消息","统一错误","版本控制")},
    @{File="B04_auth_flow.png"; Title="B04 认证鉴权：财务知识库如何不越权"; Sub="认证身份、RBAC/ABAC、metadata filter、工具鉴权"; Accent="#be123c"; Accent2="#7c3aed"; Steps=@("用户登录","解析JWT","查询角色","判断ACL","构造filter","权限内检索","引用复核","工具白名单","资源鉴权","审计日志")},
    @{File="B05_test_flow.png"; Title="B05 测试工程：Prompt改了怎么敢上线"; Sub="单测、Mock、接口、召回、回归、安全、压测"; Accent="#475569"; Accent2="#16a34a"; Steps=@("修改Prompt","单元测试","Mock模型","接口测试","召回评测","Prompt回归","安全样例","压测成本","灰度发布","BadCase闭环")},
    @{File="C01_ml_flow.png"; Title="C01 机器学习：训练客服意图识别器"; Sub="监督学习、分类、数据集、过拟合、Precision/Recall"; Accent="#7c2d12"; Accent2="#ca8a04"; Steps=@("收集问题","人工标注","划分数据集","训练分类器","验证调参","测试评估","看P/R/F1","补badcase","上线分类","分流处理")},
    @{File="C02_dl_nlp_flow.png"; Title="C02 深度学习与NLP：简历如何结构化"; Sub="分词、向量、神经网络、NER、摘要、GPU"; Accent="#6d28d9"; Accent2="#0891b2"; Steps=@("简历文本","Tokenizer","向量表示","前向传播","实体识别","JSON输出","损失函数","反向传播","优化器","GPU推理")},
    @{File="C03_transformer_embedding_flow.png"; Title="C03 Transformer与Embedding：语义检索为什么能懂同义问法"; Sub="Token、Attention、Embedding、相似度、ANN、RAG"; Accent="#1e40af"; Accent2="#65a30d"; Steps=@("用户问题","Tokenizer","位置编码","SelfAttention","Query向量","Chunk向量","相似度","ANN TopK","拼Prompt","GPT生成")},
    @{File="D01_model_call_flow.png"; Title="D01 模型调用：一次LLM请求的后端全链路"; Sub="messages、参数、超时、流式、token、日志"; Accent="#2563eb"; Accent2="#f97316"; Steps=@("用户问题","参数校验","组装messages","设置参数","ModelClient","首token超时","接收delta","SSE转发","统计token","保存日志")},
    @{File="D02_prompt_flow.png"; Title="D02 Prompt工程：让模型基于资料回答"; Sub="角色、任务、上下文、约束、格式、版本、校验"; Accent="#9333ea"; Accent2="#14b8a6"; Steps=@("用户问题","检索资料","填模板","角色任务","约束拒答","输出格式","调用模型","校验引用","记录版本","返回答案")},
    @{File="D03_rag_flow.png"; Title="D03 RAG：从文档入库到在线问答"; Sub="解析、chunk、Embedding、检索、Rerank、生成、引用"; Accent="#0f766e"; Accent2="#2563eb"; Steps=@("上传文档","解析清洗","切Chunk","Embedding","写向量库","用户提问","Query改写","混合检索","Rerank","LLM引用回答")},
    @{File="D04_tool_agent_flow.png"; Title="D04 Tool/Agent/Workflow：报销单状态怎么查"; Sub="意图分类、工具调用、鉴权、Workflow、Agent约束"; Accent="#c2410c"; Accent2="#4f46e5"; Steps=@("用户询问","意图分类","选择工具","生成参数","Schema校验","后端鉴权","调用业务API","返回结果","模型整理","人工确认")},
    @{File="D05_memory_multimodal_flow.png"; Title="D05 多轮记忆与多模态：追问加发票图片"; Sub="会话、历史摘要、OCR、结构化抽取、多模态RAG"; Accent="#be185d"; Accent2="#0891b2"; Steps=@("第一轮问题","保存会话","RAG回答","用户追问","解析上下文","上传图片","OCR识别","结构化抽取","检索制度","判断返回")},
    @{File="E01_gateway_stability_flow.png"; Title="E01 模型网关与稳定性：强模型超时怎么办"; Sub="路由、配额、重试、熔断、降级、成本观测"; Accent="#1d4ed8"; Accent2="#dc2626"; Steps=@("业务请求","模型网关","配额检查","模型路由","健康判断","调用模型","超时重试","熔断降级","返回兜底","记录成本")},
    @{File="E02_data_vector_flow.png"; Title="E02 数据工程与向量库：制度更新如何不答旧答案"; Sub="版本、清洗、metadata、索引、灰度、删除旧向量"; Accent="#047857"; Accent2="#f59e0b"; Steps=@("新版文档","计算版本","解析清洗","切Chunk","写metadata","生成向量","新索引","灰度切换","删除旧向量","质量检查")},
    @{File="E03_eval_llmops_flow.png"; Title="E03 评测与LLMOps：怎么证明RAG变好了"; Sub="Golden Dataset、Recall@K、Judge、灰度、BadCase闭环"; Accent="#7c3aed"; Accent2="#16a34a"; Steps=@("收集问题","构建评测集","标注答案","跑旧策略","跑新策略","检索指标","生成评估","成本延迟","灰度上线","BadCase回流")},
    @{File="E04_security_flow.png"; Title="E04 安全合规：恶意Prompt如何防"; Sub="输入检测、权限过滤、工具鉴权、脱敏、审计"; Accent="#b91c1c"; Accent2="#334155"; Steps=@("恶意输入","风险检测","认证身份","ACL查询","filter检索","工具白名单","资源鉴权","输出脱敏","拒答/返回","审计日志")},
    @{File="E05_deploy_cost_flow.png"; Title="E05 部署推理与成本：10万次调用怎么优化"; Sub="API/自部署、推理服务、GPU、缓存、模型分级、灰度"; Accent="#0e7490"; Accent2="#84cc16"; Steps=@("调用增长","分析日志","成本延迟","API评估","自部署评估","推理服务","Docker/K8s","模型分级","缓存压缩","灰度回滚")},
    @{File="F01_project_flow.png"; Title="F01 主项目：企业知识库问答怎么讲"; Sub="背景、入库、问答、工程难点、评测闭环"; Accent="#2563eb"; Accent2="#22c55e"; Steps=@("项目背景","创建知识库","上传文档","异步入库","向量ready","用户提问","混合检索","模型生成","流式引用","反馈优化")},
    @{File="F02_project_pool_flow.png"; Title="F02 备选项目库：项目组合怎么覆盖岗位能力"; Sub="知识库、客服、文档解析、代码问答各有侧重"; Accent="#9333ea"; Accent2="#f59e0b"; Steps=@("项目池","知识库问答","智能客服","文档解析","代码问答","RAG能力","Tool能力","OCR抽取","代码检索","差异化表达")},
    @{File="F03_system_design_flow.png"; Title="F03 系统设计：面试题怎么按顺序回答"; Sub="需求澄清、架构、链路、存储、稳定性、安全、评测"; Accent="#1e3a8a"; Accent2="#0f766e"; Steps=@("需求澄清","总体架构","入库链路","问答链路","存储设计","高并发","稳定性","安全权限","评测监控","扩展优化")},
    @{File="F04_followup_flow.png"; Title="F04 高频追问：所有追问都回到项目链路"; Sub="MQ、RAG、权限、模型网关、成本、评测一图串起"; Accent="#9f1239"; Accent2="#2563eb"; Steps=@("项目追问","文档异步","MQ幂等","RAG不准","权限过滤","模型超时","成本优化","效果评测","安全问题","总结回链路")}
)

foreach ($d in $diagrams) {
    New-FlowPng -FileName $d.File -Title $d.Title -Subtitle $d.Sub -Steps $d.Steps -Accent $d.Accent -Accent2 $d.Accent2
}

Write-Host "Generated $($diagrams.Count) PNG flow diagrams in $OutDir"
