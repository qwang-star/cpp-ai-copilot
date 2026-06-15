# -*- coding: utf-8 -*-
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
CONFIG = TOOLS / "diagram_config.json"
OUT_JSONL = TOOLS / "image2_batch_prompts.jsonl"
OUT_MD = TOOLS / "image2_batch_prompts_preview.md"


BASE_PROMPT = """Use case: infographic-diagram
Asset type: educational knowledge-base flowchart for backend AI application development
Primary request: Redraw the referenced flowchart as a polished Chinese learning infographic. Preserve the exact meaning, order, and short Chinese labels of the steps.
Style/medium: modern clean educational infographic, crisp vector-like cards, subtle depth, professional but lively.
Composition/framing: 16:9 landscape, title header at top, left-to-right process flow with clear numbered step cards and arrows, footer note optional.
Color palette: fresh professional multi-color palette, white cards, soft background, avoid a one-color theme.
Text constraints: Chinese text must be sharp, readable, correctly written, and not distorted. Keep labels short and legible.
Avoid: watermark, tiny unreadable labels, decorative clutter, overlapping text, misspelled Chinese, random extra steps.
"""


def main():
    data = json.loads(CONFIG.read_text(encoding="utf-8"))
    rows = []
    preview = ["# image2 批处理提示词预览", ""]

    for i, d in enumerate(data["diagrams"], start=1):
        image_path = f"../_images/{d['file']}"
        steps = " -> ".join(d["steps"])
        prompt = (
            BASE_PROMPT
            + f"\nTitle: {d['title']}\n"
            + f"Subtitle: {d['subtitle']}\n"
            + f"Exact step order: {steps}\n"
            + f"Reference image path: {image_path}\n"
            + "Important: use the reference image as the layout/content blueprint, but make the visual more polished and poster-like.\n"
        )
        row = {
            "id": f"{i:02d}_{Path(d['file']).stem}",
            "source_image": image_path,
            "output_suggestion": f"../_images/image2_{d['file']}",
            "title": d["title"],
            "steps": d["steps"],
            "prompt": prompt,
        }
        rows.append(row)
        preview.extend(
            [
                f"## {row['id']}",
                "",
                f"- Source: `{row['source_image']}`",
                f"- Output: `{row['output_suggestion']}`",
                "",
                "```text",
                prompt.strip(),
                "```",
                "",
            ]
        )

    OUT_JSONL.write_text(
        "\n".join(json.dumps(r, ensure_ascii=False) for r in rows) + "\n",
        encoding="utf-8",
    )
    OUT_MD.write_text("\n".join(preview), encoding="utf-8")
    print(f"Wrote {len(rows)} prompts to {OUT_JSONL}")
    print(f"Wrote preview to {OUT_MD}")


if __name__ == "__main__":
    main()
