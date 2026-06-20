#include <bits/stdc++.h>

using namespace std;

#define ll long long
#define flt double
#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()

const ll inf = 1e7;

struct SGT {
    int n;
    vector<ll> mx, mxc;
    vector<ll> d;

    void apply(int v, int x) {
        mx[v] += x;
        d[v] += x;
    }

    void push(int v) {
        apply(v + v, d[v]);
        apply(v + v + 1, d[v]);
        d[v] = 0;
    }

    void upd(int v) {
        mx[v] = max(mx[v + v], mx[v + v + 1]);
        mxc[v] = 0;
        if (mx[v + v] == mx[v]) {
            mxc[v] += mxc[v + v];
        }
        if (mx[v + v + 1] == mx[v]) {
            mxc[v] += mxc[v + v + 1];
        }
    }

    void build(int v, int tl, int tr) {
        if (tl == tr) {
            d[v] = 0;
            mx[v] = 0;
            mxc[v] = 1;
            return;
        }
        int tm = (tl + tr) / 2;
        build(v + v, tl, tm);
        build(v + v + 1, tm + 1, tr);
        upd(v);
    }

    void build(int _n) {
        n = _n;
        int sz = 1;
        while(sz < n){
            sz *= 2;
        }
        mx.assign(sz * 2, 0);
        mxc.assign(sz * 2, 0);
        d.assign(sz * 2, 0);
        build(1, 0, n - 1);
    }

    void add(int v, int tl, int tr, int l, int r, ll x) {
        if (tr < l || r < tl) {
            return;
        }
        if (l <= tl && tr <= r) {
            apply(v, x);
            return;
        }
        int tm = (tl + tr) / 2;
        push(v);
        add(v + v, tl, tm, l, r, x);
        add(v + v + 1, tm + 1, tr, l, r, x);
        upd(v);
    }

    void add(int l, int r, ll x) {
        add(1, 0, n - 1, l, r, x);
    }

    ll get() {
        return mxc[1];
    }
};

const int mxn = 1e6;
const int lgn = 20;
int q;
vector<pair<int, int>> asks;
int n;
vector<vector<int>> t;
int bup[mxn][lgn];
vector<pair<int, int>> rs;
vector<int> h;
int tc;
vector<int> tin, tout;
SGT sgt;

void build_dfs(int v, int pr, int mh) {
    tin[v] = tc++;
    h[v] = mh;
    bup[v][0] = pr;
    for (int i = 1; i < lgn; i += 1) {
        bup[v][i] = bup[bup[v][i - 1]][i - 1];
    }
    for (auto to : t[v]) {
        build_dfs(to, v, mh + 1);
    }
    tout[v] = tc - 1;
}

int get_up(int v, int x) {
    assert(x >= 0);
    for (int i = 0; i < lgn; i += 1) {
        if ((x >> i) & 1) {
            v = bup[v][i];
        }
    }
    return v;
}

int lca(int u, int v) {
    if (h[u] < h[v]) {
        swap(u, v);
    }
    u = get_up(u, h[u] - h[v]);
    if (u == v) {
        return u;
    }
    for (int i = lgn - 1; i >= 0; i -= 1) {
        if (bup[u][i] != bup[v][i]) {
            u = bup[u][i];
            v = bup[v][i];
        }
    }
    return bup[u][0];
}

void build() {
    tc = 0;
    tin.resize(n);
    tout.resize(n);
    h.resize(n);
    build_dfs(0, 0, 0);
}

ll dist(int u, int v) {
    int lc = lca(u, v);
    return h[u] + h[v] - 2 * h[lc];
}

void add_on_subtree(int u, int v, int x) {
    if (bup[v][0] == u) {
        sgt.add(tin[v], tout[v], x);
    } else {
        sgt.add(0, n - 1, x);
        sgt.add(tin[u], tout[u], -x);
    }
}

void go_by_edge(int u, int v) {
    add_on_subtree(u, v, -1);
    add_on_subtree(v, u, 1);
}

int get_last_on_way(int x, int y) {
    int lc = lca(x, y);
    if (lc != y) {
        return bup[y][0];
    }
    return get_up(x, h[x] - h[y] - 1);
}

void solve() {
    build();
    rs.resize(q);
    int du = 0;
    int dv = 0;
    int d = 0;
    int cntr = 0;
    sgt.build(n);
    for (int i = 0; i < n; i += 1) {
        ll x = h[i];
        if (i != 0) {
            x -= inf;
        }
        sgt.add(tin[i], tin[i], x);
    }
    for (int i = 0; i < q; i += 1) {
        if (asks[i].first == 0) {
            int v = asks[i].second;
            bool chng = false;
            sgt.add(tin[v], tin[v], inf);
            if (dist(v, du) > d) {
                dv = v;
                chng = true;
            } else if (dist(v, dv) > d) {
                du = v;
                chng = true;
            }
            d = dist(du, dv);
            if (chng) {
                int dst_du = dist(du, cntr);
                int dst_dv = dist(dv, cntr);
                if (dst_du > dst_dv) {
                    swap(du, dv);
                    swap(dst_du, dst_dv);
                }
                if (dst_du + 1 < dst_dv) {
                    int nc = get_last_on_way(dv, cntr);
                    go_by_edge(cntr, nc);
                    cntr = nc;
                }
            }
        } else {
            int v = asks[i].second;
            int ans = -1;
            if (v != cntr) {
                int u = get_last_on_way(v, cntr);
                add_on_subtree(cntr, u, -inf);
                ans = sgt.get();
                add_on_subtree(cntr, u, inf);
            } else {
                ans = sgt.get();
            }
            rs[i] = { max(dist(du, v), dist(dv, v)), ans };
        }
    }
}

int32_t main() {
    if (1) {
        ios::sync_with_stdio(0);
        cin.tie(0);
        cout.tie(0);
    }
    n = 1;
    t.assign(n, vector<int>());
    int m;
    cin >> q >> m;
    asks.resize(q);
    for (int i = 0; i < q; i += 1) {
        char tp;
        cin >> tp;
        int v;
        cin >> v;
        v = (v - 1) % n + 1;
        v -= 1;
        if (tp == '+') {
            t[v].push_back(n);
            t.push_back({});
            n += 1;
            asks[i] = make_pair(0, n - 1);
        } else {
            asks[i] = make_pair(1, v);
        }
    }
    solve();
    for (int i = 0; i < q; i += 1) {
        if (asks[i].first == 1) {
            cout << rs[i].first << ' ' << rs[i].second << "\n";
        }
    }
    return 0;
}
