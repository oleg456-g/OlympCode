#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template <typename T> bool chmin(T& a, T b) { if (a <= b) return false; a = b; return true; }
template <typename T> bool chmax(T& a, T b) { if (a >= b) return false; a = b; return true; }

const ll inf = 1e18;


int main() {
    ll a, b, A, B;
    string s;
    cin >> a >> b >> A >> B >> s;
    int n = s.size();
    
    if (n == 1 && s[0] == '0') {
        cout << 0 << endl;
        return 0;
    }
    
    vector dp(n+1, array<ll, 2>{{inf, inf}}); // pref, pos
    dp[0] = {A, inf};

    for (int pref = 0; pref < n; ++pref) {
        ll s10 = 0;
        for (int i = pref; i >= 0 && (1ll << (pref-i)) <= inf; --i) {
            s10 += (1ll << (pref-i)) * (s[i] - '0');
            if (i == 0) {
                chmin(dp[pref + 1][0], A + a * s10);
            } else {
                chmin(dp[pref + 1][0], dp[i][1] + (pref-i+1) * b + A + B + s10 * a);
            }
        }
        dp[pref + 1][1] = dp[pref + 1][0] + A + B;
        if (s[pref] == '0') {
            chmin(dp[pref + 1][1], dp[pref][1] + b);
        }
    }
    cout << min(dp[n][0] + A, dp[n][1] + B) << endl;
}
