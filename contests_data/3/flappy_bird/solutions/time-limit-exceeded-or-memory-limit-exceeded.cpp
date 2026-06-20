#include <bits/stdc++.h>

using namespace std;

using ll = long long;
using ld = long double;

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()
#define rep(i, n) for (int i = 0; i < (n); i += 1)
#define len(a) ((int)(a).size())

int32_t main() {
    if (1) {
        ios::sync_with_stdio(0);
        cin.tie(0);
        cout.tie(0);
    }
    int t;
    cin >> t;
    rep(itr, t) {
        int n;
        ll h, k, s;
        cin >> n >> h >> k >> s;
        vector<pair<ll, ll>> segs(n);
        for (auto &[l, r] : segs) {
            cin >> l >> r;
        }
        bool ok = false;
        vector<vector<int>> dp(n, vector<int>(h + 1));
        dp[0][s] = 1;
        for (int i = 0; i < n - 1; i += 1) {
            for (int x = 1; x <= h; x += 1) {
                if (dp[i][x]) {
                    if (segs[i + 1].first < x - 1 and x - 1 < segs[i + 1].second) {
                        dp[i + 1][x - 1] = 1;
                    }
                    if (segs[i + 1].first < x + k and x + k < segs[i + 1].second) {
                        dp[i + 1][x + k] = 1;
                    }
                }
            }
        }
        for (int x = 1; x <= h; x += 1) {
            if (dp[n - 1][x]) {
                ok = true;
            }
        }
        if (ok) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
