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
    int n, q;
    cin >> n >> q;
    vector<ar<ll, 3>> A(n);
    rep(i, n) cin >> A[i][1];
    rep(i, n) A[i][2] = 1;
    rep(i, n) {
        if (!A[i][2]) A[i][0] = INF;
        else A[i][0] = A[i][1] / A[i][2];
    }
    sort(all(A));
    vector<pair<ll, int>> ways;
    for(int i = 0; i < n; ++i) {
        if (i) {
            A[i][1] += A[i - 1][1];
            A[i][2] += A[i - 1][2];
        }
        ll sumA = A[i][1];
        ll sumB = A[i][2];
        if (sumB == 0) continue;
        if (i + 1 < n && A[i + 1][0] > A[i][0]) {
            ways.emplace_back((A[i][0] + 1) * sumB - sumA, i);
        } else if (i + 1 == n) {
            ways.emplace_back((A[i][0] + 1) * sumB - sumA, i);
            ways.emplace_back(INF, n);
        }
    }
    rep(_, q) {
        ll x; cin >> x;
        int best = lower_bound(all(ways), make_pair(x + 1, -1)) - ways.begin();
        best--;
        ll ans = 0;
        if (best != -1) {
            int i = ways[best].second;
            ll sumA = A[i][1];
            ll sumB = A[i][2];
            // sumB * q - sumA <= x
            // q <= (x + sumA) / sumB
            if (sumB) ans += (x + sumA) / sumB;
            if (i + 1 < n) ans = min(ans, A[i + 1][0]);
        } else {
            ans = A[0][0];
        }
        cout << ans << ' ';
    }
    cout << '\n';
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
