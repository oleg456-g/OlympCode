#include <bits/stdc++.h>

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()

using namespace std;

void solve() {
    int n, m;
    cin >> n >> m;
    vector<vector<int>> ban(n, vector<int> (m));
    for (int i = 0; i < n; ++i) {
        string s;
        cin >> s;
        for (int j = 0; j < m; ++j) {
            ban[i][j] = s[j] - '0';
        }
    }
    int inf = n * m + 228;
    vector<vector<int>> dp(m + 1, vector<int> ((1 << n), -inf));
    dp[0][0] = 0;
    for (int i = 0; i < m; ++i) {
        for (int mask = 0; mask < (1 << n); ++mask) {
            int sum = 0;
            bool used_ban = false;
            for (int j = 0; j < n; ++j) {
                if (mask & (1 << j))
                    ++sum;
                if (ban[j][i] && (mask & (1 << j))) {
                    used_ban = true;
                    break;
                }
            }
            if (used_ban) {
                continue;
            }
            for (int last_mask = 0; last_mask < (1 << n); ++last_mask) {
                bool valid = true;
                bool used1 = false; // 10
                bool used2 = false; // 11
                for (int j = 0; j < n; ++j) {
                    bool first_one = (1 << j) & mask;
                    bool second_one = (1 << j) & last_mask;
                    if (first_one && second_one) {
                        if (used1) {
                            valid = false;
                        }
                        used2 = true;
                    } else if (!first_one && !second_one) {
                        used1 = false;
                        used2 = false;
                    } else {
                        if (used2) {
                            valid = false;
                        }
                        used1 = true;
                    }
                }
                if (valid) {
                    dp[i + 1][mask] = max(dp[i + 1][mask], dp[i][last_mask] + sum);
                }
            }
        }
    }
    cout << *max_element(dp.back().begin(), dp.back().end()) << '\n';
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t--)
        solve();    
}
