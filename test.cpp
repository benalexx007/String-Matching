#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

// Grid limits per assignment PDF: 1 ≤ R,C ≤ 1000. Official submissions often use K ≤ 100; our benchmark uses K up to 200.
static const int kMaxGridDim = 1000;
static const int kMaxDict = 200;

// Scenario 1: larger K hurts brute force more — each keyword scans the whole grid (horizontal + vertical),
// and each start can compare up to the keyword length → total comparisons ~ Θ(K · N² · L) with L ≈ N.
// Use the maximum benchmark K (200) to maximize total work within our input caps.
static const int kBenchScenario1K = kMaxDict;
static const int kBenchScenario2Grid = 250;

// Guarantee for "not found" keywords:
// We deliberately exclude kAbsentChar from every generated grid. Then every type=0 keyword
// will contain kAbsentChar, making it impossible to appear anywhere in the grid (horiz/vert).
static const char kAbsentChar = 'z';
static const char* kGridAlphabet = "abcdefghijklmnopqrstuvwxy"; // excludes 'z'

static int RAND(mt19937& rd, int l, int r) { return uniform_int_distribution<int>(l, r)(rd); }

vector<vector<char>> generateRandomGrid(int n, const string& alphabet, mt19937& rd) {
    vector<vector<char>> grid(n, vector<char>(n));
    const int A = (int)alphabet.size() - 1;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            grid[i][j] = alphabet[RAND(rd, 0, A)];
    return grid;
}

vector<vector<char>> generateSpiralGrid(int n, const string& alphabet, mt19937& rd) {
    // Spiral mode should be measurably different from random for horizontal/vertical searches.
    // We fill the grid in spiral layers, but each layer uses a fixed character (cycling over a
    // small subset of the alphabet). This creates long repeated runs in rows/cols, which tends
    // to be a bad case for naive brute force (many long partial matches).
    vector<vector<char>> grid(n, vector<char>(n));

    const int cycle = min<int>(4, (int)alphabet.size()); // small cycle to amplify repetition
    int top = 0, bottom = n - 1, left = 0, right = n - 1;
    int layer = 0;
    while (top <= bottom && left <= right) {
        char ch = alphabet[layer % cycle];

        for (int j = left; j <= right; j++)
            grid[top][j] = ch;
        top++;

        for (int i = top; i <= bottom; i++)
            grid[i][right] = ch;
        right--;

        if (top <= bottom) {
            for (int j = right; j >= left; j--)
                grid[bottom][j] = ch;
            bottom--;
        }

        if (left <= right) {
            for (int i = bottom; i >= top; i--)
                grid[i][left] = ch;
            left++;
        }

        layer++;
    }

    // Add tiny noise so different seeds still differ, but keep the spiral structure dominant.
    // (Noise is optional; keep it very low.)
    if (n >= 3) {
        int flips = max(1, n / 50);
        for (int t = 0; t < flips; t++) {
            int r = RAND(rd, 0, n - 1);
            int c = RAND(rd, 0, n - 1);
            grid[r][c] = alphabet[RAND(rd, 0, (int)alphabet.size() - 1)];
        }
    }

    return grid;
}

vector<vector<char>> makeGrid(int mode, int n, const string& alphabet, mt19937& rd) {
    switch (mode) {
    case 0:
        return generateRandomGrid(n, alphabet, rd);
    case 1:
        return generateSpiralGrid(n, alphabet, rd);
    default:
        return generateRandomGrid(n, alphabet, rd);
    }
}

