#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator> // std::distance를 위해 포함

void stl_binary_search_comprehensive_example() {
    // 반드시 정렬된 벡터여야 합니다. (중복 값 포함)
    std::vector<int> data = {20, 10, 20, 50, 40, 30, 50};
    std::sort(data.begin(), data.end());
    int target = 20;

    std::cout << "--- C++ STL 이분 탐색 종합 예제 ---" << std::endl;
    std::cout << "Data: {10, 20, 20, 20, 30, 40, 50}" << std::endl;
    std::cout << "Target: " << target << std::endl;
    std::cout << "----------------------------------" << std::endl;

    // 1. std::binary_search
    // 단순히 값이 존재하는지 여부만 확인 (bool 반환)
    std::cout << "1. std::binary_search (존재 여부 확인): ";
    if (std::binary_search(data.begin(), data.end(), target)) {
        std::cout << "True (값이 존재함)" << std::endl;
    } else {
        std::cout << "False (값이 존재하지 않음)" << std::endl;
    }

    // 2. std::lower_bound
    // target(20)보다 크거나 같은 첫 번째 원소의 위치 (반복자)를 찾음
    auto low_it = std::lower_bound(data.begin(), data.end(), target);
    int low_index = std::distance(data.begin(), low_it);
    
    std::cout << "2. std::lower_bound (시작 위치): ";
    if (low_it != data.end() && *low_it == target) {
        std::cout << "Index " << low_index << ", Value: " << *low_it << std::endl;
        // Output: Index 1, Value: 20
    } else {
        std::cout << "Target 값의 시작 위치를 찾을 수 없거나 범위 밖에 있습니다." << std::endl;
    }
    
    // 3. std::upper_bound
    // target(20)보다 큰 첫 번째 원소의 위치 (반복자)를 찾음
    auto up_it = std::upper_bound(data.begin(), data.end(), target);
    int up_index = std::distance(data.begin(), up_it);

    std::cout << "3. std::upper_bound (끝 위치의 다음): ";
    std::cout << "Index " << up_index << ", Value: ";
    if (up_it != data.end()) {
        std::cout << *up_it << " (첫 번째 30)" << std::endl;
    } else {
        std::cout << "범위의 끝" << std::endl;
    }
    // Output: Index 4, Value: 30 (20들이 끝난 바로 다음 원소)

    // * lower_bound와 upper_bound를 이용한 중복 값 개수 계산
    std::cout << "-> 중복 값 개수: " << std::distance(low_it, up_it) << "개" << std::endl; 
    // Output: 4 - 1 = 3개

    // 4. std::equal_range
    // lower_bound와 upper_bound를 std::pair로 한 번에 반환
    auto range_pair = std::equal_range(data.begin(), data.end(), target);
    
    int range_start_index = std::distance(data.begin(), range_pair.first);
    int range_end_index = std::distance(data.begin(), range_pair.second);

    std::cout << "4. std::equal_range (범위 반환): " << std::endl;
    std::cout << "   - 시작 반복자 (lower_bound): Index " << range_start_index << std::endl;
    std::cout << "   - 끝 반복자 (upper_bound): Index " << range_end_index << std::endl;
}

int main() {
    stl_binary_search_comprehensive_example();
    return 0;
}