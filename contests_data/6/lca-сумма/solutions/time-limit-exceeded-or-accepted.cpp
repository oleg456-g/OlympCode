#include<bits/stdc++.h>
#define len(s) (int)s.size()
using namespace std;
const int N = 1e5 + 7;
const int inf = 1e9 + 7, mod = 1e9 + 7;
using ll = long long;
int n;


void dfs(int x, vector<vector<int>>&g, vector<bool>&used,
         vector<int>&depth, vector<int>&order, vector<int>&ind, int d=0){
    used[x] = true;
    depth.emplace_back(d);
    order.emplace_back(x);
    if(!ind[x]) ind[x] = int(depth.size());
    for(auto &v : g[x]){
        if(!used[v]) dfs(v, g, used, depth, order, ind, d+1);
        depth.emplace_back(d);
        order.emplace_back(x);
    }
}

int RMQ(int l, int r, vector<int>&depth) {
    int minv = inf, id = -1;
    for(int i = l; i <= r; i++){
        if(depth[i] < minv){
            minv = depth[i];
            id = i;
        }
    }
    return id;
}

int lca(int left, int right, vector<int>&depth, vector<int>&order){
    if(left > right) {
        swap(left, right);
    }
    int rmq = RMQ(left, right + 1, depth);
    return order[rmq];
}

int lca_subset(vector<bool>&vec, vector<int>&ind, vector<int>&depth, vector<int>&order){
    int prev_lca = 0;
    for(int i = 0; i < n; i++){
        if(vec[i]){
            if(!prev_lca) prev_lca = i + 1;
            else prev_lca = lca(ind[prev_lca] - 1, ind[i + 1] - 1, depth, order);
        }
    }
    return prev_lca;
}

void solve(){
    int root;
    cin >> n;
    vector<int>p(n+1);
    vector<vector<int>>g(n+1);

    for(int i = 1; i <= n; i++){
        cin >> p[i];
        if(i == p[i]) root = i;
        else g[p[i]].emplace_back(i);

    }

    vector<bool>used(n+1, false);
    vector<int>depth, order, ind(n + 1, 0);

    dfs(root, g, used, depth, order, ind);

    int prev_mask = 0;
    vector<bool>subset(n + 1, false);
    ll sum = 0;
    for(ll mask = 1; mask < (1ll << n); mask++){
        int new_mask = (mask >> 1) ^ mask;
        int difference = 0, a = new_mask, b = prev_mask;
        while(a % 2 == b % 2){
            difference++;
            a /= 2; b /=2;
        }
        if((new_mask >> difference) & 1) subset[difference] = true;
        else subset[difference] = false;
        sum = (sum + 1ll * lca_subset(subset, ind, depth, order)) % mod;
        prev_mask = new_mask;
    }
    cout << sum << "\n";
}
int main(){
    int t;
    cin >> t;
    while(t--) solve();
    return 0;
}
