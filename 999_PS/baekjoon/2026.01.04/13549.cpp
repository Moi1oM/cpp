#include <iostream>
#include <algorithm>
#include <deque>

const int MAX = 100001;
int N, K, visited[MAX];

int main() {
    std::cin >> N >> K;
    std::deque<int> dq;
    std::fill(visited, visited+MAX, -1);
    dq.push_back(N);
    visited[N] = 0;

    while (!dq.empty()) {
        int cur = dq.front();
        dq.pop_front();
        if (cur == K) {
            std::cout << visited[K] << "\n";
            return 0;
        }

        if (cur*2 < MAX && visited[cur*2] == -1) {
            visited[cur*2] = visited[cur];
            dq.push_front(cur*2);
        }

        if (cur >= 1 && visited[cur-1] == -1) {
            visited[cur-1] = visited[cur] + 1;
            dq.push_back(cur-1);
        }
        if (cur <= MAX-2 && visited[cur+1] == -1) {
            visited[cur+1] = visited[cur] + 1;
            dq.push_back(cur+1);
        }
    }

    return 0;
}