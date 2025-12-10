#include<iostream>
#include<vector>
#include<algorithm>

int cal_trees(std::vector<int>& v, int now) {
    int result=0;
    for(int i=0;i<v.size();i++) {
        if(v[i]>now) result += v[i]-now;
    }
    return result;
}

int binary_search(std::vector<int>& v, int target) {
    int result = 0;
    int left=0, right=v[v.size()-1];
    while(left<=right) {
        int mid = left + (right-left)/2;
        int tmp_result = cal_trees(v, mid);
        if(tmp_result >= target) {
            result = std::max(result, mid);
            left = mid + 1;
        } else { //tmp_result < target
            right = mid - 1;
        }
    }
    return result;
}

int main() {
    int N, M;
    std::cin >> N >> M;
    std::vector<int> trees(N);
    for(int i=0;i<N;i++) std::cin >>trees[i];
    std::sort(trees.begin(), trees.end());

    std::cout << binary_search(trees, M) << "\n";

    
    return 0;
}