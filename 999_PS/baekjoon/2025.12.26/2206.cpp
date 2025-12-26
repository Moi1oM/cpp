#include<iostream>
#include<vector>
#include<string>
#include<queue>
#include<tuple>

int result = 100000000;
int n,m;

struct State {
    int x;
    int y;
    int dist;
    bool broke;
};

int bfs(std::vector<std::vector<int>>& g, 
    std::vector<std::vector<std::vector<char>>>& visited) {
        std::queue<State> q;
        q.push(State{0,0,1,false});
        visited[0][0][0] = true;
        int dx[4] = {0,0,-1,1} , dy[4] = {-1,1,0,0};

        while (!q.empty()) {
            State curr = q.front(); q.pop();
            auto [x, y, dist, broke] = curr;
            if (x+1 == n && y+1 == m) {
                return dist;
            }

            for (int i=0;i<4;i++) {
                int nx = x+dx[i], ny = y+dy[i];
                if (nx<0 || ny<0 || nx>=n || ny>=m) continue;
                if (!broke) {
                    if (g[nx][ny] == 0) {
                        if (!visited[nx][ny][0]) {
                            visited[nx][ny][0] = true;
                            q.push(State{nx,ny,dist+1,broke});
                        }
                    } else { // 부순적 없는데 벽일때
                        if (!visited[nx][ny][1]) {
                            visited[nx][ny][1] = true;
                            q.push(State{nx,ny,dist+1,!broke});
                        }
                    }
                } else { // broke 한 상태
                    if (g[nx][ny] == 1) continue;
                    if (!visited[nx][ny][1]) {
                        visited[nx][ny][1] = true;
                        q.push(State{nx,ny,dist+1,broke});
                    }
                }
            }
        }
    return -1;
} 

int main(){
    std::cin >> n >> m;
    std::vector<std::vector<int>> graph(n, std::vector<int>(m));
    std::vector<std::vector<std::vector<char>>> visited(n, std::vector<std::vector<char>>(m, std::vector<char>(2, 0)));
    for(int i=0;i<n;i++) {
        std::string tmp;
        std::cin >> tmp;
        for(int j=0;j<m;j++) {
            int num = tmp[j] - '0';
            graph[i][j] = num;
        }
    }

    std::cout << bfs(graph, visited) << "\n";

    return 0;
}