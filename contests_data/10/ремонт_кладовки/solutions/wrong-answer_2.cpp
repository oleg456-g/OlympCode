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
    ar<ll, 2> walls;
    rep(i, 2) cin >> walls[i];
    ll h; cin >> h;
    ar<ll, 2> cnt, cost;
    rep(i, 2) cin >> cnt[i] >> cost[i];
    ll w1, h1; cin >> w1 >> h1;
    ll ans = INF;
    rep(door, 2) {
        if (walls[door] < w1 || h < h1) continue;
        rep(mask, 3) {
            bool ok = true;
            ll result = 0;
            rep(wall, 2) {
                ll need = walls[wall] * h * 2;
                if (wall == door) need -= w1 * h1;
                int wallpaper = (mask >> wall) & 1;
                if (need > cnt[wallpaper]) ok = false;
                result += need * cost[wallpaper];
            }
            if (ok) ckmin(ans, result);
        }
    }
    cout << (ans == INF ? -1 : ans) << "\n";
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
