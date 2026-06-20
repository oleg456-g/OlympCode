#ifdef LOCAL
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <cmath>
#include <deque>
#include <random>
#include <bitset>
#include <map>
#include <cassert>
#include <complex>
#include <cmath>
#include <time.h>
#else
#include "bits/stdc++.h"
#endif
 
using namespace std;
using cd = complex<double>;
 
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
 
#define ll int64_t
#define ld long double
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define x first
#define y second
 
#define double ld
#define endl '\n'
 
const int inf = 1e9;
const ll INF = 1e18;
 
const int maxn = 15;
vector<pair<int, ll> > g[maxn + 1];
vector<pair<pair<int, int>,ll> > edges;
 
struct DSU {
    vector<int> tree;
 
    DSU(int sz) {
        tree.resize(sz);
        iota(all(tree), 0);
    }
 
    int get(int u) {
        if (tree[u] == u) {
            return u;
        }
        return tree[u] = get(tree[u]);
    }
    
    void clear() {
        iota(all(tree), 0);
    }
 
    void join(int u, int v) {
        tree[get(u)] = get(v);
    }
};
 
void solve() {
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; ++i) {
        g[i].clear();
    }
    edges.clear();
    for (int i = 0; i < m; ++i) {
        int a, b, c;
        cin >> a >> b >> c;
        --a, --b;
        edges.push_back({{a, b}, c});
        edges.push_back({{b, a}, c});
 
        g[a].push_back({b, c});
        g[b].push_back({a, c});
    }
 
    sort(all(edges), [&](const auto & x, const auto & y) {
        return x.y < y.y;
    });
 
    vector<ll> res(1 << n, INF);
    vector<vector<ll>> dp(1 << n, vector<ll>(n, INF));

    for (const auto &[ver, base]: edges) {
        const auto &[start, _] = ver;
        for (int i = 0; i < (1 << n); ++i) {
            fill(all(dp[i]), INF);
        }

        dp[(1 << start)][start] = 0;
        
        for (int mask = 0; mask < (1 << n); ++mask) {
            for (int v = 0; v < n; ++v) {
                if (dp[mask][v] == INF) {
                    continue;
                }
                for (auto &[to, w]: g[v]) {
                    if (w < base) {
                        continue;
                    }
                    if (to == start) {
                        // loop
                        if (__builtin_popcountll(mask) >= 3)
                            res[mask] = min(res[mask], dp[mask][v] + w - base);
                    } else if ((mask & (1 << to)) == 0) {
                        dp[mask | (1 << to)][to] = min(dp[mask | (1 << to)][to], dp[mask][v] + w);
                    }
                }
            }
        }
    }
 
    ll ans = INF;
    DSU dsu(n + 1);

    for (int mask = 0; mask < (1 << n); ++mask) {
        if (res[mask] != INF) {
            dsu.clear();

            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    dsu.join(i, n);
                }
            }
 
            ll loc = res[mask];
            for (auto &[d, w]: edges) {
                int from = d.x;
                int to = d.y;
                if (dsu.get(from) != dsu.get(to)) {
                    dsu.join(from, to);
                    loc += w;
                }
            }
            // cout << bitset<4>(mask).to_string() << " " << res[mask] << " " << loc << endl;
            ans = min(ans, loc);
        }
    }
 
    cout << ans << endl;
}
 
signed main() {
#ifndef LOCAL
    cin.tie(NULL);
    cout.tie(NULL);
    ios_base::sync_with_stdio(false);
#endif
 
 
    cout.precision(16);
    cout << fixed;
 
    int t = 1;
    cin >> t;
    while (t--) {
        solve();
    }
}
