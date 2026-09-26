from pathlib import Path

import pdfplumber


FILES = {
    "2025": Path(r"C:\Users\24766\Desktop\2025赛事规则.自动分拣赛项-立体仓库项目.2025中国机器人大赛暨RoboCup机器人世界杯中国赛.pdf"),
    "2026": Path(r"C:\Users\24766\Desktop\2026赛事规则自动分拣机器人-立体仓库.pdf"),
}


for label, source in FILES.items():
    output = Path(__file__).with_name(f"rules_{label}.txt")
    with pdfplumber.open(source) as pdf:
        pages = []
        for number, page in enumerate(pdf.pages, start=1):
            text = page.extract_text(x_tolerance=2, y_tolerance=3) or ""
            pages.append(f"\n===== PDF PAGE {number} =====\n{text}\n")
    output.write_text("".join(pages), encoding="utf-8")
    print(f"{label}: {len(pages)} pages -> {output}")
