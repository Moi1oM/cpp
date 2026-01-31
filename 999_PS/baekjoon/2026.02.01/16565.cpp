#include<iostream>
#include<vector>
const int MODNUM = 10007;
int pascal[53][53];

void make_pascal() {
    for(int i=0;i<53;i++) {
        pascal[i][0] = 1;
        for(int j=1;j<=i;j++) {
            pascal[i][j]=(pascal[i-1][j]+pascal[i-1][j-1])%MODNUM;
        }
    }
}

int main(){
    int n, result=0;
    std::cin >> n;
    make_pascal();

    if (n >= 4) {
        int iter_num = n / 4;
        int plus_minus = 1;

        for (int k = 1; k <= iter_num; k++) {
            int term = (pascal[13][k] * pascal[52 - 4 * k][n - 4 * k]) % MODNUM;

            if (plus_minus == 1) {
                result = (result + term) % MODNUM;
            } else {
                result = (result - term + MODNUM) % MODNUM;
            }
            plus_minus *= -1;
        }
    }    std::cout<<result<<'\n';
    return 0;
}