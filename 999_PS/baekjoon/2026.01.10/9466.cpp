#include <iostream>
#include <vector>
#include <algorithm>
int T, result;
std::vector<int> target;
std::vector<char> selected;

void dfs(int now_node, std::vector<int> & order) {
    selected[now_node] = true;
    order.push_back(now_node);
    
    if(!selected[target[now_node]]) {
        dfs(target[now_node], order);
    } else {
        //TODO : order 중간에 loop이 있으면 그만큼 result-- 시키고 return 시키기
        auto it = std::find(order.begin(), order.end(), target[now_node]);
        if (it != order.end()) {
            int length = order.size();
            int index = it - order.begin();
            result -= length - index;
            return;
        }
    }
}

int main() {
    std::cin >> T;
    while(T--) {
        int n;
        std::cin >> n;
        target.resize(n+1);
        selected.assign(n+1, false);
        result = n;
        for(int i=1;i<=n;i++) {
            std::cin>>target[i];
            if(target[i] == i) {
                selected[i] = true;
                result--;
            }
        }
        for(int i=1;i<=n;i++) {
            if (!selected[i]) {
                std::vector<int> order;
                dfs(i, order);
            }
        }
        std::cout << result << "\n";
    }

    return 0;
}