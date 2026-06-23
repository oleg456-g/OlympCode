#include <bits/stdc++.h>
 
using namespace std;
 
typedef long long ll;
#define all(a) begin(a), end(a)
#define len(a) int((a).size())
#define pb emplace_back
 
const ll SZ = 2010, K = 10, INF = 1'000'000'000'000'000'000, MAXW = 12;
 
int n, m, qk;
int vals[SZ];
vector<int> gr[SZ];
ll dp[SZ][K + 1];
 
vector<ll> path, bamboo;
 
void dfs(int v, int par, int dest) {
    path.push_back(v);
    if (dest == v) bamboo = path;
 
    for (auto to : gr[v]) {
        if (to != par) {
            dfs(to, v, dest);
        }
    }
    path.pop_back();
}
 
void solve() {
    cin >> n >> m >> qk;
    for (int i = 0; i < n; ++i) cin >> vals[i];
    for (int i = 0; i < m; ++i) { ll u, v; cin >> u >> v; --u; --v; gr[u].pb(v); gr[v].pb(u); }
 
    bamboo.clear();
    path.clear();
    for (int i = 0; i < n; ++i) bamboo.push_back(i);
 
    for (int i = 0; i < bamboo.size(); ++i) {
        for (int k = 0; k <= K; ++k) {
            dp[i][k] = INF;
        }
    }
 
    ll ans = INF;
    for (int i = 0; i < bamboo.size(); ++i) {
        for (int k = 0; k <= K; ++k) {
            if (k >= i) {
                dp[i][k] = 0;
            }
            for (int t = 0; t <= K; ++t) {
                if (t + k <= K && i + t + 1 < bamboo.size()) {
                    dp[i + t + 1][t + k] = min(dp[i + t + 1][t + k], dp[i][k] + abs(vals[bamboo[i]] - vals[bamboo[i + t + 1]]));
                }
            }
 
            int left = int(bamboo.size()) - i - 1;
            if (left <= qk - k) {
                ans = min(ans, dp[i][k]);
            }
        }
    }
 
    cout << ans << "\n";
    return;
}
 
int main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
 
    ll t = 1;
    while (t--) {
        solve();
    }
 
    return 0;
}
