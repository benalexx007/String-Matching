#include <bits/stdc++.h>
using namespace std;

mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());

int RAND(int l, int r) {
    return l + rd() % (r - l + 1);
}

const int MAX_R = 500;
const int MAX_C = 500;
const int MAX_K = 100;

char masterGrid[MAX_R][MAX_C];
vector<string> masterKeywords;

void generateMasterData() {
    for (int i = 0; i < MAX_R; i++) {
        for (int j = 0; j < MAX_C; j++) {
            masterGrid[i][j] = (char)('a' + RAND(0, 25));
        }
    }
    for (int i = 0; i < MAX_K; i++) {
        string s = "";
        int len = RAND(3, 8);
        for (int l = 0; l < len; l++) s += (char)('a' + RAND(0, 25));
        masterKeywords.push_back(s);
    }
}

int main() {
    generateMasterData();
    
    ofstream out("BIG_TEST.txt");
    
    // --- SCENARIO 1: Tăng Grid Size ---
    vector<int> sizes = {5, 6, 7, 8, 9, 10};
    int fixedK = 5;

    for (int s : sizes) {
        out << s << " " << s << "\n";
        for (int i = 0; i < s; i++) {
            for (int j = 0; j < s; j++) {
                out << masterGrid[i][j] << (j == s - 1 ? "" : " ");
            }
            out << "\n";
        }
        out << fixedK << "\n";
        for (int i = 0; i < fixedK; i++) {
            out << masterKeywords[i] << "\n";
        }
        // Thêm dòng trống sau mỗi test case
        out << "\n"; 
    }

    // --- SCENARIO 2: Tăng số từ khóa ---
    int fixedSize = 10;
    vector<int> kValues = {5, 7, 10};

    for (int k : kValues) {
        out << fixedSize << " " << fixedSize << "\n";
        for (int i = 0; i < fixedSize; i++) {
            for (int j = 0; j < fixedSize; j++) {
                out << masterGrid[i][j] << (j == fixedSize - 1 ? "" : " ");
            }
            out << "\n";
        }
        out << k << "\n";
        for (int i = 0; i < k; i++) {
            out << masterKeywords[i] << "\n";
        }

        out << "\n";
    }

    out.close();
    cout << "Generated BIG_TEST.txt with blank lines between cases.\n";
    return 0;
}