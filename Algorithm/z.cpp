#include "library.h"

vector<int> z_function(string s, int& cmp_cnt) {
    int n = s.length();
    vector<int> z(n);
    for (int i = 1, l = 0, r = 0; i < n; ++i) {
        if (i <= r)
            z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]])
            ++z[i], cmp_cnt++;
        if (i + z[i] - 1 > r)
            l = i, r = i + z[i] - 1;
    }
    return z;
}

vector<pair<int, int>> z(string toSearch, string target, string& name, int& cmp_cnt){
    name = "Z-function";
    vector<pair<int, int>> matches;
    int n = toSearch.length(), m = target.length();
    if (m == 0 || m > n)
        return matches;
    vector<int> z = z_function(target + char(0) + toSearch, cmp_cnt);
    for (int i = m + 1; i <= n + 1; i++) {
        if(z[i] == m)
            matches.push_back({i - m - 1, i - 2});
    }
    return matches;
}