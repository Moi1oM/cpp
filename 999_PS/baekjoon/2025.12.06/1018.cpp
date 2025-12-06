#include<iostream>
#include<string>
#include<vector>

int check_diff(std::vector<std::string>& m) {
    int result = 65;
    std::string strArray[2][8] = {
        {"BWBWBWBW", "WBWBWBWB", "BWBWBWBW", "WBWBWBWB", 
        "BWBWBWBW", "WBWBWBWB", "BWBWBWBW", "WBWBWBWB"},
        {"WBWBWBWB", "BWBWBWBW", "WBWBWBWB", "BWBWBWBW", 
        "WBWBWBWB", "BWBWBWBW", "WBWBWBWB", "BWBWBWBW"}
    };

    for (int k=0;k<2;k++) {
        int tmp=0;

        for (int i=0;i<8;i++){
            for (int j=0;j<8;j++) {
                if(strArray[k][i][j] != m[i][j]) tmp +=1;
            }
        }
        if (tmp < result) result = tmp;
    }

    return result;
}

int main() {
    int N, M, result=65;
    std::cin >> N >> M;
    std::vector<std::string> v(N);
    for(int i=0;i<N;i++) std::cin >> v[i];

    for (int i=0;i<N-7;i++) {
        for (int j=0;j<M-7;j++) {
            std::vector<std::string> arg(N);
            for (int k=0; k<8; k++) {
                arg[k] = v[i+k].substr(j, 8);
            }
            int tmp = check_diff(arg);
            if (tmp < result) result = tmp;
        }
    }
    std::cout << result << "\n";
    return 0;
}