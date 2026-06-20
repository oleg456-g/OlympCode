#include "bits/stdc++.h"
using namespace std;

struct XorComp{
    static constexpr int MAX_STEP = 32; 
    array<int, MAX_STEP> count0{}, count1{};
    void add(int x){
        for(int i = 0; i < MAX_STEP; i++){
            ((x%2 ==0) ? count0[i] : count1[i]) += 1;
            x /= 2;  
        }
        // for(int i = 0; i < MAX_STEP; i++){
            // cout << count0[i] << " ";
        // }
        // cout << endl;
        // for(int i = 0; i < MAX_STEP; i++){
            // cout << count1[i] << " ";
        // }
        // cout << endl;
    
    } 
    long long make_xor(int x) const{
        // cout << "x: " << x << endl;
        long long t = 1, res = 0;

        for(int i = 0; i < MAX_STEP; i++){
            // cout << x%2 << " ";
            res += t * ((x%2 == 1) ? count0[i] : count1[i]);
            t*=2;
            x/=2;
        }
        // cout <<endl;

        return res;
    }
};


struct Tree{
    int root = 0;
    vector<vector<int>> gr;
    vector<long long> xor_val;
    vector<int> is_leaf, weight;
    XorComp comp;
    long long sum = 0;
    void init(const vector<int> &w, const vector<int> &pred){
        gr.resize(pred.size());
        weight = w;
        xor_val.resize(pred.size());
        is_leaf.resize(pred.size());

        for(int i = 0; i < pred.size(); i++){
            if(i == pred[i]){
                root = i;
            }
            else{
                gr[pred[i]].push_back(i);
            }
        }
        dfs(root, 0);
        // cout << root << " " << sum << endl;
    }
    void dfs(int v, long long val){
        val ^= weight[v];
        xor_val[v] = val;
        if(gr[v].empty()){   
            comp.add(val);
            // cout << "v: " << v << "\tval:" << val <<endl;;
            is_leaf[v] = 1;
            sum += val;
        }
        for(auto i : gr[v]){
            dfs(i, val);
        }
    }


    long long podv(const Tree& other, int v){
        long long res = sum;
        if(is_leaf[v]){
            res -= xor_val[v];
        }
        // cout << "v: " << v << " res: " << res << endl;
        res += other.comp.make_xor(xor_val[v]);
        // cout << "v: " << v << " res: " << res << endl;
        return res;
    }
};


void solve(){
    int n1;
    cin >> n1;
    vector<int> weight1(n1);
    for(auto &i : weight1){
        cin >> i;
    }
    vector<int> pred1(n1);
    for(auto &i : pred1){
        cin >> i;
        i--;
    }

    int n2;
    cin >> n2;
    vector<int> weight2(n2);
    for(auto &i : weight2){
        cin >> i;
    }
    vector<int> pred2(n2);
    for(auto &i : pred2){
        cin >> i;
        i--;
    }
    Tree t1, t2;
    t1.init(weight1, pred1);
    t2.init(weight2, pred2);

    long long ans = -1;
    for(int i = 0; i < n1; i++){
        ans = max(ans, t1.podv(t2, i));
    }
    for(int i = 0; i < n2; i++){
        ans = max(ans, t2.podv(t1, i));
    }
    cout << ans << endl;

    

}


int main(){
    int t;
    cin >> t;
    while(t--){
        solve();
    }
    
}