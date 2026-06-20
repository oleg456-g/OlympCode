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
    vector<bool> fl;
    int sz;
 
    void init(int n) {
        r.assign(n + 1, 1);
        p.resize(n + 1);
        iota(all(p), 0);
        fl.assign(n + 1, false);
        sz = n;
    }
 
    int get(int v) {
        if (v == p[v])
            return v;
        return p[v] = get(p[v]);
    }
 
    void merge(int a, int b) {
        a = get(a), b = get(b);
        if (a == b) {
            fl[a] = true;
            return;
        }
        sz--;
        if (r[a] < r[b])
            swap(a, b);
        r[a] += r[b];
        p[b] = a;
        fl[a] = fl[a] | fl[b];
    }
};
 
int ask(int vert) {
    dsu dsu, dsu2;
    dsu.init(n);
    dsu2.init(n);
    for (auto [v, u] : edges) {
        if (v == vert || u == vert)
            continue;
        dsu2.merge(v, u);
    }

    vector<pair<int, int>> a;
    for (auto to : g[vert]) {
        int x = (to + 1) % n, y = (to + n - 1) % n;
        x = dsu2.get(x), y = dsu2.get(y);
        if (x == vert)
            x = y;
        if (y == vert)
            y = x;
        if (x != vert)
            a.eb(x, y);
    }

    int cnt = dsu2.sz;

    for (auto [x, y] : a) {
        x = dsu2.get(x);
        y = dsu2.get(y);
        dsu.merge(x, y);
    }
    vector<bool> used(n);
    for (auto [x, y] : a) {
        x = dsu2.get(x), y = dsu2.get(y);
        if (!used[x]) {
            cnt--;
            used[x] = true;
            if (dsu.get(x) == x && !dsu.fl[x])
                cnt++;
        }
        if (!used[y]) {
            cnt--;
            used[y] = true;
            if (dsu.get(y) == y && !dsu.fl[y])
                cnt++;
        }
    }
    return cnt;
}
 

 
void solve() {
    cin >> n >> m;
    g.assign(n, {});
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
