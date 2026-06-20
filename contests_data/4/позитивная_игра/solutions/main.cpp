#include <bits/stdc++.h>

using namespace std;

struct dsu {
    int n;
    vector<int> p;
    vector<int> sz;

    dsu(int n) : n(n) {
        p.resize(n);
        sz.resize(n, 1);
        iota(p.begin(), p.end(), 0);
    }

    int get(int v) {
        if (v == p[v]) {
            return v;
        }
        return p[v] = get(p[v]);
    }

    void merge(int a, int b) {
        a = get(a), b = get(b);
        if (sz[b] < sz[a]) {
            swap(a, b);
        }
        if (a != b) {
            p[a] = b;
            sz[b] += sz[a];
        }
    }
};

const long long INF = 1e18 + 228;

void solve() {
    int n, m;
    cin >> n >> m;
    vector<array<int, 3>> edges;
    for (int i = 0; i < m; ++i) {
        int a, b, c;
        cin >> a >> b >> c;
        --a, --b;
        edges.push_back({a, b, c});
    }
    sort(edges.begin(), edges.end(), [](const array<int, 3>& a, const array<int, 3>& b) {
        return a[2] < b[2];
    });

    long long ans = INF;
    for (int i = 0; i < edges.size(); ++i) {
        int a = edges[i][0], b = edges[i][1], c = edges[i][2];
        for (int mask = 0; mask < (1 << n); ++mask) {
            int cnt = 0;
            for (int j = 0; j < n; ++j) {
                if (mask & (1 << j)) {
                    ++cnt;
                }
            }
            if (cnt < 3) {
                continue;
            }
            if (!(mask & (1 << a)) || !(mask & (1 << b))) {
                continue;
            }
            dsu d(n);
            long long cur = 0;
            for (const auto &z : edges) {
                bool cum = false;
                if ((mask & (1 << z[0])) && (mask & (1 << z[1]))) {
                    cum = true;
                }
                if (cum) {
                    if (z[2] >= c && edges[i] != z && d.get(z[0]) != d.get(z[1])) {
                        cur += z[2];
                        d.merge(z[0], z[1]);
                    }
                }
            }
            if (d.sz[d.get(a)] != cnt) {
                continue;
            }
            for (const auto &z : edges) {
                if (d.get(z[0]) != d.get(z[1])) {
                    cur += z[2];
                    d.merge(z[0], z[1]);
                }
            }
            if (d.sz[d.get(a)] == n) {
                ans = min(ans, cur);
            }
        }
    }
    cout << ans << '\n';
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t--)
        solve();
}