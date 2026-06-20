#include <bits/stdc++.h>

using namespace std;

const long long inf = 1e18 + 228;

long long ans = inf;

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

void solve(int n, const vector<array<int, 3>> &edges) {
    vector<vector<pair<int, int>>> g(n);
    vector<int> used(n);
    long long sum = 0;
    dsu d(n);
    for (const auto &e : edges) {
        sum += e[2];
        d.merge(e[0], e[1]);
        g[e[0]].emplace_back(e[1], e[2]);
        g[e[1]].emplace_back(e[0], e[2]);
    }
    if (d.sz[d.get(0)] != n) {
        return;
    }
    vector<int> edges_cycle;
    vector<pair<int, int>> parent(n);

    auto dfs = [&](auto dfs, int v) -> bool {
        used[v] = 1;
        for (auto i : g[v]) {
            if (!used[i.first]) {
                parent[i.first] = {v, i.second};
                if (dfs(dfs, i.first)) {
                    return true;
                }
            } else if (parent[v].first != i.first) {
                edges_cycle.emplace_back(i.second);
                int cur = v;
                while (cur != i.first) {
                    edges_cycle.emplace_back(parent[cur].second);
                    cur = parent[cur].first;
                }
                return true;
            }
        }
        return false;
    };
    bool z = dfs(dfs, 0);
    assert(z);
    if (sum - *min_element(edges_cycle.begin(), edges_cycle.end()) == 12) {
        for (auto i : edges) {
            for (int j = 0; j < 3; ++j)
                cout << i[j] << ' ';
            cout << '\n';
        }
    }
    ans = min(ans, sum - *min_element(edges_cycle.begin(), edges_cycle.end()));
}

void gen(int n, int m, int ind, vector<array<int, 3>> &test, const vector<array<int, 3>> &edges) {
    if (test.size() == n) {
        solve(n, test);
        return;
    }
    if (test.size() + m - ind < n) {
        return;
    }

    test.push_back(edges[ind]);
    gen(n, m, ind + 1, test, edges);
    test.pop_back();
    gen(n, m, ind + 1, test, edges);
}

void solve() {
    int n, m;
    cin >> n >> m;
    vector<array<int, 3>> edges(m);
    for (int i = 0; i < m; ++i) {
        cin >> edges[i][0] >> edges[i][1] >> edges[i][2];
        --edges[i][0], --edges[i][1];
    }
    vector<array<int, 3>> tmp;
    gen(n, m, 0, tmp, edges);
    cout << ans << '\n';
}

int main() {
    int t;
    cin >> t;
    while (t--)
        solve();
}