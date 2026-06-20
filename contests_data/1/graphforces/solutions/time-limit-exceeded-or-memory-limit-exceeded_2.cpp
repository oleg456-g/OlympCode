#include <bits/stdc++.h>

using namespace std;

// #define int long long
#define all(x) begin(x),end(x)
#define rall(x) rbegin(x),rend(x)
#define pb push_back
#define ar(x) array<int,x>
typedef long long ll;

const int N = 1e6 + 77;
vector<int> g[N];
char used[N];
vector<int> lol;

int a2solver(vector<ar(2) > &a) {
    // 1 from each pair
    // max #diff
    // tree->sz-1, non-tree->sz
    lol = {};
    for (auto [x, y]: a)lol.pb(x), lol.pb(y);
    sort(all(lol));
    auto code = [&](int x) {
        return lower_bound(all(lol), x) - lol.begin();
    };
    int mx = 0;
    for (auto &q: a) {
        q[0] = code(q[0]);
        q[1] = code(q[1]);
        mx = max({mx, q[0], q[1]});
    }
    fill(g, g + mx + 1, (vector<int>) {});
    fill(used, used + mx + 1, 0);
    for (auto [x, y]: a) {
        g[x].pb(y);
        g[y].pb(x);
    }
    int ans = 0;
    for (int x = 0; x <= mx; x++) {
        if (used[x])continue;
        int sz = 1, edges = 0;
        used[x] = 1;
        vector<int> s = {x};
        while (!s.empty()) {
            int v = s.back();
            s.pop_back();
            for (auto u: g[v]) {
                if (used[u]) {
                    edges++;
                    continue;
                }
                edges++;
                used[u] = 1;
                s.pb(u);
                sz++;
            }
        }
        edges /= 2;
//        assert(edges >= sz - 1);
        if (edges == sz - 1) {
            ans += sz - 1;
        } else {
            ans += sz;
        }
    }
    return ans;
}

#define ADD 0
#define QUERY 1

struct DSU {
    int n;
    vector<int> par, sz, mn;

    DSU() = default;

    vector<ar(8) > history;

    DSU(int n) : n(n) {
        par.resize(n);
        iota(all(par), 0);
        sz.assign(n, 1);
        mn.resize(n);
        iota(all(mn), 0);
    }

    int get(int v) {
        if (v == par[v]) return v;
        return get(par[v]);
    }

    void uni(int u, int v) {
        u = get(u), v = get(v);
        history.pb({v, u, sz[v], sz[u], par[v], par[u], mn[v], mn[u]});
        if (u == v) {
            return;
        }
        if (sz[u] > sz[v]) {
            swap(u, v);
        }
        sz[v] += sz[u];
        par[u] = v;
        mn[v] = min(mn[v], mn[u]);
    }

    void rollback() {
        auto [v, u, sv, su, pv, pu, mv, mu] = history.back();
        history.pop_back();
        sz[v] = sv;
        sz[u] = su;
        par[v] = pv;
        par[u] = pu;
        mn[v] = mv;
        mn[u] = mu;
    }

};

struct SegTree {
    int n;

    SegTree() = default;

    vector<vector<pair<char, ar(2)>>> tree;

    SegTree(int n) : n(n) {
        tree.resize(4 * n);
    }

    void addedge(int v, int l, int r, int lq, int rq, ar(2) edge) {
        if (l >= rq || lq >= r) return;
        if (lq <= l && r <= rq) {
            tree[v].pb({ADD, edge});
            return;
        }
        int mid = (l + r) / 2;
        addedge(2 * v + 1, l, mid, lq, rq, edge);
        addedge(2 * v + 2, mid, r, lq, rq, edge);
    }

    void addedge(int l, int r, ar(2) edge) {
        addedge(0, 0, n, l, r, edge);
    }

