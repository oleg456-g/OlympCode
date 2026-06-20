#include <bits/stdc++.h>

using namespace std;
#define int long long

#ifdef LOCAL
#include "algo/debug.h"
#else
#define debug(...) 42
#endif

const int MAXH = 102;

void solve() {
  int n, h, k, s;
  cin >> n >> h >> k >> s;
  vector<array<int, 2>> a(n);
  for (auto& [l, r] : a) {
    cin >> l >> r;
  }

  int cur = 0;
  vector<bitset<MAXH>> dp(2);
  dp[cur][s] = 1;

  for (int i = 1; i < n; i++) {
    cur ^= 1;
    dp[cur].reset();
    for (int j = a[i][0] + 1; j < a[i][1]; j++) {
      if (j + 1 < MAXH && dp[cur ^ 1][j + 1]) {
        dp[cur][j] = 1;
      }
      if (j - k > 0 && dp[cur ^ 1][j - k]) {
        dp[cur][j] = 1;
      }
    }
  }

  for (int i = a[n - 1][0] + 1; i < a[n - 1][1]; i++) {
    if (dp[cur][i]) {
      cout << "Yes\n";
      return;
    }
  }

  cout << "No\n";
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  int tc;
  cin >> tc;
  while (tc --> 0) solve();
  return 0;
}
