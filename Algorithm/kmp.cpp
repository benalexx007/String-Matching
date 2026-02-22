#include "library.h"

vector<int> buildLPS( string target, int& cmp_cnt) {
    int m = target.size();
    vector<int> lps(m, 0);
    int len = 0, i = 1;
    while (i < m) {
        cmp_cnt++;
        if (target[i] == target[len])
            len++, lps[i] = len, i++;
        else {
            if (len != 0)
                len = lps[len - 1]; 
            else
                lps[i] = 0, i++;
        }
    }
    return lps;
}

vector<pair<int, int>> kmp(string toSearch, string target, string& name, int& cmp_cnt) {
    name = "KMP";
    vector<pair<int, int>> matches;
    int n = toSearch.size();
    int m = target.size();
    if (m == 0 || n < m) return matches;
    vector<int> lps = buildLPS(target, cmp_cnt);
    int i = 0, j = 0;
    while (i < n) {
        cmp_cnt++;
        if (target[j] == toSearch[i]) {
            j++, i++;
            if (j == m) {
                matches.push_back({i - m, i - 1});
                j = lps[j - 1]; 
            }
        } 
        else {
            if (j != 0)
                j = lps[j - 1]; 
            else
                i++;
        }
    }
    return matches;
}