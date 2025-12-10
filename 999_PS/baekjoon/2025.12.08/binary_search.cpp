#include<iostream>
#include<vector>

int binarySearch(std::vector<int>& v, int t) {
    int left=0, right=v.size()-1;
    while (left <= right) {
        int mid = left + (right-left) / 2;
        if (v[mid] == t) {return mid;}
        else if (v[mid] > t) {
            right = mid - 1;
        } else { // v[mid] < t
            left = mid + 1;
        }
    }
    return -1;
}

int main() {
    std::vector<int> data = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    int target = 23;
    int index = binarySearch(data, target);

    std::cout << index << "\n";

    return 0;
}