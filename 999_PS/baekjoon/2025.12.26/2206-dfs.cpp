#include<iostream>
#include<vector>
#include<string>

int result = 100000000;
int n,m;

void dfs(int x, int y, bool breaked, int depth,
    std::vector<std::vector<int>>& g, 
    std::vector<std::vector<char>>& visited) {
        // std::cout << x << "  " << y << "\n";
        if (x+1 == n && y+1 == m) {
            result = std::min(result, depth);
            return;
        }
        visited[x][y] = true;
        
        int dx[4] = {0,0,-1,1}, dy[4] = {-1,1,0,0};
        for(int i=0;i<4;i++) {
            int nx = x+dx[i], ny = y+dy[i];
            bool cannot_aceess = nx<0||ny<0||nx>=n||ny>=m;
            if (cannot_aceess) continue;
            if (!visited[nx][ny]) {
                if (g[nx][ny] == 0) {
                    dfs(nx, ny, breaked, depth+1, g, visited);
                } else {
                    if (!breaked) {
                        dfs(nx, ny, !breaked, depth+1, g, visited);
                    }
                }
            }
        }
        visited[x][y] = false;
}

int main(){
    std::cin >> n >> m;
    std::vector<std::vector<int>> graph(n, std::vector<int>(m));
    std::vector<std::vector<char>> visited(n, std::vector<char>(m, false));
    for(int i=0;i<n;i++) {
        std::string tmp;
        std::cin >> tmp;
        for(int j=0;j<m;j++) {
            int num = tmp[j] - '0';
            graph[i][j] = num;
        }
    }

    dfs(0, 0, false, 1, graph, visited);

    if (result == 100000000) std::cout << -1 << "\n";
    else std::cout << result << "\n";

    return 0;
}