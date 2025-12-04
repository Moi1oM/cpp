#include <iostream>
#include <vector>

using namespace std;

int main(){
    vector<int> v;
    for(unsigned long i=1;i<=5;i++){
        v.push_back(i);
    }
    for(int i=0;i<v.size();i++){
        cout << v[i] << " ";
    }
    for(auto x: v) {
        cout << x << " ";
    }
    cout << "\n";
    if (!v.empty()){
        cout << "첫 요소 : " << v.front() << "\n";
        cout << "마지막 원소 요소 : " << v.back() << "\n";
    }


    v.erase(v.begin());
    v.insert(v.begin()+1, 99);
    return 0;
}