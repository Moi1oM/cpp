#include<iostream>

int main(){
    int n;
    std::cin >> n;
    while (n--) {
        int r;
        std::string s;
        std::cin >> r >> s;
        for (auto x: s) {
            for(int i=0;i<r;i++) std::cout << x;
            //std::cout << std::string(r, x);
        }
        std::cout << "\n";
    }
    return 0;
}