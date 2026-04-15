# 2D Crossword String Matching

> **CSC10004 — String Matching Group Project**  
> Faculty of Information Technology, HCMUS — VNUHCM

A high-performance command-line tool for locating keywords inside a two-dimensional character grid. The program searches **horizontally** (left → right) and **vertically** (top → bottom), supporting five distinct string-matching algorithms with built-in benchmarking.

GitHub: https://github.com/benalexx007/String-Matching

---

## Table of Contents

- [Features](#features)
- [Algorithms](#algorithms)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Usage](#usage)
- [Input Format](#input-format)
- [Output Format](#output-format)
- [Benchmarking](#benchmarking)
- [Example](#example)
- [License](#license)

---

## Features

- **Five algorithms** — Brute Force, Rabin-Karp, KMP, Boyer-Moore, and Z-function
- **2D grid search** — scans every row (horizontal) and every column (vertical)
- **Performance metrics** — reports character comparisons and wall-clock execution time
- **Flexible CLI** — choose algorithm, input file, and output file via command-line flags
- **Extensive test suite** — pre-generated random and spiral datasets across multiple scales

---

## Algorithms

| Key  | Algorithm    | Time Complexity (Worst) | Space     |
|------|-------------|------------------------|-----------|
| `bf` | Brute Force | O(n × m)               | O(1)      |
| `rk` | Rabin-Karp  | O(n × m)               | O(1)      |
| `kmp`| KMP         | O(n + m)               | O(m)      |
| `bm` | Boyer-Moore | O(n × m + \|Σ\|)       | O(m + \|Σ\|) |
| `z`  | Z-function  | O(n + m)               | O(n + m)  |

> **n** = length of the text being searched, **m** = length of the pattern, **|Σ|** = alphabet size

---

## Project Structure

```
source/
├── file1.cpp                   # Main entry point (CLI argument parsing)
├── Algorithm/
│   ├── library.h               # Shared headers, data structures & declarations
│   ├── process.h               # Grid I/O, search orchestration & output formatting
│   ├── bf.cpp                  # Brute Force implementation
│   ├── rk.cpp                  # Rabin-Karp implementation
│   ├── kmp.cpp                 # Knuth-Morris-Pratt implementation
│   ├── bm.cpp                  # Boyer-Moore implementation
│   └── z.cpp                   # Z-function implementation
├── tests/
│   ├── Input/
│   │   ├── Scenario 1/         # Fixed keywords, varying grid size (10–1000)
│   │   │   ├── 10/             # 10×10 grids
│   │   │   │   ├── random.txt
│   │   │   │   └── spiral.txt
│   │   │   ├── 250/
│   │   │   ├── 500/
│   │   │   └── 1000/
│   │   └── Scenario 2/         # Fixed grid, varying keyword count (10–200)
│   │       ├── 10/
│   │       ├── 50/
│   │       ├── 100/
│   │       └── 200/
│   └── Output/                 # Pre-computed outputs per algorithm & scenario
│       ├── Scenario 1/{bf,bm,kmp,rk,z}/
│       └── Scenario 2/{bf,bm,kmp,rk,z}/
├── puzzle.txt                  # Sample input file
├── solution.txt                # Sample output file
└── README.md
```

---

## Prerequisites

- **Compiler:** GCC with C++17 support (`g++` ≥ 7)
- **OS:** Windows / Linux / macOS

---

## Building

Compile all source files into a single executable:

```bash
g++ file1.cpp Algorithm/bf.cpp Algorithm/bm.cpp Algorithm/kmp.cpp Algorithm/rk.cpp Algorithm/z.cpp -o crossword.exe -std=c++17
```

> On Linux/macOS, replace `crossword.exe` with `crossword`.

---

## Usage

```
crossword[.exe] -a <algorithm> -i <input_file> -o <output_file>
```

| Flag | Description | Required | Default | Values |
|------|-------------|----------|---------|--------|
| `-a` | Algorithm to use | ✅ | — | `bf`, `rk`, `kmp`, `bm`, `z` |
| `-i` | Path to input file | ❌ | `./puzzle.txt` | Any valid file path |
| `-o` | Path to output file | ❌ | `./solution.txt` | Any valid file path |

> **Note:** If `-i` and `-o` are omitted, the program defaults to reading from `puzzle.txt` and writing to `solution.txt` in the current directory.

**Examples:**

```bash
# Run with Brute Force
crossword.exe -a bf -i puzzle.txt -o solution.txt

# Run with KMP on a benchmark test
crossword.exe -a kmp -i "tests/Input/Scenario 1/250/random.txt" -o output.txt

# Run with Rabin-Karp on a spiral stress test
crossword.exe -a rk -i "tests/Input/Scenario 1/1000/spiral.txt" -o results.txt
```

> On Linux/macOS, replace `crossword.exe` with `./crossword`. On PowerShell, replace `crossword.exe` with `.\crossword.exe`.

---

## Input Format

```
R C
c₁₁ c₁₂ ... c₁C
c₂₁ c₂₂ ... c₂C
...
cR₁ cR₂ ... cRC
K
keyword₁
keyword₂
...
keywordK
```

| Field | Description |
|-------|-------------|
| `R C` | Number of rows and columns in the grid |
| Grid  | `R` lines, each containing `C` characters separated by spaces |
| `K`   | Number of keywords to search for |
| Keywords | `K` lines, one keyword per line |

**Example (`puzzle.txt`):**

```
4 4
t e s t
e a b c
s d e f
t g h i
2
test
bug
```

---

## Output Format

For each keyword, the program outputs its occurrences as coordinate pairs `(row_start, col_start) -> (row_end, col_end)`. If a keyword is not found, it prints `not found`. A summary section follows with algorithm name, total comparisons, and execution time.

```
test: (0, 0) -> (0, 3); (0, 0) -> (3, 0); 
bug: not found
-----------------------------------
Algorithm: Z-function
Comparisons: 20
Execution Time: 0.2555ms
```

> **Coordinates are 0-indexed.** Each match shows the starting cell and ending cell of the keyword in the grid.

---

## Benchmarking

The test suite includes two benchmark scenarios designed to evaluate algorithm scalability:

### Scenario 1 — Grid Scaling

Fixed dictionary size, grid dimensions vary: **10×10**, **250×250**, **500×500**, **1000×1000**

Two spatial distributions per size:
- `random.txt` — uniformly sampled printable ASCII (|Σ|=26), exercises average-case behavior
- `spiral.txt` — deterministic spiral fill with restricted alphabet (|Σ|≤4), near-worst-case stress test

### Scenario 2 — Dictionary Scaling

Fixed grid dimension, keyword count varies: **10**, **50**, **100**, **200**

Same dual distributions (`random` + `spiral`) per configuration.

### Key Findings (N = 1000)

| Algorithm    | Random Time (ms) | Random Cmp     | Spiral Time (ms) | Spiral Cmp       |
|-------------|----------------:|---------------:|------------------:|-----------------:|
| Brute Force | **1,259.58**    | 41,760,158     | 23,986.50         | 1,403,860,835    |
| KMP         | 17,851.00       | 790,355,694    | 20,787.90         | 958,899,717      |
| Z-function  | 40,651.30       | 31,712,405     | 56,148.80         | 351,933,902      |
| Boyer-Moore | 26,873.00       | 722,183,388    | 24,849.00         | 730,704,714      |
| Rabin-Karp  | 10,466.00       | **126,742**    | **10,107.60**     | **34,774,914**   |

> Brute Force dominates on small/random inputs due to zero preprocessing overhead. Rabin-Karp excels at scale thanks to its O(1) rolling-hash updates, achieving the fewest character comparisons across all configurations.

---

## Example

```bash
# 1. Build
g++ file1.cpp Algorithm/bf.cpp Algorithm/bm.cpp Algorithm/kmp.cpp Algorithm/rk.cpp Algorithm/z.cpp -o crossword.exe -std=c++17

# 2. Run
crossword.exe -a z -i puzzle.txt -o solution.txt
```

**Output:**

```
test: (0, 0) -> (0, 3); (0, 0) -> (3, 0); 
bug: not found
-----------------------------------
Algorithm: Z-function
Comparisons: 20
Execution Time: 0.2555ms
```

---

## License

This project was developed as part of the **CSC10004 — String Matching** course at Ho Chi Minh City University of Science (HCMUS), Vietnam National University.
