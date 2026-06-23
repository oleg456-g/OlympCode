#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using ld = long double;

#define all(a) begin(a), end(a)
#define len(a) int((a).size())

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);

    int n, m, k;
    cin >> n >> m >> k;
    vector<int> a(n);
    for (auto &x : a) {
        cin >> x;
    }
    vector<vector<int>> g(n);
    for (int i = 0; i < m; i++) {
        int v, u;
        cin >> v >> u;
        v--, u--;
        g[v].push_back(u);
        g[u].push_back(v);
    }

    vector<vector<int>> dist(n, vector<int>(n, n));
    for (int st = 0; st < n; st++) {
        queue<int> que;
        que.push(st);
        dist[st][st] = 0;
        while (!que.empty()) {
            int v = que.front();
            que.pop();
            for (auto u : g[v]) {
                if (dist[st][u] > dist[st][v] + 1) {
                    dist[st][u] = dist[st][v] + 1;
                    que.push(u);
                }
            }
        }
    }

    constexpr ll INF = 1e18;
    vector<array<ll, 11>> dp(n);
    for (auto &arr : dp) {
        arr.fill(INF);
    }

    priority_queue<tuple<ll, int, int>, vector<tuple<ll, int, int>>, greater<>> pq;
    dp[0][0] = 0;
    pq.emplace(0, 0, 0);
    for (int i = 1; i < n; i++) {
        if (dist[0][i] <= k) {
            dp[i][dist[0][i]] = 0;
            pq.emplace(0, i, dist[0][i]);
        }
    }

    ll ans = INF;

    while (!pq.empty()) {
        auto [d, v, x] = pq.top();
        pq.pop();
        if (d != dp[v][x]) {
            continue;
        }

        if (x + dist[v][n - 1] <= k) {
            ans = min(ans, d);
        }

        for (int u = 0; u < n; u++) {
            if (u == v) {
                continue;
            }
            int y = x + dist[v][u] - 1;
            if (y > k) {
                continue;
            }
            if (dp[u][y] > d + abs(a[u] - a[v])) {
                dp[u][y] = d + abs(a[u] - a[v]);
                pq.emplace(dp[u][y], u, y);
            }
        }
    }
    ans = min(ans, *min_element(all(dp[n - 1])));
    cout << ans << '\n';
}
