#include "library.h"
void run(const string in, const string out, const function<map<string, vector<Line>>(vector<vector<char>> grid, vector<string> dict)> algo){
    int n, m, w;
    ifstream fin(in);
    fin>> n >> m;
    vector<vector<char>> grid;
    for(int i=0; i<n; i++){
        vector<char> row;
        for(int j=0; j<m; j++){
            char c;
            fin>>c;
            row.push_back(c);
        }
        grid.push_back(row);
    }
    fin >> w;
    vector<string> dict;
    for(int i=0; i<w; i++){
        string s;
        fin >> s;
        dict.push_back(s);
    }
    fin.close();
    string name = "";
    int cmp_cnt= 0;
    auto start = chrono::high_resolution_clock::now();
    map<string, vector<Line>> store = algo(grid, dict);
    auto stop = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = stop - start;
    ofstream fout(out);
    for(auto i: dict){
        fout << i + ": ";
        if(store[i].empty()) fout << "not found";
        else
            for(auto j: store[i])
                fout << "(" << j.first_x << ", " << j.first_y << ") -> (" << j.second_x << ", " << j.second_y << "); ";
        fout << '\n';
    }
    fout << "-----------------------------------\n";
    fout << "Algorithm: " <<  name << '\n';
    fout << "Comparisons: " << cmp_cnt << '\n';
    fout << "Execution Time: " << duration.count() << "ms\n";
}