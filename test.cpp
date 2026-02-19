#include <bits/stdc++.h>
using namespace std;

mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());

// Hàm RAND an toàn hơn, kiểm tra nếu l > r thì hoán đổi hoặc trả về l
int RAND(int l, int r) {
    if (l > r) return l; 
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
        if (RAND(1, 100) <= 70) {
            int len = RAND(3, 8);
            int direction = RAND(0, 1); 
            string s = "";

            if (direction == 0) { // Ngang
                // SỬA LỖI: Đảm bảo r, c không vượt quá giới hạn grid và len
                int r = (i < 20) ? RAND(0, 4) : RAND(0, MAX_R - 1);
                // Giới hạn c sao cho c + len không vượt quá MAX_C
                int c = (i < 20) ? RAND(0, max(0, 5 - len)) : RAND(0, MAX_C - len);
                
                for (int l = 0; l < len; l++) s += masterGrid[r % MAX_R][(c + l) % MAX_C];
            } 
            else { // Dọc
                int r = (i < 20) ? RAND(0, max(0, 5 - len)) : RAND(0, MAX_R - len);
                int c = (i < 20) ? RAND(0, 4) : RAND(0, MAX_C - 1);

                for (int l = 0; l < len; l++) s += masterGrid[(r + l) % MAX_R][c % MAX_C];
            }
            masterKeywords.push_back(s);
        } 
        else {
            string s = "";
            int len = RAND(3, 8);
            for (int l = 0; l < len; l++) s += (char)('a' + RAND(0, 25));
            masterKeywords.push_back(s);
        }
    }
}

int main() {
    generateMasterData();
    ofstream out("BIG_TEST.txt");
    
    if (!out.is_open()) {
        cerr << "Error: Could not open file for writing!" << endl;
        return 1;
    }
    
    // Scenario 1: Tăng Grid Size
    vector<int> sizes = {10, 250, 500};
    int fixedK = 10; 

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
        out << "\n"; 
    }

    // Scenario 2: Tăng số từ khóa
    int fixedSize = 100;
    vector<int> kValues = {10, 50, 100};

    for (int k : kValues) {
        out << fixedSize << " " << fixedSize << "\n";
        for (int i = 0; i < fixedSize; i++) {
            for (int j = 0; j < fixedSize; j++) {
                out << masterGrid[i][j] << (j == fixedSize - 1 ? "" : " ");
            }
            out << "\n";
        }
        out << k << "\n";
        // Lấy từ khóa từ đầu danh sách để đảm bảo tính nhất quán
        for (int i = 0; i < k; i++) {
            out << masterKeywords[i] << "\n";
        }
        out << "\n";
    }

    out.close();
    cout << "Generated BIG_TEST.txt successfully." << endl;
    return 0;
}
