#include<iostream>
#include<vector>
#include<cmath>

class SegmentTree {
public:
    std::vector<long long> tree;

    long long search_tree(long long start, long long end,
                            long long t_s_i, long long t_e_i, long long index) {
        
        if (start > t_e_i|| end < t_s_i) return 0;
        if (start <= t_s_i && t_e_i <= end) return tree[index];
                            
        long long t_mid_i = (t_s_i+t_e_i) /2;
        long long left_result = search_tree(start, end, t_s_i, t_mid_i, index*2);
        long long right_result = search_tree(start, end, t_mid_i+1, t_e_i, index*2+1);
        return left_result+right_result;
    }

    void update_tree(long long target_index, long long diff,
                    long long start, long long end, long long tree_index) {
        if(target_index < start || target_index > end) return;
        tree[tree_index] += diff;

        if (start != end) {
            long long mid = (start+end)/2;
            update_tree(target_index, diff, start, mid, tree_index*2);
            update_tree(target_index, diff, mid+1, end, tree_index*2+1);
        }
    }

    long long make_tree(std::vector<long long>& vec, 
                    std::vector<long long>& tree,
                    long long v_start,
                    long long v_end,
                    long long t_index) {
                        
        if(v_start == v_end) {
            tree[t_index] = vec[v_start];
            return vec[v_start];
        }

        long long v_mid = (v_start+v_end) / 2;
        long long left_value = make_tree(vec, tree, v_start, v_mid, t_index*2);
        long long right_value = make_tree(vec, tree, v_mid+1, v_end, t_index*2 + 1);
        long long value = left_value+right_value;
        tree[t_index] = value;
        return value;
    }

    SegmentTree(std::vector<long long>& vec) {
        int n = vec.size();
        long long height = (long long)(ceil(log2(n)));
        long long max_length = (1 << (height+1));
        tree.resize(max_length+1);

        make_tree(vec, tree, 0, vec.size()-1, 1);
        // for(auto& x: tree) {
        //     std::cout << x << " ";
        // }
    }


};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    long long N, M, K;
    std::cin >> N >> M >> K;
    std::vector<long long> v(N);
    for(auto& x: v) std::cin >> x;
    SegmentTree st(v);

    for(long long x=0;x<M+K;x++) {
        long long t, a, b;
        std::cin >> t >> a >> b;
        if(t == 1) {
            long long value = v[a-1];
            long long diff = b - value;
            v[a-1] = b;
            st.update_tree(a-1, diff, 0, v.size()-1, 1);
        }
        if (t == 2) {
            std::cout << st.search_tree(a-1, b-1, 0, v.size()-1, 1) << "\n";
        }
    }
    return 0;
}