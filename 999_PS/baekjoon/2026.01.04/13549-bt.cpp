#include <iostream>
#include <algorithm>

int N, K, dist[100001];
int solve(int k) {
    if (k<=N) return N-k;

    if (k%2==0) { //짝수
        return std::min(solve(k/2),k-N);
    } else { //홀수
        return std::min(solve(k-1), solve(k+1))+1;
    }
}

int main() {
    std::cin >> N >> K;
    std::cout << solve(K) << "\n";
    return 0;
}