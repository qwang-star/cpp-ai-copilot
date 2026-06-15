# -*- coding: utf-8 -*-
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
CONFIG = TOOLS / "diagram_config.json"
JSONL = TOOLS / "image2_batch_prompts.jsonl"
OUT = TOOLS / "asset_audit_report.md"


MODULE_DIRS = [
    ROOT / "A_计算机与后端基础",
    ROOT / "B_后端开发工程能力",
    ROOT / "C_AI基础能力",
    ROOT / "D_大模型应用开发",
    ROOT / "E_AI系统工程化",
    ROOT / "F_项目与面试表达",
]


def main():
    config = json.loads(CONFIG.read_text(encoding="utf-8"))
    prompts = [json.loads(line) for line in JSONL.read_text(encoding="utf-8").splitlines() if line.strip()]
    prompt_by_file = {Path(row["source_image"]).name: row for row in prompts}

    important_files = list(ROOT.glob("*/！重要！*.md"))
    normal_files = []
    for d in MODULE_DIRS:
        normal_files.extend([p for p in d.glob("*.md") if not p.name.startswith("！重要！")])

    rows = []
    failures = []
    for d in config["diagrams"]:
        png = ROOT / "_images" / d["file"]
        svg = ROOT / "_images" / d["file"].replace(".png", ".svg")
        prompt_ok = d["file"] in prompt_by_file
        matching_important = [p for p in important_files if d["file"] in p.read_text(encoding="utf-8", errors="ignore")]
        row = {
            "title": d["title"],
            "png": png.exists(),
            "svg": svg.exists(),
            "important": bool(matching_important),
            "prompt": prompt_ok,
        }
        rows.append(row)
        if not all([row["png"], row["svg"], row["important"], row["prompt"]]):
            failures.append(row)

    report = [
        "# asset audit report",
        "",
        "## summary",
        "",
        f"- normal knowledge files: {len(normal_files)}",
        f"- important example files: {len(important_files)}",
        f"- configured diagrams: {len(config['diagrams'])}",
        f"- image2 prompt rows: {len(prompts)}",
        f"- failures: {len(failures)}",
        "",
        "## per diagram",
        "",
        "| title | png | svg | important md references png | image2 prompt |",
        "|---|---:|---:|---:|---:|",
    ]
    for row in rows:
        report.append(
            f"| {row['title']} | {row['png']} | {row['svg']} | {row['important']} | {row['prompt']} |"
        )

    if failures:
        report.extend(["", "## failures", ""])
        for row in failures:
            report.append(f"- {row['title']}: {row}")
    else:
        report.extend(["", "## result", "", "All configured diagrams have PNG, SVG, important Markdown reference, and image2 prompt rows."])

    OUT.write_text("\n".join(report) + "\n", encoding="utf-8")
    print(f"normal={len(normal_files)} important={len(important_files)} diagrams={len(config['diagrams'])} prompts={len(prompts)} failures={len(failures)}")


if __name__ == "__main__":
    main()
