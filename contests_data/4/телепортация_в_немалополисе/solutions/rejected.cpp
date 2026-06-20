#pragma GCC optimize("O3,unroll-loops")

#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>

using namespace std;
using namespace __gnu_pbds;

#define int long long
#define sz(arr) (int)((arr).size())

#define cerr if (false) cerr

typedef pair<int, int> ipair;

#define MAXN 300300
#define MAXS 300300

#define P 50497
#define MOD 1'000'000'007

static inline int add(int a, int b) {
    return (a + b >= MOD ? a + b - MOD : a + b);
}

static inline int sub(int a, int b) {
    return (a < b ? a + MOD - b : a - b);
}

static inline int mul(int a, int b) {
    return a * b % MOD;
}

int n, k;
string arr[MAXN];
vector<int> hs[MAXN];
int res[MAXN], po[MAXN];
map<int, vector<int>> D[MAXN];
vector<ipair> gr[MAXN];
bool used[MAXN];

void init() {
    *po = 1;
    for (int i = 0; i + 1 < MAXS; ++i)
        po[i + 1] = mul(po[i], P);
}

// [l..r)
static inline int chsh(int v, int l, int r) {
    return sub(hs[v][r], mul(po[r - l], hs[v][l]));
}

void build() {
    memset(res, -1, sizeof(*res) * n);
    memset(used, 0, sizeof(*used) * n);
    for (int v = 0; v < n; ++v) {
        gr[v].clear();
        D[v].clear();
    }
    for (int v = 0; v < n; ++v) {
        int ss = sz(arr[v]);
        hs[v].assign(ss + 1, 0);
        for (int i = 0; i < ss; ++i)
            hs[v][i + 1] = add(mul(hs[v][i], P), arr[v][i]);
        for (int i = 0; i <= ss - k; ++i) {
            int h = chsh(v, i, ss);
            D[ss - i][h].push_back(v);
        }
    }
    for (int m = k; m < 10; ++m) {
        cerr << m << ": ";
        for (auto [h, lst]: D[m]) {
            cerr << h << "->";
            for (auto u: lst)
                cerr << u << ',';
            cerr << ' ';
        }
        cerr << '\n';
    }
    for (int v = 0; v < n; ++v) {
        for (int m = k; m <= sz(arr[v]); ++m) {
            int h = chsh(v, 0, m);
            auto it = D[m].find(h);
            if (it == D[m].end()) continue;
            for (auto u: it->second) { // TODO: optimize
                if (u == v) continue;
                int hv = sz(arr[v]) == m ? -2 : chsh(v, 0, m + 1);
                int hu = sz(arr[u]) == m ? -1 : chsh(u, sz(arr[u]) - m - 1, sz(arr[u]));
                if (hu != hv) {
                    cerr << "eq: " << m << ' ' << v << ',' << u << '\n';
                    // gr[v].emplace_back(u, m);
                    gr[u].emplace_back(v, m);
                }
            }
        }
    }
}

void solve() {
    priority_queue<ipair> qu;
    qu.emplace(0, 0);
    *res = 0;
    while (!qu.empty()) {
        auto [_, v] = qu.top();
        cerr << "v: " << v << '\n';
        qu.pop();
        if (used[v]) continue;
        used[v] = true;
        for (auto [u, w]: gr[v]) {
            if (res[u] == -1 || res[u] > res[v] + w) {
                res[u] = res[v] + w;
                qu.emplace(-res[u], u);
            }
        }
    }
}

signed main() {
    ios_base::sync_with_stdio(false), cin.tie(NULL);
    int t; cin >> t;
    init();
    while (t--) {
        cin >> n >> k;
        for (int v = 0; v < n; ++v)
            cin >> arr[v];
        build();
        solve();
        for (int v = 1; v < n; ++v)
            cout << res[v] << ' ';
        cout << '\n';
    }
}