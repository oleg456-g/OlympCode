#include <bits/stdc++.h>
using namespace std;
#define eb emplace_back
#define int long long
#define all(x) x.begin(), x.end()
#define fi first
#define se second
#define lwb lower_bound
#define ll long long
#define upb upper_bound
#define lwb lower_bound
#define ld long double
#define pb push_back
#define pii pair<int, int>
#define popcount __builtin_popcount
//#define xxint unsigned int
//mt19937 rnd(chrono::high_resolution_clock::now().time_since_epoch().count());
const int INF = 1e18 + 1000, N = 2e6 + 100, K = 23, P = 998244353, C = 1000;
const ld EPS = 1e-8, pi = 3.1415926535;
 
//ifstream fin("C:/Users/yunet/CLionProjects/chupa3/cmake-build-debug/input.txt");
 
vector<vector<int>> g;
vector<pair<int, int>> edges;
int n, m;
 
struct dsu {
    vector<int> r, p;
    int sz;

    void init(int n) {
        r.assign(n + 1, 1);
        p.resize(n + 1);
        iota(all(p), 0);
        sz = n;
    }
 
    int get(int v) {
        if (v == p[v])
            return v;
        return p[v] = get(p[v]);
    }
 
    void merge(int a, int b) {
        a = get(a), b = get(b);
        if (a == b)
            return;
        sz--;
        if (r[a] < r[b])
            swap(a, b);
        r[a] += r[b];
        p[b] = a;
    }
};

int f(vector<pair<int, int>> a, int v) {
    int ans = INF;
    int k = (int) a.size();
    for (int mask = 0; mask < (1 << k); mask++) {
        dsu dsu;
        dsu.init(n);
        for (auto [x, y] : edges) {
            if (x == v || y == v)
                continue;
            dsu.merge(x, y);
        }
        for (int i = 0; i < k; i++) {
            if ((1 << i) & mask)
                dsu.merge(v, a[i].fi);
            else
                dsu.merge(v, a[i].se);
        }
        ans = min(ans, dsu.sz);
    }
    return ans;
}
 
int ask(int v) {
    vector<pair<int, int>> a;
    for (auto to : g[v]) {
        int x = (to + 1) % n, y = (to + n - 1) % n;
        a.eb(x, y);
    }
    return f(a, v);
}
 
void solve() {
    cin >> n >> m;
    g.assign(n + 1, {});
    edges.assign(m, {});
    for (int i = 0; i < m; i++) {
        int v, u;
        cin >> v >> u;
        g[v].eb(u);
        g[u].eb(v);
        edges[i] = {v, u};
    }
    for (int i = 0; i < n; i++) {
        cout << ask(i) << " ";
    }
    cout << "\n";
}
 
signed main() {
    //cout << fixed << setprecision(5);
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    //cin >> T;
    //cin >> G;
    while (T--)
        solve();
    return 0;
}
