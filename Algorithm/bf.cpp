#include "library.h"

vector<pair<int, int>> bf(string toSearch, string target, string& name, int& cmp_cnt) {
    name = "bf";
    
    vector<pair<int, int>> matches;
    int n = toSearch.length();
    int m = target.length();

    if (m == 0 || m > n) {
        return matches;
    }

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            cmp_cnt++; 
            if (toSearch[i + j] != target[j]) {
                break; 
            }
        }
        if (j == m) {
            matches.push_back({i, i + m - 1});
        }
    }

    return matches;
}
