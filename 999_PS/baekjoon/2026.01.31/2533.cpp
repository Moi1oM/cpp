#include <iostream>
#include <vector>
#include <algorithm>
int n;
const int INF = 987654321;

void dfs(int now,
    std::vector<std::vector<int>>& g,
    std::vector<char>& v,
    std::vector<std::vector<int>>& dp) {
    v[now] = true;
    dp[now][0] = 0;
    dp[now][1] = 1;
    for(auto& x: g[now]) {
        if (!v[x]) {
            dfs(x, g, v, dp);
            dp[now][0] += dp[x][1];
            dp[now][1] += std::min(dp[x][0], dp[x][1]);
        }
    }
}

int main() {
    std::cin >> n;
    std::vector<std::vector<int>> graph(n+1, std::vector<int>());
    std::vector<char> visited(n+1, false);
    // dp[x][0] 이 자신이 아닐 때, dp[x][1] 이 자신이 얼리어답터일때.
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(2, INF));
    for (size_t i = 0; i < n-1; i++) {
        int a, b;
        std::cin >> a >> b;
        graph[a].push_back(b);
        graph[b].push_back(a);
    }
    dfs(1, graph, visited, dp);
    std::cout << std::min(dp[1][0], dp[1][1]) << '\n';
    return 0;
}