#include<iostream>
#include<vector>
#include<set>
#include<algorithm>

bool has_ban_element(const std::vector<int>& v, const std::set<int>& ban) {
    return std::any_of(v.begin(), v.end(),
        [&ban](int x) { return ban.count(x) > 0; });
}

int main(){
    int n,m,t,result;
    std::cin>>n>>m;
    std::vector<std::vector<int>> party(m, std::vector<int>());
    result=m;
    std::set<int> ban;
    std::cin>>t;
    std::vector<int> truths(t);
    for(auto& x:truths) {
        std::cin>>x;
        ban.insert(x);
    }
    for(size_t i=0;i<m;i++){
        int tmp_l;
        std::cin>>tmp_l;
        for(size_t j=0;j<tmp_l;j++){
            int tmp;
            std::cin>>tmp;
            party[i].push_back(tmp);
        }
    }
    bool flag=true;
    std::vector<char> checked(m, false);
    while(flag){
        flag=false;
        for(size_t i=0;i<m;i++) {
            if(checked[i])continue;
            auto v = party[i];
            if(has_ban_element(v,ban)) {
                result--;
                for(auto& x:v)ban.insert(x);
                flag=true;
                checked[i]=true;
            }
        }
    }
    std::cout<<result<<"\n";
    return 0;
}