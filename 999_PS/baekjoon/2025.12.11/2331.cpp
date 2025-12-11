#include<iostream>
#include<vector>
#include<algorithm>
#include<cmath>

std::vector<std::pair<int, int>> trace;
int result=-1, P;

int cal_next(int prev) {
    int next = 0;
    while(prev) {
        int last = prev % 10;
        next += std::pow(last, P);
        prev /= 10;
    }
    return next;
}

void dfs(int n, int depth) {
    if (result != -1) return;

    auto it = std::find_if(trace.begin(), trace.end(),
        [n](const std::pair<int, int>& element) {
            return element.first == n;
        }
    );
    if (it != trace.end()) { // 찾았을 때
        result = it->second;
        return;
    } else {
        std::pair<int, int> tmp_pair;
        tmp_pair.first = n;
        tmp_pair.second = depth;
        trace.push_back(tmp_pair);
        dfs(cal_next(n), depth+1);
    }
    return;
}

int main(){
    int A;
    std::cin >> A >> P;
    dfs(A, 0);
    std::cout << result << "\n";
    return 0;
}