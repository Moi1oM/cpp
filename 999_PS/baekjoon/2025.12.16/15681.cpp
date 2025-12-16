#include<iostream>
#include<vector>

using namespace std;
vector<char> visited;
vector<int> depth;

int dfs(int n, vector<vector<int>>& g) {
    visited[n] = true;
    int result = 1;
    for(int i=0;i<g[n].size();i++) {
        int next_node = g[n][i];
        if (!visited[next_node]) {
            int tmp = dfs(next_node, g);
            result += tmp;
        }
    }
    depth[n] = result;
    visited[n] = false;
    return result;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int N, R, Q;
    cin >> N >> R >> Q;
    vector<vector<int>> graph(N + 1, vector<int>());
    visited.resize(N+1, false);
    depth.resize(N+1, false);
    for (int i=0;i<N-1;i++) {
        int a, b;
        cin >> a >> b;
        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    dfs(R, graph);
    for(int i=0;i<Q;i++) {
        int a;
        cin >> a;
        cout << depth[a] << "\n";
    }
    return 0;
}