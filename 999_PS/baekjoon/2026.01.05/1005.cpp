#include <iostream>
#include <vector>
#include <deque>

int main(){
    int T;
    std::cin >> T;
    int N, K, final_building;
    for(int t=0;t<T;t++) {
        std::cin>>N>>K;
        std::vector<int> build_time(N+1, -1), depth(N+1, 0), max_time(N+1, 0);
        std::vector<std::vector<int>> connection(N+1, std::vector<int>());
        for(int i=1;i<=N;i++) {
            int tmp;
            std::cin >> tmp;
            build_time[i] = tmp;
            max_time[i] = tmp;
        }
        for(int i=0;i<K;i++) {
            int from, to;
            std::cin>> from >> to;
            connection[from].push_back(to);
            depth[to] += 1;
        }
        std::cin >> final_building;
        std::deque<std::pair<int,int>> q;
        for(int i=1;i<=N;i++) {
            if (depth[i] == 0) q.push_back(std::make_pair(i, build_time[i]));
        }
        while(!q.empty()) {
            std::pair<int, int> tmp = q.front();
            q.pop_front();
            int n_node = tmp.first, n_time = tmp.second;

            for(auto& x : connection[n_node]) {
                depth[x] -= 1;
                max_time[x] = std::max(max_time[x], n_time+build_time[x]);
                if (depth[x] == 0) q.push_back(std::make_pair(x, max_time[x]));
            }
        }
        std::cout << max_time[final_building] << '\n';
    }
    return 0;
}