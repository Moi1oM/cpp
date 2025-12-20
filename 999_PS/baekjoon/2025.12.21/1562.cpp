#include<iostream>
#include<vector>
#include<algorithm>

long long result;
int n;

void make_num(int now, int depth, std::vector<char>& mark);

void call_next(int now, int last_ed, int depth, std::vector<char>& mark) {
    int branch = now*10 + last_ed;
    if (mark[last_ed]) {
        make_num(branch, depth+1, mark);
    } else {
        mark[last_ed] = true;
        make_num(branch, depth+1, mark);
        mark[last_ed] = false;
    }
}

void make_num(int now, int depth, std::vector<char>& mark) {
    if(depth >= n) {
        if (std::find(mark.begin(), mark.end(), false) == mark.end()) {
            result += 1;
            if (result >= 1000000000) result -= 1000000000;
        }
        return;
    }
    int last;
    last = now % 10;
    if (last != 0 && last!=9) {
        call_next(now, last+1, depth, mark);
        call_next(now, last-1, depth, mark);
    } else if (last == 0) {
        call_next(now, last+1, depth, mark);
    } else { //last == 9
        call_next(now, last-1, depth, mark);
    }
}

int main() {
    std::cin >> n;
    for(int i=1;i<=9;i++) {
        std::vector<char> marking(10, false);
        marking[i] = true;
        make_num(i, 1, marking);
    }
    std::cout << result << "\n";
    return 0;
}