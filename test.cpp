#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

mt19937 rd(42); 
int RAND(int l, int r) { return uniform_int_distribution<int>(l, r)(rd); }

// Sinh Master Grid lớn nhất (500x500)
vector<vector<char>> generateMasterGrid(int size, string alphabet) {
    vector<vector<char>> grid(size, vector<char>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            grid[i][j] = alphabet[RAND(0, alphabet.size() - 1)];
    return grid;
}

// Hàm sinh từ khóa theo yêu cầu độ khó và độ dài
string generateKeyword(const vector<vector<char>>& grid, int gridR, int gridC, string alphabet, int type, int minLen, int maxLen) {
    int len = RAND(minLen, maxLen);
    int r = RAND(0, gridR - 1);
    int c = RAND(0, max(0, gridC - len));
    string s = "";

    if (type == 0) { // Near-miss Not Found (Hard)
        for (int l = 0; l < len - 1; l++) s += grid[r][c + l];
        char wrong;
        do { wrong = alphabet[RAND(0, alphabet.size() - 1)]; } while (wrong == grid[r][c + len - 1]);
        s += wrong;
    } else { // Found (Hard/Easy)
        for (int l = 0; l < len; l++) s += grid[r][c + l];
    }
    return s;
}

// Hàm sinh danh sách từ khóa tích lũy (Cumulative Dictionary)
vector<string> generateCumulativeDict(int totalCount, const vector<vector<char>>& grid, int gridR, int gridC, string alphabet, double minScale, double maxScale) {
    vector<string> dict;
    int minLen = gridC * minScale;
    int maxLen = gridC * maxScale;
    if (minLen < 1) minLen = 1;

    int notFound = totalCount * 0.3;
    int found = totalCount - notFound;
    int hardFound = found * 0.8;

    vector<int> types;
    for(int i=0; i<notFound; i++) types.push_back(0);
    for(int i=0; i<hardFound; i++) types.push_back(1);
    for(int i=0; i<(found - hardFound); i++) types.push_back(2);
    shuffle(types.begin(), types.end(), rd);

    for(int type : types) {
        dict.push_back(generateKeyword(grid, gridR, gridC, alphabet, type, minLen, maxLen));
    }
    return dict;
}

void saveTest(string path, int r, int c, const vector<vector<char>>& masterGrid, const vector<string>& dict, int dictCount) {
    fs::create_directories(fs::path(path).parent_path());
    ofstream out(path);
    out << r << " " << c << "\n";
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) out << masterGrid[i][j] << (j == c - 1 ? "" : " ");
        out << "\n";
    }
    out << dictCount << "\n";
    for (int i = 0; i < dictCount; i++) out << dict[i] << "\n";
    out.close();
}

int main() {
    string ALPHABET = "abcdefghijklmnopqrstuvwxyz";
    auto masterGrid = generateMasterGrid(500, ALPHABET);

    // --- SCENARIO 1: Dictionary cố định (250 từ), Grid tăng dần ---
    // Từ khóa phải khớp Grid 10x10 đầu tiên, độ dài 8-10 ký tự.
    auto s1_master_dict = generateCumulativeDict(250, masterGrid, 10, 10, ALPHABET, 0.8, 1.0);
    saveTest("Input/Scenario 1/small.txt", 10, 10, masterGrid, s1_master_dict, 250);
    saveTest("Input/Scenario 1/medium.txt", 250, 250, masterGrid, s1_master_dict, 250);
    saveTest("Input/Scenario 1/large.txt", 500, 500, masterGrid, s1_master_dict, 250);

    // --- SCENARIO 2: Grid cố định (250x250), Dictionary tăng dần (10 -> 250 -> 500) ---
    // Từ khóa đạt 80-100% Grid (200-250 ký tự).
    auto s2_master_dict = generateCumulativeDict(500, masterGrid, 250, 250, ALPHABET, 0.8, 1.0);
    saveTest("Input/Scenario 2/small.txt", 250, 250, masterGrid, s2_master_dict, 10);
    saveTest("Input/Scenario 2/medium.txt", 250, 250, masterGrid, s2_master_dict, 250);
    saveTest("Input/Scenario 2/large.txt", 250, 250, masterGrid, s2_master_dict, 500);

    cout << "Success: Generated cumulative tests with long keywords (80-100% grid size)." << endl;
    return 0;
}
