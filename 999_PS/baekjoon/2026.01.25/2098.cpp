#include <iostream>
#include <vector>

const int MAX = 987654321;

int dfs(int x, int mask, int n,
    std::vector<std::vector<int>>& g,
    std::vector<std::vector<int>>& dp) {
    
    if (mask == (1 << n) - 1) {
        return g[x][0] != 0 ? g[x][0] : MAX;
    }

    if (dp[mask][x] != -1) return dp[mask][x];

    dp[mask][x] = MAX;

    for (int next = 0; next < n; next++) {
        if (g[x][next] == 0 || (mask & (1 << next))) continue;
        int res = dfs(next, mask | (1 << next), n, g, dp);
        if (res != MAX) {
            dp[mask][x] = std::min(dp[mask][x], res + g[x][next]);
        }
    }

    return dp[mask][x];
}

int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n;
    std::cin >> n;
    std::vector<std::vector<int>> g(n, std::vector<int>(n, 0));
    for(int i=0;i<n;i++) {
        for(int j=0;j<n;j++) std::cin >> g[i][j];
    }
    //dp[mask][i] = 지금까지 방문한 도시 집합이 mask이고, 현재 도시가 i일 때의 최소 비용
    std::vector<std::vector<int>> dp(1<<n, std::vector<int>(n, -1));

    int ans = dfs(0, 1 << 0, n, g, dp);

    if (ans >= MAX) std::cout << 0 << '\n';
    else std::cout << ans << '\n';    

    return 0;
}