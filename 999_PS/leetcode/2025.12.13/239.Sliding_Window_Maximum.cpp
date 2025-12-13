#include<vector>
#include<iostream>
#include<deque>

class Solution {
public:
    static std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
        std::vector<int> result;
        std::deque<int> dq;

        for (int i=0;i<nums.size();i++) {
            if (i>=k) { 
                // TODO : 맨 앞에 pop_front 하기.
                if(!dq.empty() && dq.front() <= i-k) dq.pop_front();
            }
            
            while(!dq.empty() && nums[dq.back()] <= nums[i]) {
                dq.pop_back();
            }
            dq.push_back(i);

            // TODO : 최대값 구해서 result에 넣기
            if (i >= k-1) {
                result.push_back(nums[dq.front()]);
            }
        }

        return result;
    }
};

int main(){
    std::vector<int> nums = {4,5,2,3,1,6};
    std::vector<int> nums2 = {1,3,-1,-3,5,3,6,7};
    int k = 3;
    auto result = Solution::maxSlidingWindow(nums, k);
    for(auto x : result) {
        std:: cout << x << " " ;
    }
    std::cout<<"\n";
    return 0;
}