#include<iostream>
#include<vector>
#include<algorithm>

int cal_count(std::vector<int>& v, int length) {
    int result=0;
    for(int i=0;i<v.size();i++) {
        result += v[i] / length;
    }
    return result;
}

int main(){
    int N, K; 
    long long result=0;
    std::cin >> N >> K;
    std::vector<int> v(N);
    for(int i=0;i<N;i++)std::cin>>v[i];
    std::sort(v.begin(), v.end());
    long long left=1, right=v[N-1];
    while(left <= right) {
        long long mid = left + (right-left)/2;
        // std::cout <<left<<" "<<right<<" "<< mid << "\n";
        int tmp_res = cal_count(v, mid);
        // std::cout << result << " " << tmp_res << "\n\n";
        if (tmp_res >= K) result = std::max(result, mid);
        if (tmp_res >= K) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    std::cout << result << "\n";
    return 0;
}