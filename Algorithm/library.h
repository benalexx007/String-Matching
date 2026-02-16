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

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

map<string, vector<Line>> z(vector<vector<char>> grid, vector<string> dict);
map<string, vector<Line>> kmp(vector<vector<char>> grid, vector<string> dict);
map<string, vector<Line>> bf(vector<vector<char>> grid, vector<string> dict);
map<string, vector<Line>> bm(vector<vector<char>> grid, vector<string> dict);
map<string, vector<Line>> rk(vector<vector<char>> grid, vector<string> dict);

#endif