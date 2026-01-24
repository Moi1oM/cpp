#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>

int count = 1;
std::unordered_map<char, int> mp{
    {'U', 0},
    {'D', 1},
    {'L', 2},
    {'R', 3},
};
int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};

void dfs(int x, int y, int c,
    std::deque<std::pair<int, int>>& t,
    std::vector<std::vector<int>>& g,
    std::vector<std::vector<int>>& v) {
        v[x][y] = count;
        int idx = mp[g[x][y]];
        int px = x + dx[idx];
        int py = y + dy[idx];
        // std::cout << x << " " << y << " " << c << " " << idx << " " << px << " " << py << '\n';
        if (v[px][py] && v[px][py] < c) {
            while(!t.empty()) {
                std::pair<int, int> p = t.front();
                t.pop_front();
                int a = p.first, b = p.second;
                v[a][b] = v[px][py];
            }
            count--;
            return;
        } else if (!v[px][py]) {
            t.push_back(std::make_pair(px, py));
            dfs(px, py, c, t, g, v);
        }
}

int main() {
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> graph(n, std::vector<int> (m));
    std::vector<std::vector<int>> visited(n, std::vector<int> (m, 0));
    for(int i=0;i<n;i++) {
        std::string s;
        std::cin >> s;
        for(int j=0;j<s.size();j++) graph[i][j] = s[j];
    }

    for(int i=0;i<n;i++) {
        for(int j=0;j<m;j++) {
            if(!visited[i][j]) {
                std::deque<std::pair<int, int>> trace;
                trace.push_back(std::make_pair(i, j));
                // std::cout << "dfs start\n";
                dfs(i, j, count, trace, graph, visited);
                count++;
            }
        }
    }
    std::cout << count-1 << '\n';
    return 0;
}