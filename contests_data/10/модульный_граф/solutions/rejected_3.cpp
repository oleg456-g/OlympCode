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
ll dist[SZ], used[SZ];
 
void solve() {
    cin >> n >> m >> qk;
    for (int i = 0; i < n; ++i) cin >> vals[i];
    for (int i = 0; i < m; ++i) { ll u, v; cin >> u >> v; --u; --v; gr[u].pb(v); gr[v].pb(u); }
 
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> q;
    for (int i = 0; i < n; ++i) dist[i] = INF;
    dist[0] = 0;
    
    q.push({0, 0});
    while (!q.empty()) {
        pair<ll, int> cur = q.top();
        q.pop();
        if (used[cur.second]) continue;
        used[cur.second] = 1;

        for (auto to : gr[cur.second]) {
            if (dist[to] > cur.first + abs(vals[to] - vals[cur.second])) {
                dist[to] = cur.first + abs(vals[to] - vals[cur.second]);
                q.push({dist[to], to});
            }
        }
    }
    
    cout << dist[n - 1] << "\n";
}
 
int main() {
    ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
 
    ll t = 1;
    while (t--) {
        solve();
    }
 
    return 0;
}