string generateKeyword(const vector<vector<char>>& grid, int gridR, int gridC, const string& alphabet, int type,
                       int minLen, int maxLen, mt19937& rd) {
    (void)alphabet; // kept for compatibility; type=0 uses kAbsentChar, type>0 reads from grid
    int len = RAND(rd, minLen, maxLen);
    int direction = RAND(rd, 0, 1);

    int r, c;
    if (direction == 0) {
        r = RAND(rd, 0, gridR - 1);
        c = RAND(rd, 0, max(0, gridC - len));
    } else {
        r = RAND(rd, 0, max(0, gridR - len));
        c = RAND(rd, 0, gridC - 1);
    }

    string s;
    if (type == 0) {
        for (int l = 0; l < len - 1; l++) {
            if (direction == 0)
                s += grid[r][c + l];
            else
                s += grid[r + l][c];
        }
        // Absolute "not found": inject a character that does not exist in the grid at all.
        // (We generate grids without kAbsentChar.)
        char correct = (direction == 0) ? grid[r][c + len - 1] : grid[r + len - 1][c];
        s += (correct == kAbsentChar ? 'y' : kAbsentChar);
    } else {
        for (int l = 0; l < len; l++) {
            if (direction == 0)
                s += grid[r][c + l];
            else
                s += grid[r + l][c];
        }
    }
    return s;
}

vector<string> generateCumulativeDict(int totalCount, const vector<vector<char>>& grid, int gridR, int gridC,
                                      const string& alphabet, double minScale, double maxScale, mt19937& rd) {
    vector<string> dict;
    int minLen = (int)(gridC * minScale);
    int maxLen = (int)(gridC * maxScale);
    if (minLen < 1)
        minLen = 1;
    if (maxLen < minLen)
        maxLen = minLen;

    int notFound = (int)(totalCount * 0.3);
    int found = totalCount - notFound;
    int hardFound = (int)(found * 0.8);

    vector<int> types;
    for (int i = 0; i < notFound; i++)
        types.push_back(0);
    for (int i = 0; i < hardFound; i++)
        types.push_back(1);
    for (int i = 0; i < (found - hardFound); i++)
        types.push_back(2);
    shuffle(types.begin(), types.end(), rd);

    for (int type : types) {
        if ((int)dict.size() < totalCount)
            dict.push_back(generateKeyword(grid, gridR, gridC, alphabet, type, minLen, maxLen, rd));
    }
    return dict;
}

void saveTest(const string& path, int r, int c, const vector<vector<char>>& masterGrid, const vector<string>& dict,
              int dictCount) {
    fs::path fp(path);
    if (fp.has_parent_path() && !fp.parent_path().empty())
        fs::create_directories(fp.parent_path());
    ofstream out(path);
    if (!out) {
        cerr << "Cannot write: " << path << "\n";
        exit(1);
    }
    out << r << " " << c << "\n";
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++)
            out << masterGrid[i][j] << (j == c - 1 ? "" : " ");
        out << "\n";
    }
    out << dictCount << "\n";
    for (int i = 0; i < dictCount; i++)
        out << dict[i] << "\n";
}

static void generateOneFile(int mode, int n, int dictCount, const string& path, const string& alphabet,
                            unsigned seed) {
    if (n > kMaxGridDim || n < 1) {
        cerr << "Invalid grid size " << n << "\n";
        exit(1);
    }
    if (dictCount < 1 || dictCount > kMaxDict) {
        cerr << "Invalid dict " << dictCount << "\n";
        exit(1);
    }
    mt19937 rd(seed);
    vector<vector<char>> grid = makeGrid(mode, n, alphabet, rd);
    auto dict = generateCumulativeDict(dictCount, grid, n, n, alphabet, 0.8, 1.0, rd);
    saveTest(path, n, n, grid, dict, dictCount);
}

static unsigned benchSeed(unsigned base, int scenario, int bucket, int mode) {
    return base + scenario * 1000000u + bucket * 10000u + (unsigned)mode;
}

static void generateBenchmarkInputTree(const string& rootIn, unsigned baseSeed) {
    const string alphabet = kGridAlphabet;
    static const char* modeFiles[] = {"random.txt", "spiral.txt"};
    const fs::path base = fs::path(rootIn);

    const int s1sizes[] = {10, 250, 500, 1000};
    for (int b = 0; b < 4; b++) {
        int n = s1sizes[b];
        for (int m = 0; m < 2; m++) {
            fs::path out = base / "Scenario 1" / to_string(n) / modeFiles[m];
            unsigned sd = benchSeed(baseSeed, 1, b, m);
            generateOneFile(m, n, kBenchScenario1K, out.string(), alphabet, sd);
            cout << "Wrote " << out.string() << "\n";
        }
    }

    const int s2k[] = {10, 50, 100, 200};
    int g = kBenchScenario2Grid;
    for (int b = 0; b < 4; b++) {
        int k = s2k[b];
        for (int m = 0; m < 2; m++) {
            fs::path out = base / "Scenario 2" / to_string(k) / modeFiles[m];
            unsigned sd = benchSeed(baseSeed, 2, b, m);
            generateOneFile(m, g, k, out.string(), alphabet, sd);
            cout << "Wrote " << out.string() << "\n";
        }
    }
}

