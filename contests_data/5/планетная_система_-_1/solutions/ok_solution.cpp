#include <bits/stdc++.h>
using namespace std;
#define ll long long
const int mod = 1e9 + 7;
vector<int> q;
vector<int> suf;
void build_suf() {
    suf.push_back(1);
    for (int i = (int) q.size() - 1; i >= 1; i--) {
        suf.push_back(suf.back() * q[i]);
    }
    reverse(suf.begin(), suf.end());
}
int getmask(vector<int>& vals) {
    int res = 0;
    for (int i = 0; i < vals.size(); i++) {
        res += vals[i] * suf[i];
    }
    return res;
}
int getbit(int mask, int pos) {
    return (mask / suf[pos]) % q[pos];
}
void clear() {
    vector<int>().swap(q);
    vector<int>().swap(suf);
}
void solve() {
    clear();
    int n, m, k;
    cin >> n >> m >> k;
    q.resize(m);
    int N = 1;
    for (int i = 0; i < m; i++) {
        cin >> q[i];
        N *= q[i];
    }
    build_suf();
    vector<int> masks;
    vector<bool> exists(N), habitable(N);
    for (int i = 0; i < n; i++) {
        vector<int> vals(m);
        for (int j = 0; j < m; j++) {
            cin >> vals[j];
        }
        int mask = getmask(vals);
        masks.push_back(mask);
        exists[mask] = true;
    }
    for (int i = 0; i < k; i++) {
        int ind;
        cin >> ind;
        habitable[masks[ind - 1]] = true;
    }
    vector<vector<int>> dp(N, vector<int>(m + 1));
    vector<vector<int>> prefsum(N, vector<int>(m + 1));
    vector<int> ans(N);
    for (int mask = 0; mask < N; mask++) {
        {
            int bit = getbit(mask, 0);
            if (bit) {
                prefsum[mask][0] = prefsum[mask - suf[0]][0];
            }
        }
        for (int j = 1; j <= m; j++) {
            dp[mask][j] = prefsum[mask][j - 1];
            if (j != m) {
                prefsum[mask][j] = dp[mask][j];
                int bit = getbit(mask, j);
                if (bit) {
                    prefsum[mask][j] = (prefsum[mask][j] + prefsum[mask - suf[j]][j]) % mod;
                }
            }
        }
        if (exists[mask]) {
            ans[mask] = (dp[mask][m] + habitable[mask]) % mod;
            for (int j = 0; j <= m; j++) {
                dp[mask][j] = (dp[mask][j] + ans[mask]) % mod;
                prefsum[mask][j] = (prefsum[mask][j] + ans[mask]) % mod;
            }
        }
    }
    for (auto mask : masks) {
        cout << ans[mask] << '\n';
    }
}
int main() {
    cin.tie(NULL);
    ios_base::sync_with_stdio(false);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}