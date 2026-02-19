#ifndef LIBRARY_SM
#define LIBRARY_SM
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <map>
#include <chrono>
#include <functional>

using namespace std;

struct Line{
    int first_x, first_y, second_x, second_y;
};

vector<pair<int, int>> z(string toSearch, string target, string& name, int& cmp_cnt);
vector<pair<int, int>> kmp(string toSearch, string target, string& name, int& cmp_cnt);
vector<pair<int, int>> bf(string toSearch, string target, string& name, int& cmp_cnt);
vector<pair<int, int>> bm(string toSearch, string target, string& name, int& cmp_cnt);
vector<pair<int, int>> rk(string toSearch, string target, string& name, int& cmp_cnt);

#endif