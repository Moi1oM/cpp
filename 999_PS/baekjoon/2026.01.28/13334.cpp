#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

int main() {
    int N, L;
    size_t result=0;
    std::cin>>N;
    std::vector<std::pair<int, int>> hc;
    hc.reserve(N);
    for (size_t i = 0; i < N; i++) {
        int h, c;
        std::cin >> h >> c;
        if (h <= c) {
            hc.push_back(std::make_pair(h,c));
        } else {
            hc.push_back(std::make_pair(c,h));
        }
    }
    std::cin>>L;

    std::sort(hc.begin(), hc.end(), 
          [](const auto& a, const auto& b) {
              return a.second < b.second;
          });
    
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
    for(size_t i=0; i<hc.size(); i++) {
        int now_r = hc[i].second;
        int now_l = now_r - L;
        int l = hc[i].first;
        int r = hc[i].second;
        if (r - l > L) continue;

        pq.push(hc[i].first);
        while (!pq.empty() && pq.top() < now_l) {
            pq.pop();
        }
        result = std::max(result, pq.size());
    }
    
    std::cout << result << '\n';

    return 0;
}