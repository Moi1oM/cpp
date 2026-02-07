#include <iostream>
#include <vector>
void cmp(int i, int j, std::vector<int>& v, std::vector<int>& nums) {
    int v_i=nums[i], v_j=nums[j];
    if(v_i<v_j) {
        int remain = v_j%v_i;
        if(remain==0) {
            v[i]++;
            v[j]--;
        }
    } else { // v_i>v_j
        int remain = v_i%v_j;
        if(remain==0) {
            v[j]++;
            v[i]--;
        }
    }
}

int main() {
    int n;
    std::cin>>n;
    std::vector<int> nums(n), results(n, 0);
    for (size_t i = 0; i < n; i++) {
        std::cin >> nums[i];
    }
    for(size_t i=0;i<n;i++) {
        for(size_t j=i+1;j<n;j++) {
            cmp(i,j,results, nums);
        }
    }
    for(size_t i=0;i<n;i++) {
        std::cout<<results[i]<<" ";
    }
    std::cout<<"\n";
    return 0;
}