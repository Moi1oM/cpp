#include <iostream>
#include <vector>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    vector<vector<int>> ar(N + 1, vector<int>(N + 1));
    vector<vector<int>> ar_sum(N + 1, vector<int>(N + 1, 0));

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            cin >> ar[i][j];
            // 구간 합 공식: 현재값 + 위쪽누적 + 왼쪽누적 - 중복더해진 대각선누적
            ar_sum[i][j] = ar[i][j] + ar_sum[i-1][j] + ar_sum[i][j-1] - ar_sum[i-1][j-1];
        }
    }

    for (int i = 0; i < M; i++) {
        int x1, y1, x2, y2;
        cin >> x1 >> y1 >> x2 >> y2;

        int result = ar_sum[x2][y2] 
                   - ar_sum[x1-1][y2] 
                   - ar_sum[x2][y1-1] 
                   + ar_sum[x1-1][y1-1];

        cout << result << "\n";
    }

    return 0;
}