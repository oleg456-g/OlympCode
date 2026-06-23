#include <bits/stdc++.h>
using namespace std;
 
using ll = long long;
using ld = long double;
 
#define all(a) begin(a), end(a)
#define len(a) int((a).size())
 
int main() {
    cin.tie(nullptr)->sync_with_stdio(false);
 
    ll a, b;
    cin >> a >> b;
    ll ta, tb;
    cin >> ta >> tb;
    string s;
    cin >> s;
 
    int start = 0;
    while (start < len(s) && s[start] == '0') {
        start++;
    }
    s = s.substr(start);
 
    constexpr ll INF = 1e18;
    int n = len(s);
    if (n == 0) {
        cout << "0\n";
        return 0;
    }
    vector<array<ll, 2>> dp(n + 1, {INF, INF});
    dp[0] = {ta, tb};
 
    for (int i = 0; i < n; i++) {
        ll value = 0;
        if (s[i] == '0') {
            dp[i + 1][1] = min(dp[i + 1][1], dp[i][0] + (ta + tb) + b);
            dp[i + 1][1] = min(dp[i + 1][1], dp[i][1] + b);
        }
        for (int j = i; j >= max(0, i - 50); j--) {
            value += ll(s[j] - '0') * (1ll << (i - j));
            if (value >= INF / a) {
                break;
            }
            if (j == 0) {
                dp[i + 1][0] = min(dp[i + 1][0], dp[0][0] + value * a);
            }
            dp[i + 1][0] = min(dp[i + 1][0], dp[j][1] + (i - j + 1) * b + (ta + tb) + value * a);
            dp[i + 1][0] = min(dp[i + 1][0], dp[j][0] + (i - j + 1) * b + 2 * (ta + tb) + value * a);
        }
    }
    cout << min(dp[n][0] + ta, dp[n][1] + tb) << '\n';
}
