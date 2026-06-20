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
mt19937 rnd(chrono::high_resolution_clock::now().time_since_epoch().count());
 
//ifstream fin("C:/Users/yunet/CLionProjects/chupa3/cmake-build-debug/input.txt");
 
struct dsu {
    vector<int> r, p;
 
    void init(int n) {
        r.assign(n, 1);
        p.assign(n, 0);
        iota(all(p), 0);
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
        r[a] += r[b];
        p[b] = a;
    }
};
 
vector<vector<int>> g;
vector<int> h, deep, ans, good;
int timer = 1;
vector<bool> used;
int n, m;
dsu dsu;
 
vector<vector<int>> req_g;
vector<bool> req_used;
 
bool find_cyc(int v, int p) {
    req_used[v] = true;
    bool fl = false;
    for (auto to : req_g[v]) {
        if (to == p) {
            if (fl)
                return true;
            fl = true;
        }
        else if (req_used[to])   
            return true;
        else if (find_cyc(to, v))
            return true;
    }
    return false;
}
 
int f(vector<pair<int, int>> &a, int cnt) {
    int res = 0;
    for (auto &[v, u] : a) {
        req_g[v].eb(u);
        req_g[u].eb(v);
    }
    for (auto &[v, u] : a) {
        if (!req_used[v] && !find_cyc(v, -1))
            res++;
        if (!req_used[u] && !find_cyc(u, -1))
            res++;
    }
 
    for (auto [u, v] : a) {
        if (req_used[u]) {
            cnt--;
            req_used[u] = false;
        }
        if (req_used[v]) {
            cnt--;
            req_used[v] = false;
        }
        req_g[v].clear(), req_g[u].clear();
    }
    return cnt + res;
}
 
void dfs(int v, int p) {
    used[v] = true;
    good[v] = timer;
    h[v] = deep[v];
    for (auto to : g[v]) {
        if (!used[to]) {
            deep[to] = deep[v] + 1;
            dfs(to, v);
            h[v] = min(h[v], h[to]);
        }
        else if (to != p) {
            h[v] = min(h[v], deep[to]);
        }
    }
 
    timer++;
 
    int cnt = 0;
    if (p != -1)
        cnt++;
 
    for (auto to : g[v]) {
        int x = dsu.get(to);
        if (x == v)
            continue;
        if (h[x] < deep[v])
            x = p;
        else if (good[x] != timer) {
            good[x] = timer;
            cnt++;
        }
    }
 
    good[v] = timer;
 
    vector<pair<int, int>> a;
 
    for (auto to : g[v]) {
        int x = (to + 1) % n,
        y = (to - 1 + n) % n;
        x = dsu.get(x), y = dsu.get(y);
        if (good[x] != timer)
            x = p;
        if (good[y] != timer)
            y = p;
        if (x == v)
            x = y;
        if (y == v)
            y = x;
        if (x != v)
            a.eb(x, y);
    }
 
    ans[v] = f(a, cnt) + 1;
 
 
    timer++;
    for (auto to : g[v]) {
        if (deep[to] > deep[v])
            dsu.merge(v, to);
    }
}
 
void clear() {
    g.assign(n, {});
    req_g.assign(n, {});
    ans.assign(n, 0);
    h.assign(n, INF);
    deep.assign(n, 0);
    used.assign(n, false);
    req_used.assign(n, false);
    good.assign(n, 0);
    dsu.init(n);
}
 
void solve() {
    cin >> n >> m;
    clear();
    for (int i = 0; i < m; i++) {
        int v, u;
        cin >> v >> u;
        g[v].eb(u);
        g[u].eb(v);
    }
    dfs(0, -1);
    for (int i = 0; i < n; i++) {
        cout << ans[i] << " ";
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
