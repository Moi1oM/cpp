#include<iostream>
#include<vector>
#include<algorithm>
#include<deque>

int main(){
    int n,m;
    std::cin>>n;
    std::vector<int> n_v(n);
    for(auto& x:n_v){
        std::cin>>x;
    }
    std::cin>>m;
    std::vector<int> m_v(m);
    for(auto& x:m_v){
        std::cin>>x;
    }
    std::vector<std::vector<int>>dp(n+1, std::vector(m+1, 0));
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(n_v[i-1]==m_v[j-1]){
                dp[i][j]=dp[i-1][j-1]+1;
            }
            dp[i][j]=std::max(dp[i-1][j],dp[i][j-1]);
        }
    }
    std::deque<int> result;
    int n_i=n, m_i=m;
    while(n_i>0 && m_i>0) {
        if(n_v[n_i-1]==m_v[m_i-1]){
            int val = n_v[n_i-1];
            result.push_front(val);
        } else if(dp[n_i-1][m_i] >= dp[n_i][m_i-1]) {
            n_i--;
        } else {
            m_i--;
        }
    }
    auto it = std::max_element(result.begin(), result.end());
    int max_index = std::distance(result.begin(), it);
    std::cout<<dp[n][m]<<"\n";
    for (size_t i = max_index; i < result.size(); ++i) {
        std::cout << result[i] << " ";
    }
    std::cout << "\n";

    return 0;
}