    void addquery(int v, int l, int r, int i, int qid, int x) {
        if (l + 1 == r) {
            tree[v].pb({QUERY, (ar(2)) {qid, x}});
            return;
        }
        int mid = (l + r) / 2;
        if (i < mid) {
            addquery(2 * v + 1, l, mid, i, qid, x);
        } else {
            addquery(2 * v + 2, mid, r, i, qid, x);
        }
    }

    void addquery(int i, int qid, int x) {
        addquery(0, 0, n, i, qid, x);
    }


};


struct UltraDCP {
    int n;
    map<ar(2), vector<int>> timeline;
    int timer = 0;
    SegTree tree;

    UltraDCP(int n) : n(n) {
    }

    void toggle(int v, int u) {
        if (v > u) swap(v, u);
        timeline[{v, u}].pb(timer);
        timer++;
    }

    vector<ar(2) > qs;

    void query_min(int v) {
        // min in same comp as v
        qs.pb({v, timer});
        timer++;
    }

    void commit() {
        tree = SegTree(timer + 1);
        for (auto [edge, ts]: timeline) {
            ts.pb(timer);
            for (int i = 0; i + 1 < ts.size(); i += 2) {
                int l = ts[i], r = ts[i + 1];
                tree.addedge(l, r, edge);
            }
        }
        for (int i = 0; i < qs.size(); i++) {
            auto [v, tm] = qs[i];
            tree.addquery(tm, i, v);
        }
    }

    vector<int> ans;
    DSU dsu;

    void dfs(int v, int l, int r) {
        for (auto [c, edge]: tree.tree[v]) {
            if (c == ADD) {
                dsu.uni(edge[0], edge[1]);
            }
        }
        if (l + 1 == r) {
            for (auto [c, edge]: tree.tree[v]) {
                if (c == QUERY) {
                    int i = edge[0];
                    int x = edge[1];
                    ans[i] = dsu.mn[dsu.get(x)];
                }
            }
            for (auto [c, edge]: tree.tree[v]) {
                if (c == ADD) {
                    dsu.rollback();
                }
            }
            return;
        }
        int mid = (l + r) / 2;
        dfs(2 * v + 1, l, mid);
        dfs(2 * v + 2, mid, r);
        for (auto [c, edge]: tree.tree[v]) {
            if (c == ADD) {
                dsu.rollback();
            }
        }
    }

    vector<int> solve() {
        ans.resize(qs.size());
        dsu = DSU(n);
        dfs(0, 0, tree.n);
        return ans;
    }
};


void solve() {
    int n, m;
    cin >> n >> m;
    UltraDCP udine(n);
    vector<vector<int>> gg(n);
    vector<ar(2) > edges;
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        gg[a].pb(b);
        gg[b].pb(a);
        edges.pb({a, b});
    }
    for (auto [v, u]: edges) {
        udine.toggle(v, u);
    }
    for (int v = 0; v < n; v++) {
        for (auto u: gg[v]) {
            udine.toggle(v, u);
        }
        for (auto u: gg[v]) {
            udine.query_min(u);
            udine.query_min((u + n - 1) % n);
            udine.query_min((u + 1) % n);
        }
        for (auto u: gg[v]) {
            udine.toggle(v, u);
        }
    }
    udine.commit();
    auto answers = udine.solve();
    int tick = 0;
    vector<int> diff;
    vector<ar(2) > q;
    for (int v = 0; v < n; v++) {
        q = {}, diff = {};
        for (auto u: gg[v]) {
            int cu = answers[tick++];
            int a = answers[tick++];
            int b = answers[tick++];
            diff.pb(cu);
            if (a != v || b != v) {
                if (a == v)a = b;
                if (b == v)b = a;
                q.pb({a, b});
            }
        }
        int difs = 0;
        difs += (!diff.empty());
        sort(all(diff));
        for (int i = 1; i < diff.size(); i++)difs += (diff[i - 1] != diff[i]);
        int comps = difs + 1 - a2solver(q);
        cout << comps << '\n';
    }
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
}