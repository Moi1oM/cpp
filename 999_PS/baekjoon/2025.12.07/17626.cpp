#include<iostream>
#include<vector>
#include<algorithm>

int main(){
    int N, idx;
    std::cin >> N;
    std::vector<int> v;
    std::vector<int> dp(N+1, 1000);
    for(int i=1; i<=230; i++) {
        if(i*i > N) break;
        v.push_back(i*i);
        dp[i*i] = 1;
    }
    for (int i=2; i<=N; i++) {
        for (int j=0; j<v.size(); j++){
            idx = i - v[j];
            if(idx <= 0) break;
            dp[i] = std::min(dp[i], dp[idx]+1);
        }
    }

    std::cout << dp[N] << "\n";
    return 0;
}