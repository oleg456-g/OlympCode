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
    ll a,b, A, B; cin >> a >> b >> A >> B;
    string s; cin >> s;
    reverse(all(s));
    while (!s.empty() && s.back() == '0') s.pop_back();
    reverse(all(s));
    if (s.empty()) {
        cout << "0\n";
        return;
    }
    int n = s.size();
    vector<ll> dp(n + 1, INF);
    dp[0] = 0;
    int j = 0;
    rep(i, n) {
        if (dp[i] == INF) continue;
        j = max(j, i);
        while (j < n && s[j] == '0') j++;
        if (i < j) dp[j] = min(dp[j], 1ll * (j - i) * b + dp[i] + 2 * B);
        ll val = 0;
        for(int last = j; last < n; ++last) {
            val *= 2;
            if (s[last] == '1') val++;
            if (val >= INF / a) break;
            ll cur = dp[i];
            if (i) cur += 1ll * (last + 1 - i) * b + 2 * B;
            cur += 2 * A + a * val;
            dp[last+1] = min(dp[last + 1], cur);
        }
    }
    cout << dp[n] << '\n';
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
