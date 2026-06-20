#include "bits/stdc++.h"
using namespace std;


constexpr double TL = 1.98;

double get_time() {
    return double(clock()) / double(CLOCKS_PER_SEC);
}

struct Tree{
    int root = 0;
    vector<vector<int>> gr;
    vector<long long> xor_val;
    vector<int> is_leaf, weight;
    vector<int> leafs;
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
        
    }
    void dfs(int v, long long val){
        val ^= weight[v];
        xor_val[v] = val;
        if(gr[v].empty()){   
            is_leaf[v] = 1;
            leafs.push_back(v);
            sum += val;
        }
        for(auto i : gr[v]){
            dfs(i, val);
        }
    }
    long long sum_dfs(int v, long long val) const{
        long long sum = 0;
        for (auto v : leafs)
        {
            sum += xor_val[v] ^ val;   
        }
        
        return sum;
    }


    long long podv(const Tree& other, int v){
        long long res = sum;

        res -= xor_val[v] * is_leaf[v];
        res += other.sum_dfs(other.root, xor_val[v]);

        return res;
    }
};



struct TimeBudget{
    static double end_time;
    static void set_budget(double time){
        end_time = get_time() + time; 
    }
    static bool check(){
        return get_time() < end_time;
    }
};
double TimeBudget::end_time = 0.0;

static TimeBudget budget;

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

    
   vector<pair<int, int>> pers;
   pers.reserve(n1+n2);
   for (int i = 0; i < n1; i++)
   {
    pers.push_back({i, 0});
   }
   for (int i = 0; i < n2; i++)
   {
    pers.push_back({i, 1});
   }
   mt19937 rnd(911);
   std::shuffle(pers.begin(), pers.end(), rnd);

   int i = 0;
   long long ans = 0;

   while(i < pers.size() && budget.check()){
    auto [ver, root_tree_id] = pers[i];
    auto &root_tree = (root_tree_id ? t2 : t1);
    auto &other_tree = (root_tree_id ? t1 : t2);
    auto val = root_tree.podv(other_tree, ver);
    ans = max(ans, val);
    i++;
   }

//    cout << "i: " << i << " pers.size():" << pers.size() << endl;
   
   cout << ans << '\n';

    

}


int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    auto start_time = get_time();
    int t;
    cin >> t;
    for(int i = 0; i < t; i++){
        auto time_per_test = max(1.0, TL / t);
        budget.set_budget(1);
        solve();
    }
    
}