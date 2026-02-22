#include "library.h" 

//check mathcing chars
bool checkString(const string& toSearch, const string& target, int start, int len, int& cmp_cnt) {
    for (int i = 0; i < len; ++i) {
        ++cmp_cnt;
        if (toSearch[i + start] != target[i]) return false;
    }
    return true;
}

vector<pair<int, int>> rk(string toSearch, string target, string& name, int& cmp_cnt) {
    name = "rk"; vector<pair<int, int>> res; // result

    const long long radix = 91138233; // bc the test only has lowercase english letter
    const long long q = 1e9 + 7; // prime number for hasing

    const long long SEARCH_LEN = toSearch.size();
    const long long TARGET_LEN = target.size();

    long long highOrder = 1;
    for (int i = 0; i < TARGET_LEN - 1; ++i) {
        highOrder *= radix;
    }
    highOrder = highOrder % q;

    long long targetNum = 0;
    long long curSearch = 0;

    // hashing m first chars in toSearch an hashing target
    for (int i = 0; i < TARGET_LEN; ++i) {
        targetNum = (radix * targetNum + target[i]) % q;
        curSearch = (radix * curSearch + toSearch[i]) % q;
    }

    // searching process
    for (int i = 0; i <= SEARCH_LEN - TARGET_LEN; ++i) {
        // check for equal hash
        if (targetNum == curSearch) {
            // check individual chars
            if (checkString(toSearch, target, i, TARGET_LEN, cmp_cnt)) {
                res.push_back({i, i + TARGET_LEN - 1});
            }
        }

        if (i < SEARCH_LEN - TARGET_LEN) {
            curSearch = (radix * (curSearch - (toSearch[i]) * highOrder) + toSearch[i + TARGET_LEN]) % q;
        }
        
        if (curSearch < 0) curSearch += q;
    }

    return res;
}
