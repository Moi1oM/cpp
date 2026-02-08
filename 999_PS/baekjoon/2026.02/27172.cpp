#include<iostream>
#include<vector>

const int MAX_VAL = 1000000;
int card_exists[MAX_VAL + 1];
int scores[MAX_VAL + 1];
int input_cards[100001];

int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    int n;
    std::cin>>n;
    for(int i=0;i<n;i++) {
        std::cin>>input_cards[i];
        card_exists[input_cards[i]]=1;
    }
    for(int i=0;i<n;i++) {
        int current_card = input_cards[i];
        for(int mult=current_card*2;mult<=MAX_VAL;mult+=current_card) {
            if (card_exists[mult]) {
                scores[mult]--;
                scores[current_card]++;
            }
        }
    }
    for(int i=0;i<n;i++) std::cout<<scores[input_cards[i]]<<" ";
    std::cout<<"\n";
    return 0;
}