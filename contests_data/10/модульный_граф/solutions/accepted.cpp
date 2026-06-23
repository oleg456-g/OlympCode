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

void solve() {
    int n, m, k;
    cin >> n >> m >> k;
    vi a(n);
    rep(i, n) cin >> a[i];
    vvi g(n);
    rep(_, m) {
        int u, v;
        cin >> u >> v;
        u--;
        v--;
        g[u].push_back(v);
        g[v].push_back(u);
    }
    vvi d(n, vi(n, -1));
    rep(i, n) {
        queue<int> q;
        q.push(i);
        d[i][i] = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (auto &u: g[v]) {
                if (d[i][u] == -1) {
                    d[i][u] = d[i][v] + 1;
                    q.push(u);
                }
            }
        }
    }
    vector<vector<ll>> dp(n, vector<ll>(k + 1, INF));
    dp[0][0] = 0;
    rep1(i, n) {
        int s = d[0][i];
        if (s <= k) dp[i][s] = 0;
    }
    ll ans = INF;
    rep(s, k + 1) {
        vector<bool> was(n);
        while (true) {
            int i = -1;
            rep(j, n) {
                if (dp[j][s] != INF && !was[j] && (i == -1 || dp[i][s] > dp[j][s])) {
                    i = j;
                }
            }
            if (i == -1) break;
            was[i] = true;
            if (d[i][n - 1] + s <= k) ckmin(ans, dp[i][s]);
            rep(j, n) {
                if (j == i) continue;
                int s2 = s + d[i][j] - 1;
                if (s2 <= k) dp[j][s2] = min(dp[j][s2], dp[i][s] + abs(a[i] - a[j]));
            }
        }
    }
    cout << ans << '\n';
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
