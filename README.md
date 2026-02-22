# String-Matching
A benchmark string matching project

# Command (temp)
## Compile
g++ file1.cpp @(Get-ChildItem Algorithm -Recurse -Filter *.cpp | ForEach-Object { $_.FullName }) -o crossword.exe -std=c++17
## Run (in CMD)
crossword.exe -a [Algorithm] -i [Input Directory] -o [Output Directory]
Ex: "crossword.exe -a bf -i "Benchmark Test\Scenario 1\large.txt" -o solution.txt"
