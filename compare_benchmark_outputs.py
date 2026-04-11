"""
So sánh kết quả tìm kiếm giữa các thuật toán trên output benchmark.
Bỏ qua phần footer (thời gian, số so sánh). Chuẩn hóa thứ tự các match trùng lặp.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent

if hasattr(sys.stdout, "reconfigure"):
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except (OSError, ValueError):
        pass

ALGO_KEYS = ["bf", "kmp", "z", "bm", "rk"]

# Mỗi đoạn "(r, c) -> (r, c)"
SEG_RE = re.compile(
    r"\(\s*(\d+)\s*,\s*(\d+)\s*\)\s*->\s*\(\s*(\d+)\s*,\s*(\d+)\s*\)"
)


def split_body(text: str) -> str:
    """Phần kết quả theo từng từ khóa (không gồm footer metrics)."""
    if "---" in text:
        text = text.split("---", 1)[0]
    lines = []
    for line in text.splitlines():
        if line.strip().startswith("---"):
            break
        lines.append(line)
    return "\n".join(lines).strip()


def parse_result_lines(body: str) -> list[tuple[str, str]]:
    """Mỗi dòng 'keyword: ...' theo đúng thứ tự trong file (giữ trùng keyword)."""
    out: list[tuple[str, str]] = []
    for raw in body.splitlines():
        raw = raw.strip()
        if not raw or ":" not in raw:
            continue
        key, rest = raw.split(":", 1)
        out.append((key.strip(), rest.strip()))
    return out


def normalize_answer(rest: str) -> tuple:
    """Chuẩn hóa để so sánh: not found hoặc tuple các segment đã sort."""
    if rest == "not found":
        return ("NF",)
    segs = SEG_RE.findall(rest)
    if not segs:
        return ("EMPTY", rest)
    normalized = []
    for a, b, c, d in segs:
        normalized.append((int(a), int(b), int(c), int(d)))
    return ("OK", tuple(sorted(normalized)))


def stats_from_lines(lines: list[tuple[str, str]]) -> tuple[int, int]:
    found = sum(1 for _k, v in lines if v != "not found")
    nf = sum(1 for _k, v in lines if v == "not found")
    return found, nf


def compare_files(paths: dict[str, Path]) -> list[str]:
    """Trả về danh sách mô tả lỗi (rỗng nếu khớp). So khớp theo từng dòng (cùng thứ tự từ điển)."""
    bodies = {k: split_body(p.read_text(encoding="utf-8", errors="ignore")) for k, p in paths.items()}
    lists = {k: parse_result_lines(bodies[k]) for k in paths}

    ref = ALGO_KEYS[0]
    nref = len(lists[ref])
    for k in ALGO_KEYS[1:]:
        if len(lists[k]) != nref:
            return [f"Số dòng kết quả khác: {ref}={nref}, {k}={len(lists[k])}"]

    errs: list[str] = []
    for i in range(nref):
        word_ref, rest_ref = lists[ref][i]
        nref_a = normalize_answer(rest_ref)
        for other in ALGO_KEYS[1:]:
            word_o, rest_o = lists[other][i]
            if word_ref != word_o:
                errs.append(f"  dòng {i + 1}: keyword '{word_ref}' != '{other}': '{word_o}'")
                continue
            if nref_a != normalize_answer(rest_o):
                errs.append(f"  dòng {i + 1} '{word_ref}': {ref} != {other}")
    return errs


def run_scenario1() -> bool:
    base = ROOT / "Benchmark Test" / "Output" / "Scenario 1"
    sizes = [10, 250, 500, 1000]
    modes = ["random", "spiral"]
    ok_all = True
    print("=== Scenario 1 ===\n")
    for size in sizes:
        for mode in modes:
            paths = {a: base / a / str(size) / f"{mode}.txt" for a in ALGO_KEYS}
            if not all(p.exists() for p in paths.values()):
                print(f"[SKIP] {size}x{size} {mode}: thiếu file output")
                ok_all = False
                continue
            errs = compare_files(paths)
            ref_lines = parse_result_lines(split_body(paths["bf"].read_text(encoding="utf-8", errors="ignore")))
            found, nf = stats_from_lines(ref_lines)
            total = found + nf
            pct_f = 100.0 * found / total if total else 0.0
            pct_nf = 100.0 * nf / total if total else 0.0
            tag = "OK" if not errs else "FAIL"
            print(f"{tag} | {size}x{size} | {mode:7} | keywords={total} | found={found} ({pct_f:.1f}%) | not_found={nf} ({pct_nf:.1f}%)")
            if errs:
                ok_all = False
                for e in errs[:20]:
                    print(e)
                if len(errs) > 20:
                    print(f"  ... và {len(errs) - 20} lỗi khác")
            print()
    return ok_all


def run_scenario2() -> bool:
    base = ROOT / "Benchmark Test" / "Output" / "Scenario 2"
    ks = [10, 50, 100, 200]
    modes = ["random", "spiral"]
    ok_all = True
    print("=== Scenario 2 ===\n")
    for k in ks:
        for mode in modes:
            paths = {a: base / a / str(k) / f"{mode}.txt" for a in ALGO_KEYS}
            if not all(p.exists() for p in paths.values()):
                print(f"[SKIP] K={k} {mode}: thiếu file output")
                ok_all = False
                continue
            errs = compare_files(paths)
            ref_lines = parse_result_lines(split_body(paths["bf"].read_text(encoding="utf-8", errors="ignore")))
            found, nf = stats_from_lines(ref_lines)
            total = found + nf
            pct_f = 100.0 * found / total if total else 0.0
            pct_nf = 100.0 * nf / total if total else 0.0
            tag = "OK" if not errs else "FAIL"
            print(
                f"{tag} | K={k:3} | {mode:7} | keywords={total} | found={found} ({pct_f:.1f}%) | not_found={nf} ({pct_nf:.1f}%)"
            )
            if errs:
                ok_all = False
                for e in errs[:20]:
                    print(e)
                if len(errs) > 20:
                    print(f"  ... và {len(errs) - 20} lỗi khác")
            print()
    return ok_all


def main():
    ap = argparse.ArgumentParser(description="So sánh output các thuật toán benchmark.")
    ap.add_argument("--scenario", choices=["1", "2", "all"], default="all")
    args = ap.parse_args()

    ok = True
    if args.scenario in ("1", "all"):
        ok = run_scenario1() and ok
    if args.scenario in ("2", "all"):
        ok = run_scenario2() and ok

    if ok:
        print("Kết luận: Mọi cặp thuật toán cho cùng một test đều khớp (theo chuẩn hóa tập match).")
    else:
        print("Kết luận: Có ít nhất một test không khớp giữa các thuật toán.")
        raise SystemExit(1)


if __name__ == "__main__":
    main()
