#include <bits/stdc++.h>
 
using namespace std;
 
typedef long long ll;
#define all(a) begin(a), end(a)
#define len(a) int((a).size())
#define pb emplace_back
 
const ll SZ = 2010, K = 10, INF = 1'000'000'000'000'000'000, MAXW = 21;
 
int n, m, qk;
int vals[SZ];
vector<int> gr[SZ];
ll dp[SZ][MAXW][K + 1];
bool used[SZ][MAXW][K + 1];
 
void solve() {
    cin >> n >> m >> qk;
    for (int i = 0; i < n; ++i) cin >> vals[i];
    for (int i = 0; i < m; ++i) { ll u, v; cin >> u >> v; --u; --v; gr[u].pb(v); gr[v].pb(u); }
 
    for (int k = 0; k <= K; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int w = 0; w < MAXW; ++w) dp[i][w][k] = INF;
        }
    }
    dp[0][vals[0]][0] = 0;
 
    for (int k = 0; k <= qk; ++k) {    
        priority_queue<pair<ll, pair<int, int>>, std::vector<pair<ll, pair<int, int>>>, std::greater<pair<ll, pair<int, int>>>> qs;
        for (int i = 0; i < n; ++i) {
            for (int w = 0; w < MAXW; ++w) {
                if (k) dp[i][w][k] = min(dp[i][w][k], dp[i][w][k - 1]);
                qs.push({dp[i][w][k], {i, w}});
            }
        }
        while (!qs.empty()) {
            pair<ll, pair<int, int>> v = qs.top();
            qs.pop();
            auto [i, curw] = v.second;
            if (used[i][curw][k]) continue;
            used[i][curw][k] = 1;
 
            for (auto to : gr[i]) {
                for (int w = 0; w < MAXW; ++w) {
                    ll dst = dp[to][w][k + (w != vals[to])];
                    if (dst > dp[i][curw][k] + abs(curw - w)) {
                        dp[to][w][k + (w != vals[to])] = dp[i][curw][k] + abs(curw - w);
                        if (w == vals[to]) {
                            qs.push({dp[to][w][k + (w != vals[to])], {to, w}});
                        }
                    }
                }
            }
        }
    }
 
    ll ans = INF;
    for (int w = 0; w < MAXW; ++w) ans = min(ans, dp[n - 1][w][qk]);
 
    cout << ans << "\n";
}
 
int main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
 
    ll t = 1;
    while (t--) {
        solve();
    }
 
    return 0;
}
