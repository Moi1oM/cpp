#include<iostream>
#include<vector>
#include<set>

using namespace std;
vector<vector<int>> sdk(9, vector<int>(9));
bool flag = false;
bool row[9][10], col[9][10], square[9][10];

int get_square_idx(int x, int y) {
    return (x / 3) * 3 + (y / 3);
}

pair<int, int> next_idx(int a, int b) {
    if(b==8) return make_pair(a+1, 0);
    else return make_pair(a, b+1);
}

vector<int> available_nums(int a, int b) {
    vector<int> result;
    set<int> s;
    for(int i=1;i<=9;i++) {
        if(row[a][i])s.insert(i);
        if(col[b][i])s.insert(i);
        if(square[get_square_idx(a, b)][i])s.insert(i);
    }
    for(int i=1;i<=9;i++) {
        if(s.find(i)==s.end())result.push_back(i);
    }
    return result;
}

void dfs(int i, int j) {
    if(flag) return;
    if(i==9 && j==0) {
        for(auto sd: sdk) {
            for(auto x: sd) {
                cout << x;
            }
            cout << "\n";
        }
        flag = true;
        return;
    }
    
    auto [a, b] = next_idx(i, j);
    if(sdk[i][j]!=0) {
        dfs(a, b);
        return;
    }
    auto nums = available_nums(i, j);
    if (nums.size() == 0) return;
    
    // cout << i << " " << j << " : ";
    // for(auto num: nums) {
    //     cout << num << " ";
    // }
    // cout << "\n";
    
    for(auto num: nums) {
        sdk[i][j] = num;
        // cout << i << " " << j << " num : " << num << '\n';
        row[i][num] = true;
        col[j][num] = true;
        square[get_square_idx(i,j)][num] = true;
        dfs(a, b);
        row[i][num] = false;
        col[j][num] = false;
        square[get_square_idx(i,j)][num] = false;
        sdk[i][j] = 0;
    }
}

int main() {
    for(int i=0;i<9;i++) {
        string s;
        cin >> s;
        for(int j=0;j<9;j++) {
            int num = sdk[i][j] = s[j]-'0';
            if(num != 0) {
                row[i][num] = true;
                col[j][num] = true;
                square[get_square_idx(i, j)][num] = true;
            }
        }
    }
    dfs(0, 0);
    return 0;
}