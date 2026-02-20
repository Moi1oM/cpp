#include<iostream>
#include<vector>
int max_dist=0, max_node=-1;

void dfs(int _node, int _dist, 
    std::vector<std::vector<std::pair<int,int>>>& _graph, 
    std::vector<char>& _visited){
    _visited[_node] = true;
    if(_dist > max_dist) {
        max_dist = _dist;
        max_node = _node;
    }
    for(int i=0;i<_graph[_node].size();i++) {
        std::pair<int,int> tmp;
        tmp = _graph[_node][i];
        int to=tmp.first, cost=tmp.second;
        if(_visited[to]) continue;
        dfs(to, _dist+cost, _graph, _visited);
    }    
}

int main(){
    int n;
    std::cin>>n;

    if (n == 1) {
        std::cout << 0 << "\n";
        return 0;
    }
    std::vector<std::vector<std::pair<int,int>>> g(n+1);
    for(int i=0;i<n-1;i++){
        int a,b,c;
        std::cin>>a>>b>>c;
        g[a].push_back(std::make_pair(b,c));
        g[b].push_back(std::make_pair(a,c));
    }
    std::vector<char> visited(n+1, false);
    dfs(1, 0, g, visited);
    visited.assign(n+1, false);
    max_dist=0;
    dfs(max_node, 0, g, visited);
    
    std::cout<<max_dist<<"\n";
    return 0;
}