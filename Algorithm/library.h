#ifndef LIBRARY_SM
#define LIBRARY_SM
#include <string>
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

map<string, vector<Line>> z(string toSearch, vector<string> dict, string& name, int& cmp_cnt);
map<string, vector<Line>> kmp(string toSearch, vector<string> dict, string& name, int& cmp_cnt);
map<string, vector<Line>> bf(string toSearch, vector<string> dict, string& name, int& cmp_cnt);
map<string, vector<Line>> bm(string toSearch, vector<string> dict, string& name, int& cmp_cnt);
map<string, vector<Line>> rk(string toSearch, vector<string> dict, string& name, int& cmp_cnt);

#endif