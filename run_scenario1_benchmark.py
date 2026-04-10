import csv
import argparse
import os
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent
EXE_PATH = ROOT / f"crossword_benchmark_{os.getpid()}.exe"
CSV_PATH = ROOT / "Số liệu - Scenario 1.csv"
INPUT_ROOT = ROOT / "Benchmark Test" / "Input" / "Scenario 1"
OUTPUT_ROOT = ROOT / "Benchmark Test" / "Output" / "Scenario 1"

SIZES = [10, 250, 500, 1000]
MODES = ["random", "spiral"]
ALGORITHMS = [
    ("bf", "Brute force"),
    ("kmp", "KMP"),
    ("z", "Z-function"),
    ("bm", "Boyer-Moore"),
    ("rk", "Rabin Karp"),
]


def run_cmd(args):
    result = subprocess.run(args, cwd=ROOT, text=True, capture_output=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"Command failed: {' '.join(args)}\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}"
        )
    return result


def compile_executable():
    cpp_files = sorted((ROOT / "Algorithm").glob("*.cpp"))
    if not cpp_files:
        raise FileNotFoundError("Cannot find any .cpp files under Algorithm/")

    args = ["g++", "file1.cpp"] + [str(p) for p in cpp_files] + ["-o", str(EXE_PATH), "-std=c++17"]
    print(f"Compiling {EXE_PATH.name} ...")
    run_cmd(args)
    print("Compile done.")


def parse_metrics(output_file: Path):
    text = output_file.read_text(encoding="utf-8", errors="ignore")
    time_match = re.search(r"Execution Time:\s*([0-9]+(?:\.[0-9]+)?)ms", text)
    cmp_match = re.search(r"Comparisons:\s*([0-9]+)", text)
    if not time_match or not cmp_match:
        raise ValueError(f"Cannot parse metrics from: {output_file}")
    time_ms = float(time_match.group(1))
    comparisons = int(cmp_match.group(1))
    return time_ms, comparisons


def run_all_cases(selected_algorithms, selected_modes, selected_sizes):
    metrics = {}
    for algo_key, _algo_label in ALGORITHMS:
        if algo_key not in selected_algorithms:
            continue
        for mode in MODES:
            if mode not in selected_modes:
                continue
            for size in SIZES:
                if size not in selected_sizes:
                    continue
                input_path = INPUT_ROOT / str(size) / f"{mode}.txt"
                output_path = OUTPUT_ROOT / algo_key / str(size) / f"{mode}.txt"
                if not input_path.exists():
                    raise FileNotFoundError(f"Missing input file: {input_path}")

                output_path.parent.mkdir(parents=True, exist_ok=True)
                args = [
                    str(EXE_PATH),
                    "-a",
                    algo_key,
                    "-i",
                    str(input_path),
                    "-o",
                    str(output_path),
                ]
                print(f"Running {algo_key.upper()} | {mode:9} | {size}x{size}", flush=True)
                run_cmd(args)
                metrics[(algo_key, mode, size)] = parse_metrics(output_path)
    return metrics


def write_csv(metrics):
    rows = [
        ["", "", "10x10", "", "250x250", "", "500x500", "", "1000x1000", ""],
        ["", "", "Time", "Comparison", "Time", "Comparison", "Time", "Comparison", "Time", "Comparison"],
    ]

    for algo_key, algo_label in ALGORITHMS:
        for i, mode in enumerate(MODES):
            row = [algo_label if i == 0 else "", mode.capitalize()]
            for size in SIZES:
                if (algo_key, mode, size) in metrics:
                    time_ms, comparisons = metrics[(algo_key, mode, size)]
                    row.extend([f"{time_ms:.3f}", str(comparisons)])
                else:
                    row.extend(["", ""])
            rows.append(row)

    with CSV_PATH.open("w", newline="", encoding="utf-8-sig") as f:
        writer = csv.writer(f)
        writer.writerows(rows)


def main():
    try:
        parser = argparse.ArgumentParser(description="Run Scenario 1 benchmark and fill CSV.")
        parser.add_argument("--algos", nargs="*", default=[k for k, _ in ALGORITHMS], help="Algo keys: bf kmp z bm rk")
        parser.add_argument("--modes", nargs="*", default=MODES, help="Modes: random spiral")
        parser.add_argument("--sizes", nargs="*", type=int, default=SIZES, help="Sizes: 10 250 500 1000")
        parser.add_argument("--skip-csv", action="store_true", help="Run cases but do not rewrite CSV")
        args = parser.parse_args()

        valid_algo = {k for k, _ in ALGORITHMS}
        valid_mode = set(MODES)
        valid_size = set(SIZES)
        for a in args.algos:
            if a not in valid_algo:
                raise ValueError(f"Unknown algo key: {a}")
        for m in args.modes:
            if m not in valid_mode:
                raise ValueError(f"Unknown mode: {m}")
        for s in args.sizes:
            if s not in valid_size:
                raise ValueError(f"Unsupported size: {s}")

        compile_executable()
        metrics = run_all_cases(args.algos, args.modes, args.sizes)
        if not args.skip_csv:
            write_csv(metrics)
            print("Done. Updated CSV.")
        else:
            print("Done. CSV update skipped.")
    except Exception as exc:  # pylint: disable=broad-except
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
