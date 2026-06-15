# image2 重绘说明

## 当前状态

当前知识库已经有三层流程图资产：

```text
_images/*.png
  -> 当前 Markdown 默认引用的主流程图，可直接查看

_images/*.svg
  -> 可编辑源图，适合后续修改文字和结构

tools/image2_batch_prompts.jsonl
  -> 供 image2 / image_gen 后续批量重绘的提示词清单
```

## 为什么还保留 image2 清单

当前运行环境没有暴露可直接调用的 `image2/image_gen` 工具，所以无法真正让模型生成插画风位图。

但已经提前准备好了：

- 每张图的标题。
- 每张图的步骤顺序。
- 每张图对应的源 PNG。
- 每张图的 image2 重绘提示词。
- 建议输出文件名。

后续只要 image2 工具可用，就可以按 `image2_batch_prompts.jsonl` 逐条重绘。

## 文件说明

### image2_batch_prompts.jsonl

机器可读格式，一行一张图。

字段：

```text
id
source_image
output_suggestion
title
steps
prompt
```

### image2_batch_prompts_preview.md

人类可读预览版，方便你直接复制某一张图的 prompt。

### image2_prompts.md

主题级说明，列出 29 张图分别讲什么。

## 重绘要求

用 image2 重绘时，要特别注意：

- 中文文字必须正确。
- 步骤顺序不能变。
- 不要添加随机额外步骤。
- 不要把图画得太花，学习资料优先清晰。
- 输出建议仍放在 `_images/` 下，文件名前缀可用 `image2_`。

## 推荐重绘顺序

优先重绘这几张，因为最核心：

1. `D03_rag_flow.png`
2. `F01_project_flow.png`
3. `A05_mq_flow.png`
4. `E01_gateway_stability_flow.png`
5. `E03_eval_llmops_flow.png`

