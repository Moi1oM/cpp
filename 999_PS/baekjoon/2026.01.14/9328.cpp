#include <iostream>
#include <vector>
#include <deque>

int h, w, result;
std::string key;
std::deque<std::pair<int, int>> later;
bool flag;

void dfs(int x, int y, 
    std::vector<std::vector<char>>& graph,
    std::vector<std::vector<char>>& visited) {
    visited[x][y] = true;
    int dx[4] = {0, 0, -1, 1}, dy[4] = {-1, 1, 0, 0};
    for(int i=0;i<4;i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (nx < 0 || ny < 0 || ny >w+1 || nx > h+1) continue;
        if (visited[nx][ny]) continue;
        char next_step = graph[nx][ny];
        if ('a' <= next_step && next_step <= 'z') {
            key += next_step;
            flag = true;
            dfs(nx, ny, graph, visited);
        } else if ('A' <= next_step && next_step <= 'Z') {
            int dif = 'A' - 'a';
            char need_key = next_step - dif;
            if (key.find(need_key) != std::string::npos) {
                dfs(nx, ny, graph, visited);
            } else {
                later.push_back(std::make_pair(nx, ny));
            }
        } else if (next_step == '.') {
            dfs(nx, ny, graph, visited);
        } else if (next_step == '*') {
            
        } else { // next_step == '$'
            result += 1;
            dfs(nx, ny, graph, visited);
        }
    }
}

int main() {
    int T;
    std::cin >> T;
    while (T--) {
        later.clear();
        key = "";
        result = 0;
        // 입력
        std::cin >> h >> w;
        std::vector<std::vector<char>> graph(h+2, std::vector<char>(w+2, '.'));
        std::vector<std::vector<char>> visited(h+2, std::vector<char>(w+2, false));
        for(int i=1;i<=h;i++) {
            std::string s;
            std::cin >> s;
            for(int j=1;j<=w;j++) {
                graph[i][j] = s[j-1];
            }
        }
        std::cin >> key;
        // 처리
        flag = false;
        dfs(0, 0, graph, visited);
        while (flag) {
            flag = false;
            for(size_t i=0;i<later.size();i++) {
                char next_step = graph[later[i].first][later[i].second];
                int dif = 'A' - 'a';
                char need_key = next_step - dif;
                if (key.find(need_key) != std::string::npos) {
                    dfs(later[i].first, later[i].second, graph, visited);
                }
            }
        }
        // 출력
        std::cout << result << '\n';
    }
    return 0;
}