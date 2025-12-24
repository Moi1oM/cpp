#include<iostream>
#include<vector>


// vec에서 num 보다는 작은 최대의 idx
int bi_search_idx(std::vector<int>& vec, int num) {
    int left = 0;
    int right = vec.size() - 1;
    int ans = -1; // 못 찾았을 경우 기본값

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (vec[mid] < num) {
            // mid 값이 num보다 작으므로 조건을 만족함.
            // 더 큰 값이 있는지 확인하기 위해 오른쪽 구간을 탐색
            ans = mid;      // 현재 인덱스 저장 (후보)
            left = mid + 1;
        } else {
            // vec[mid] >= num 이므로, 더 작은 값을 찾기 위해 왼쪽 구간 탐색
            right = mid - 1;
        }
    }

    return ans;
}

int main(){
    int n;
    std::cin >> n;
    std::vector<int> nums(n, 0), record(n, 0);
    for(auto& x: nums) std::cin >> x;
    std::vector<int> tails;
    for(int i=0;i<n;i++) {
        int val = nums[i];
        if(tails.empty() || tails[tails.size()-1] < val) {
            tails.push_back(val);
            record[i] = tails.size()-1;
        } else {
            int idx = bi_search_idx(tails, val);
                tails[idx+1] = val;
                record[i] = idx+1;
        }
    }
    int target_idx = tails.size()-1;
    std::vector<int> result;
    for(int i=n-1;i>=0;i--) {
        if(record[i] == target_idx) {
            result.push_back(nums[i]);
            target_idx--;
            if(target_idx<0) break;
        }
    }
    std::cout << result.size() << "\n";
    for(int i=result.size()-1;i>=0;i--) {
        std::cout << result[i] << " ";
    } 
    std::cout << "\n";
    return 0;
}