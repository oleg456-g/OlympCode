#include <bits/stdc++.h>
 
using namespace std;
 
typedef long long ll;
#define all(a) begin(a), end(a)
#define len(a) int((a).size())
#define pb emplace_back
 
const ll SZ = 2010, K = 10, INF = 1'000'000'000'000'000'000;
 
int n, m, qk;
int vals[SZ];
vector<int> gr[SZ];
int dists[SZ][SZ];
 
void bfs(int s) {
    for (int i = 0; i < n; ++i) dists[s][i] = SZ;
    dists[s][s] = 0;
    queue<int> q;
    q.push(s);
 
    while (!q.empty()) {
        int v = q.front(); 
        q.pop();
        for (auto to : gr[v]) {
            if (dists[s][to] > dists[s][v] + 1) { 
                q.push(to); 
                dists[s][to] = dists[s][v] + 1;
            }
        }
    }
}
 
ll dp[SZ][K + 1];
 
void solve() {
    cin >> n >> m >> qk;
    for (int i = 0; i < n; ++i) cin >> vals[i];
    for (int i = 0; i < m; ++i) { ll u, v; cin >> u >> v; --u; --v; gr[u].pb(v); gr[v].pb(u); }
 
    for (int i = 0; i < n; ++i) bfs(i);
    for (int k = 0; k <= K; ++k) {
        for (int i = 0; i < n; ++i) dp[i][k] = INF;
        dp[0][k] = 0;
    }
    
    ll ans = INF;
    for (int k = 0; k <= K; ++k) {
        priority_queue<pair<ll, int>, std::vector<pair<ll, int>>, std::greater<pair<ll, int>>> q;
        vector<int> used(n, 0);
        for (int i = 0; i < n; ++i) {
            if (k) dp[i][k] = min(dp[i][k], dp[i][k - 1]);
            if (dists[0][i] <= k) dp[i][k] = 0;
            q.push({dp[i][k], i});
        }
        while (!q.empty()) {
            pair<ll, int> v = q.top();
            q.pop();
            if (used[v.second]) continue;
            used[v.second] = 1;
 
            for (auto to : gr[v.second]) {
                if (dp[to][k] > dp[v.second][k] + abs(vals[v.second] - vals[to])) {
                    dp[to][k] = dp[v.second][k] + abs(vals[v.second] - vals[to]);
                    q.push({dp[to][k], to});
                }
            }
        }
 
        for (int fr = 0; fr < n; ++fr) {
            for (int to = 0; to < n; ++to) {
                if (dists[fr][to] >= 2 && dists[fr][to] <= 10 && k + dists[fr][to] - 1 <= K) {
                    dp[to][k + dists[fr][to] - 1] = min(dp[to][k + dists[fr][to] - 1], dp[fr][k] + abs(vals[fr] - vals[to]));
                }
            }
        }
        
        for (int i = 0; i < n; ++i) {
            if (dists[i][n - 1] <= (qk - k)) ans = min(ans, dp[i][k]);
        }
    }
    
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
