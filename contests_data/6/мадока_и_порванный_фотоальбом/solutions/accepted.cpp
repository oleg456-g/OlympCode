#include<bits/stdc++.h>
using namespace std;
#define pb  push_back
#define ll  long long
#define vi  vector<ll >
#define vvi vector<vi >
#define all(x) x.begin(), x.end()

ll n, m;
vector<string> a;
int ok[6][32][32];
int bi[6][32];

ll solve() {
    cin >> n >> m;
    a.resize(n);
    for (int i = 0; i < n; ++i)
        cin >> a[i];
    if (n == 1) {
        ll res = 0;
        for (int j = 0; j < m; ++j)
            res += (a[0][j] == '0');
        return res;
    }
    vi  p(m, 0);
    for (ll j = 0; j < m; ++j)
        for (ll i = 0; i < n; ++i)
            p[j] ^= ((a[i][j] - '0') << i);
    vvi dp(m, vi(1 << n, 0));
    for (ll ma = 0; ma < (1 << n); ++ma)
        dp[0][ma] = ((p[0] & ma) == 0 ? bi[n][ma] : -1);
    for (ll j = 1; j < m; ++j) {
        for (ll mf = 0; mf < (1 << n); ++mf) {
            if (dp[j - 1][mf] == -1) continue;
            for (ll ms = 0; ms < (1 << n); ++ms) {
                if ((ms & p[j]) != 0) continue;
                if (ok[n][mf][ms])
                    dp[j][ms] = max(dp[j][ms], dp[j-1][mf] + bi[n][ms]);
            }
        }
    }
    ll res = 0;
    for (ll ma = 0; ma < (1 << n); ++ma)
        res = max(res, dp[m - 1][ma]);
    return res;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(0);
    for (ll n = 2; n <= 5; ++n) {
        for (ll mf = 0; mf < (1 << n); ++mf) {
            int bii = 0;
            for (ll e = 0; e < n; ++e)
                bii += (mf >> e) % 2;
            bi[n][mf] = bii;
            for (ll ms = 0; ms < (1 << n); ++ms) {
                int res = 1;
                for (ll e = 1; e < n; ++e) {
                    ll num = (mf >> e) % 2 + (mf >> (e - 1)) % 2
                           + (ms >> e) % 2 + (ms >> (e - 1)) % 2;
                    if (num == 3)
                        res = 0;
                }
                ok[n][mf][ms] = res;
            }
        }
    }
    int t; cin >> t;
    while (t--)
        cout << solve() << "\n";

    return 0;
}