static void printUsage(const char* prog) {
    cerr << "Usage:\n"
         << "  " << prog << " --mode <random|spiral> --size N --out <file> [--dict K] [--seed S]\n"
         << "  " << prog << " -m <0|1> -n N -o <file> [--dict K] [--seed S]\n\n"
         << "  " << prog << " --bench [--bench-root <dir>] [--seed S]\n"
         << "      Generate the full Benchmark Test/Input tree:\n"
         << "      Scenario 1: folders 10,250,500,1000 with 2 files each (random, spiral), K="
         << kBenchScenario1K << ".\n"
         << "      Scenario 2: dict folders 10,50,100,200 with 2 files each, grid " << kBenchScenario2Grid << "x"
         << kBenchScenario2Grid << ".\n\n"
         << "Modes (single file):\n"
         << "  random    (0), spiral (1).\n\n"
         << "Options:\n"
         << "  --dict K      1.." << kMaxDict << " (default 50). Official PDF: K ≤ 100.\n"
         << "  --seed S      RNG seed (default 42).\n"
         << "  --bench-root  Root directory for Scenario 1/2 (default: Benchmark Test/Input).\n"
         << "  -h, --help\n";
}

static bool parseMode(const string& s, int& modeOut) {
    if (s == "0" || s == "random") {
        modeOut = 0;
        return true;
    }
    if (s == "1" || s == "spiral") {
        modeOut = 1;
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    string modeStr;
    string outPath;
    int size = 0;
    int dictCount = 50;
    unsigned seed = 42;
    bool doBench = false;
    string benchRoot = "Benchmark Test/Input";

    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "-h" || a == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        if (a == "--bench") {
            doBench = true;
            continue;
        }
        if (a == "--bench-root" && i + 1 < argc) {
            benchRoot = argv[++i];
            continue;
        }
        if ((a == "--mode" || a == "-m") && i + 1 < argc)
            modeStr = argv[++i];
        else if ((a == "--size" || a == "-n") && i + 1 < argc)
            size = atoi(argv[++i]);
        else if ((a == "--out" || a == "-o") && i + 1 < argc)
            outPath = argv[++i];
        else if (a == "--dict" && i + 1 < argc)
            dictCount = atoi(argv[++i]);
        else if (a == "--seed" && i + 1 < argc)
            seed = (unsigned)strtoul(argv[++i], nullptr, 10);
    }

    if (doBench) {
        generateBenchmarkInputTree(benchRoot, seed);
        cout << "Benchmark input tree done under \"" << benchRoot << "\".\n";
        return 0;
    }

    int mode = -1;
    if (!modeStr.empty() && !parseMode(modeStr, mode)) {
        cerr << "Unknown --mode: " << modeStr << "\n";
        printUsage(argv[0]);
        return 1;
    }
    if (mode < 0 || size <= 0 || outPath.empty()) {
        cerr << "Missing arguments. Use --bench or (--mode, --size, --out).\n\n";
        printUsage(argv[0]);
        return 1;
    }
    if (size > kMaxGridDim) {
        cerr << "Error: --size must be <= " << kMaxGridDim << ".\n";
        return 1;
    }
    if (dictCount < 1 || dictCount > kMaxDict) {
        cerr << "Error: --dict K must satisfy 1 <= K <= " << kMaxDict << ".\n";
        return 1;
    }

    const string gridAlphabet = kGridAlphabet;
    generateOneFile(mode, size, dictCount, outPath, gridAlphabet, seed);

    const char* names[] = {"random", "spiral"};
    cout << "OK: " << names[mode] << ", " << size << "x" << size << ", dict=" << dictCount << " -> " << outPath
         << "\n";
    return 0;
}
