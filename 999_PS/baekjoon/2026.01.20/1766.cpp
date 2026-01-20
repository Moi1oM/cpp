#include <iostream>
#include <vector>
#include <queue>

int main() {
    int n, m;
    std::cin >> n >> m;
    std::vector<int> depth(n+1, 0);
    std::vector<std::vector<int>> graph(n+1, std::vector<int>());
    while(m--) {
        int f, s;
        std::cin >> f >> s;
        depth[s] += 1;
        graph[f].push_back(s);
    }
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    
    for(int i=1;i<=n;i++) {
        if(depth[i]==0) minHeap.push(i);
    }
    while(!minHeap.empty()) {
        int top = minHeap.top();
        minHeap.pop();
        std::cout << top << " ";
        for(auto& x : graph[top]) {
            depth[x] -= 1;
            if (depth[x] == 0) minHeap.push(x);
        }
    }
    std::cout<<'\n';
    return 0;
}