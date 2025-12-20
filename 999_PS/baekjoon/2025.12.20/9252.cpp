#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

std::vector<int> result;

void dfs(int i, int j, std::vector<std::vector<int>>& map) {
    if (map[i][j]==0 || i<0 || j<0) return;
    if(map[i][j] == map[i-1][j]) {
        dfs(i-1, j, map);
    } else if(map[i][j] == map[i][j-1]) {
        dfs(i, j-1, map);
    } else if (map[i][j] == map[i-1][j-1]+1) {
        result.push_back(i-1);
        dfs(i-1, j-1, map);
    }
}

int main(){
    std::string s1, s2;
    std::cin >> s1;
    std::cin >> s2;
    int s1_l = s1.size(), s2_l = s2.size();
    std::vector<std::vector<int>> dp(s1_l + 1, std::vector<int>(s2_l + 1, 0));

    for(int i=1;i<=s1_l;i++) {
        for(int j=1;j<=s2_l;j++) {
            if(s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1]+1;
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }

    std::cout << dp[s1_l][s2_l] << "\n";
    if (dp[s1_l][s2_l]!=0) {

        dfs(s1_l, s2_l, dp);
        
        for(int i=result.size()-1;i>=0;i--) {
            std::cout<<s1[result[i]];
        }
    }
    std::cout << "\n";

    return 0;
}