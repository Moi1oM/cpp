#include<iostream>
#include<vector>

int main(){
    int n;
    std::cin >> n;
    std::vector<int> v(n, 0);
    for(auto& x : v) std::cin >> x;
    std::vector<int> result = {v[0], v[n-1]};
    int l_idx = 0, r_idx=n-1;
    while (l_idx<r_idx) {
        int l_value = v[l_idx], r_value = v[r_idx];
        int now_val = l_value+r_value, prev_val = result[0]+result[1];
        if(std::abs(now_val) < std::abs(prev_val)) {
            result[0] = l_value;
            result[1] = r_value;
        }
        if(now_val == 0) {
            break;
        } else if (now_val < 0) {
            l_idx++;
        } else {
            r_idx--;
        }
    }
    std::cout << result[0] << " " << result[1] << "\n";
    return 0;
}