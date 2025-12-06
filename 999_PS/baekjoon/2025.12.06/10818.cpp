#include<iostream>
#include<vector>
#include<algorithm>

int main(){
    int N;
    std::cin>>N;
    std::vector<int> v;
    while(N--){
        int tmp;
        std::cin >> tmp;
        v.push_back(tmp);
    }
    std::cout << *max_element(v.begin(), v.end())
       << " " << *min_element(v.begin(), v.end()) << "\n";
    return 0;
}