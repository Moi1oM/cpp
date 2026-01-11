#include <iostream>
#include <vector>
#include <bitset>

long long dp[101][10][1024];
const int modules = 1000000000;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cout.tie(NULL);

    int n;
    std::cin  >> n;

    if (n<10) {
        std::cout << 0 << "\n";
        return 0;
    }

    for(int i=1;i<=9;i++) dp[1][i][1 << i] = 1;
    for(int len=1;len<n;len++) {
        for(int last=0;last<10;last++) {
            for(int mask=0;mask<1024;mask++) {
                if (dp[len][last][mask]) {
                    long long current = dp[len][last][mask];
                    if(last > 0) {
                        dp[len+1][last-1][mask|(1<<(last-1))] += current;
                        dp[len+1][last-1][mask|(1<<(last-1))] %= modules;
                    }
                    if(last < 9) {
                        dp[len+1][last+1][mask|(1<<(last+1))] += current;
                        dp[len+1][last+1][mask|(1<<(last+1))] %= modules;
                    }
                }
            }
        }
    }

    long long result = 0;
    for(int i=0;i<10;i++) {
        result += dp[n][i][1023];
        result %= modules;
    }
    std::cout << result << "\n";
    
    return 0;
}