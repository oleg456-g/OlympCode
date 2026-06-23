#include "bits/stdc++.h"

#define rep(i, n) for (int i = 0; i < (n); ++i)
#define rep1(i, n) for (int i = 1; i < (n); ++i)
#define rep1n(i, n) for (int i = 1; i <= (n); ++i)
#define repr(i, n) for (int i = (n) - 1; i >= 0; --i)
#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()
#define rall(a) (a).rbegin(), (a).rend()
#define each(x, a) for (auto &x : a)
#define ar array
#define vec vector
#define range(i, n) rep(i, n)

using namespace std;

using ll = long long;
using ull = unsigned long long;
using ld = double;
using str = string;
using pi = pair<int, int>;
using pl = pair<ll, ll>;

using vi = vector<int>;
using vl = vector<ll>;
using vpl = vector<pl>;
using vpi = vector<pair<int, int>>;
using vvi = vector<vi>;

int Bit(int mask, int b) { return (mask >> b) & 1; }

template<class T>
bool ckmin(T &a, const T &b) {
    if (b < a) {
        a = b;
        return true;
    }
    return false;
}

template<class T>
bool ckmax(T &a, const T &b) {
    if (b > a) {
        a = b;
        return true;
    }
    return false;
}

const int INFi = 2e9;
const ll INF = 2e18;

const int N = 2e5 + 5;
const int LG = 20;
vector<int> g[N];
int up[N][LG];
int h[N];
int tin[N], tout[N];
int T = 0;

void dfs(int v, int p) {
    up[v][0] = p;
    for (int i = 1; i < LG; ++i) up[v][i] = up[up[v][i - 1]][i - 1];
    tin[v] = T++;
    for (auto &u: g[v]) {
        if (u == p) continue;
        h[u] = h[v] + 1;
        dfs(u, v);
    }
    tout[v] = T;
}

int lca(int v, int u) {
    if (h[v] < h[u]) swap(u, v);
    for (int i = LG - 1; i >= 0; --i) {
        if (h[up[v][i]] >= h[u]) v = up[v][i];
    }
    if (u == v) return v;
    for (int i = LG - 1; i >= 0; --i) {
        if (up[v][i] != up[u][i]) v = up[v][i], u = up[u][i];
    }
    return up[v][0];
}

int dist(int a, int b) {
    return h[a] + h[b] - 2 * h[lca(a, b)];
}

bool is_parent(int p, int v) {
    return tin[p] <= tin[v] && tout[v] <= tout[p];
}

bool alive[N];

namespace SegTree {
    priority_queue<pair<int, int>> t[2 * N];

    int Get(int i) {
        int ans = 0;
        for (i += N; i >= 1; i >>= 1)
            while (!t[i].empty()) {
                auto [c, v] = t[i].top();
                if (!alive[v]) {
                    t[i].pop();
                } else {
                    ckmax(ans, c);
                    break;
                }
            }
        return ans;
    }

    void Add(int l, int r, pair<int, int> x) {
        for (l += N, r += N; l < r; l >>= 1, r >>= 1) {
            if (l & 1) t[l++].push(x);
            if (r & 1) t[--r].push(x);
        }
    }
}


enum Type {
    ADD,
    REMOVE,
    QUERY
};

struct Event {
    Type type;
    int vid;

    Event(Type type_ = QUERY, int vid_ = -1) : type(type_), vid(vid_) {}
};

void solve() {
    int n; cin >> n;
    int q; cin >> q;
    for(int i = 2; i <= n; ++i) {
        int p; cin >> p;
        g[p].push_back(i);
    }
    dfs(1, 1);
    vi t(n + 1);
    vector<vector<Event>> events(n + 1);
    for(int i = 1; i <= n; ++i) {
        cin >> t[i];
        events[t[i]].emplace_back(ADD, i);
    }
    vi c(n + 1);
    for(int i = 1; i <= n; ++i) cin >> c[i];
    vector<vector<int>> qs;
    vector<ll> answ;
    rep(_, q) {
        int tp; cin >> tp;
        if (tp == 1) {
            int v; cin >> v;
            events[t[v]].emplace_back(REMOVE, v);
            cin >> t[v];
            events[t[v]].emplace_back(ADD, v);
        } else if (tp == 2) {
            int to, k; cin >> to >> k;
            events[to].emplace_back(QUERY, qs.size());
            qs.emplace_back(vi(k));
            rep(j, k) cin >> qs.back()[j];
            answ.emplace_back(0);
        }
    }
    for(int i = 1; i <= n; ++i) {
        events[t[i]].emplace_back(REMOVE, i);
    }
    for(int to = 1; to <= n; ++to) {
        for(auto &event : events[to]) {
            if (event.type == ADD) {
                alive[event.vid] = true;
                SegTree::Add(tin[event.vid], tout[event.vid], {c[event.vid], event.vid});
                continue;
            } else if (event.type == REMOVE) {
                alive[event.vid] = false;
                continue;
            }
            // query
            assert(event.type == QUERY);
            int i = event.vid;
            ll ans = 0;
            vector<pair<int, int>> vrs;
            for(auto &v : qs[i]) vrs.emplace_back(v, 1);
            sort(all(vrs), [&] (const pi &x, const pi &y) { return tin[x.first] < tin[y.first]; });
            for(int j = (int)vrs.size() - 1; j >= 1; --j) vrs.emplace_back(lca(vrs[j].first, vrs[j - 1].first), 0);
            sort(all(vrs), [&] (const pi &x, const pi &y) { return tin[x.first] < tin[y.first]; });
            vector<pair<int, int>> stk;
            auto Pop = [&] () {
                assert(!stk.empty());
                auto [v, cnt] = stk.back();
                stk.pop_back();
                ckmax(ans, 1ll * cnt * SegTree::Get(tin[v]));
                if (!stk.empty()) stk.back().second += cnt;
            };
            for(auto &[v, cnt] : vrs) {
                while (!stk.empty() && !is_parent(stk.back().first, v)) Pop();
                stk.emplace_back(v, cnt);
            }
            while (!stk.empty()) Pop();
            answ[i] = ans;
        }
    }
    for(auto &x : answ) cout << x << '\n';
}

signed main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    cout << setprecision(15) << fixed;
    int t = 1;
//    cin >> t;
    rep(_, t) {
        solve();
    }
    return 0;
}
