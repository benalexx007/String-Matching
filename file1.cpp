#include "Algorithm/process.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
    unordered_map<string, function<vector<pair<int, int>>(string toSearch, string target, string& name, int& cmp_cnt)>> 
        algorithms {{"z", z}, {"kmp", kmp}, {"bm", bm}, {"bf", bf}, {"rk", rk}};
    string in, out, algo;
    for(int i=1; i<argc; ++i){
        string arg = argv[i];
        if(arg == "-a" && i + 1 < argc)
            algo = argv[++i];
        else if (arg == "-i" && i + 1 < argc)
            in = argv[++i];
        else if (arg == "-o" && i + 1 < argc)
            out = argv[++i];
    }
    if(algo.empty() || in.empty() || out.empty()){
        cerr << "Usage: " << argv[0] << " -a <algorithm> -i <input> -o <output>\n";
        return 1;
    }
    auto it = algorithms.find(algo);
    if(it == algorithms.end()){
        cerr << "Unknown algorithm: " << algo << '\n';
        return 1;
    }
    run(in, out, it->second);
    return 0;
}