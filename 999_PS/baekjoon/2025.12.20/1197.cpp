#include<iostream>
#include<queue>
#include<functional>

using namespace std;

struct line {
    int cost, a, b;
};

struct cmpt {
    bool operator()(line x, line y) {
        return x.cost > y.cost;
    }
};

int parent[10001];
int find_parent(int x) {
    if(parent[x]==x) return x;
    int tmp = find_parent(parent[x]);
    parent[x] = tmp;
    return tmp;
}

void merge_two(int a, int b) {
    int a_parent = find_parent(a), b_parent = find_parent(b);
    if (a_parent > b_parent) swap(a_parent, b_parent);
    parent[b_parent] = a_parent;
}

int main(){
    priority_queue<line, vector<line>, cmpt> pq;
    int V, E, break_point = 0;
    long long result = 0;
    cin >> V >> E;
    for (int i=1;i<=V;i++) parent[i]=i;
    for(int i=0;i<E;i++) {
        int a, b, cost;
        cin >> a >> b >> cost;
        pq.push({cost, a, b});
    }
    while (!pq.empty()) {
        line cur = pq.top();
        int cost = cur.cost, a=cur.a, b= cur.b;
        pq.pop();
        if (find_parent(a) != find_parent(b)) {
            result += cost;
            break_point++;
            merge_two(a, b);
        }
        if (break_point == V-1) break;
    }

    cout << result << "\n";
    return 0;
}