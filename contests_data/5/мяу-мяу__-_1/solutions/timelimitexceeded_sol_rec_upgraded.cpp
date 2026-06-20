#include "bits/stdc++.h"

using namespace std;

using ll = long long;

int const MOD = 998244353;

int mul(ll x, int y) {
    return int(x * y % MOD);
}

void solveTest() {
    int n;
    cin >> n;
    vector adj(n, vector<int>(n));
    vector<vector<int>> graph(n);
    for (int i = 0; i < n - 1; ++i) {
        int u, v;
        cin >> u >> v;
        --u, --v;
        adj[u][v] = adj[v][u] = 1;
        graph[v].push_back(u);
        graph[u].push_back(v);
    }
    vector<int> d(n);
    auto dfs_pre = [&](auto self, int v, int p = -1) -> void {
        for (int u : graph[v]) {
            if (u == p) {
                continue;
            }
            d[u] = d[v] + 1;
            self(self, u, v);
        }
    };
    dfs_pre(dfs_pre, 0);

    vector<bool> used(n);
    vector<int> p{0};
    used[0] = true;
    int ans = 0;
    auto rec = [&](auto self, int i) -> void {
        if (i == n) {
            ans += 1;
            return;
        }
        for (int val = 0; val < n; ++val) {
            if (used[val] || d[val] != d[i]) {
                continue;
            }
            used[val] = true;
            p.push_back(val);

            bool ok = true;
            for (int j = 0; j < i; ++j) {
                if (adj[i][j] != adj[p[i]][p[j]]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                self(self, i + 1);
            }

            p.pop_back();
            used[val] = false;
        }
    };

    rec(rec, 1);
    cout << ans << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t; cin >> t;
    while (t--) {
        solveTest();
    }

    return 0;
}
