#include<bits/stdc++.h>
#define len(s) (int)s.size()
using namespace std;
const int N = 1e5 + 7;
const int inf = 1e9 + 7, mod = 1e9 + 7;
using ll = long long;
int n;

ll modpow[N];


void fill_pow(){
    modpow[0] = 1ll;
    for(int i = 1; i < N; i++) modpow[i] = (modpow[i - 1] *  2) % mod;
    for(long long & i : modpow) i = (i - 1) % mod;
}

pair<int, int> dfs(int x, vector<vector<int>>&g, vector<ll>&res){
    if(g[x].empty()){
        res[x] = 1;
        return {1, 1};
    }
    ll child_sum = 0ll;
    int child_num = 0;
    for(auto &v : g[x]){
        pair<int, int>ret = dfs(v, g, res);
        child_sum = (child_sum + ret.first) % mod;
        child_num += ret.second;
    }
    child_num++;
    ll cur_sum = (modpow[child_num] - child_sum) % mod;
    res[x] = cur_sum;
    return {(cur_sum + child_sum) % mod, child_num};
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

    vector<ll>res(n+1, 0ll);

    dfs(root, g, res);
    ll sum = 0;
    for(int i = 1; i <= n; i++) {
        ll add = (res[i] * i) % mod;
        sum = (sum + add) % mod;
    }
    if (sum < 0) sum += mod;
    cout << sum << "\n";

}
int main(){
    ios_base :: sync_with_stdio(false);
    cin.tie(nullptr);
    fill_pow();
    int t;
    cin >> t;
    while(t--) solve();
    return 0;
}
