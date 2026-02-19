#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const long long MOD = 1000000007;

int main() {
    int N;
    if (!(cin >> N)) return 0;

    vector<long long> v(N);
    for (int i = 0; i < N; i++) cin >> v[i];
    sort(v.begin(), v.end());

    vector<long long> pow2(N);
    pow2[0] = 1;
    for (int i = 1; i < N; i++) {
        pow2[i] = (pow2[i - 1] * 2) % MOD;
    }

    long long max_sum = 0;
    long long min_sum = 0;

    for (int i = 0; i < N; i++) {
        max_sum = (max_sum + (v[i] * pow2[i])) % MOD;
        min_sum = (min_sum + (v[i] * pow2[N - 1 - i])) % MOD;
    }

    long long result = (max_sum - min_sum + MOD) % MOD;
    cout << result << endl;

    return 0;
}