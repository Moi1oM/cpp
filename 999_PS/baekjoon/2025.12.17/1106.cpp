#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;

int main(){
    int C, N;
    const int INF = 1e9;
    cin >> C >> N;
    vector<pair<int, int>> v(N);
    for(int i=0;i<N;i++) {
        int a, b;
        cin >> a >> b;
        v[i] = make_pair(a, b); 
    }
    sort(v.begin(), v.end(), [](const pair<int, int>& x, const pair<int, int>& y) {
    return (long long)x.second * y.first < (long long)y.second * x.first;
    });
    // for(int i=0;i<N;i++) cout << v[i].first << " " << v[i].second << "\n";
    vector<int> dp(C+101, INF);
    dp[0]=0;
    
    for(int i=0;i<N;i++) {
        int cost = v[i].first, value = v[i].second;
        for(int j=value;j<=C+100;j++) {
            if(dp[j-value]!=INF) {
                dp[j] = min(dp[j], dp[j-value]+cost);
            }
        }
    }
    int ans = INF;
    for(int i=C; i<=C+100; i++) {
        ans = min(ans, dp[i]);
    }
    cout << ans << "\n";
    return 0;
}