#include "library.h"

vector<int> buildGood(const string& target, int& cmp_cnt) {
    int m = target.size();
    vector<int> good(m + 1, m), border(m + 1, 0);
    int i = m, j = m + 1;
    border[i] = j;
    while (i > 0) {
        while (j <= m && target[i - 1] != target[j - 1]) {
            cmp_cnt++;
            if (good[j] == m)
                good[j] = j - i;
            j = border[j];
        }
        i--, j--;
        border[i] = j;
    }
    j = border[0];
    for (i = 0; i <= m; i++) {
        if (good[i] == m)
            good[i] = j, cmp_cnt++;
        if (i == j)
            j = border[j], cmp_cnt++;
    }
    return good;
}

vector<pair<int, int>> bm(string toSearch, string target, string& name, int& cmp_cnt) {
    name = "Boyer-Moore";
    vector<pair<int, int>> matches;
    int n = toSearch.size(), m = target.size();
    if (m == 0 || n < m) return matches;
    vector<int> bad(256, -1);
    for (int i = 0; i < target.size(); i++)
        bad[(unsigned char)target[i]] = i;
    vector<int> good = buildGood(target, cmp_cnt);
    int i = 0;
    while (i <= n - m) {
        int j;
        for (j = m - 1; j >= 0; j--) {
            cmp_cnt++;
            if (target[j] != toSearch[i + j])
                break;
        }
        if (j < 0) {
            matches.push_back({i, i + m - 1});
            i += good[0];
        } 
        else {
            int badShift = j - bad[(unsigned char)toSearch[i + j]], goodShift = good[j+1];
            i += max(1, max(badShift, goodShift));
        }
    }
    return matches;
}