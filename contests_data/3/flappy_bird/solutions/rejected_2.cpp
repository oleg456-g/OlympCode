#include <bits/stdc++.h>

using namespace std;
#define int long long

#ifdef LOCAL
#include "algo/debug.h"
#else
#define debug(...) 42
#endif

const int MAXH = 103;

void solve() {
  int n, h, k, s;
  cin >> n >> h >> k >> s;

  int mx_delta = 0;
  vector<array<int, 2>> a(n);
  for (auto& [l, r] : a) {
    cin >> l >> r;
    mx_delta = max(mx_delta, r - l);
  }

  if (mx_delta <= 100) {
    int cur = 0;
    vector<bitset<MAXH>> dp(2);
    dp[cur][s - a[0][0]] = 1;
   
    for (int i = 1; i < n; i++) {
      cur ^= 1;
      dp[cur].reset();
      for (int j = 1; j < a[i][1] - a[i][0]; j++) {
        int real_value = a[i][0] + j;
        if (a[i - 1][0] < real_value + 1 && real_value + 1 < a[i - 1][1] && dp[cur ^ 1][real_value + 1 - a[i - 1][0]]) {
          dp[cur][j] = 1;
        }
        if (a[i - 1][0] < real_value - k && real_value - k < a[i - 1][1] && dp[cur ^ 1][real_value - k - a[i - 1][0]]) {
          dp[cur][j] = 1;
        }
      }
    }
   
    for (int i = 1; i < a[n - 1][1] - a[n - 1][0]; i++) {
      if (dp[cur][i]) {
        cout << "Yes\n";
        return;
      }
    }
   
    cout << "No\n";
  } else if (k == 1) {
    a[0][0] = s;
    a[0][1] = s;

    for (int i = 1; i < n; i++) {
      int new_a0 = a[i][0];
      if (a[i - 1][0] - 1 > a[i][0]) {
        if (a[i - 1][0] - 1 < a[i][1]) {
          new_a0 = a[i - 1][0] - 1;
        } else {
          cout << "No\n";
          return;
        }
      } else {
        ++new_a0;
        if (new_a0 % 2 == a[i - 1][0] % 2) {
          ++new_a0;
        }
      }

      if (a[i - 1][1] + 1 < a[i][1]) {
        if (a[i - 1][1] + 1 > a[i][0]) {
          a[i][1] = a[i - 1][1] + 1;
        } else {
          cout << "No\n";
          return;
        }
      } else {
        --a[i][1];
        if (a[i][1] % 2 == a[i - 1][1] % 2) {
          --a[i][1];
        }
      }

      a[i][0] = new_a0;

      if (a[i][0] > a[i][1]) {
        cout << "No\n";
        return;
      }
    }

    cout << "Yes\n";
  } else {
    for (int mask = 0; mask < (1 << (n - 1)); mask++) {
      int cur = s;
      bool ok = true;
      for (int i = 0; i < n - 1; i++) {
        if (mask >> i & 1) {
          cur += k;
        } else {
          cur -= 1;
        }
        ok &= a[i + 1][0] < cur && cur < a[i + 1][1];
      }
      if (ok) {
        cout << "Yes\n";
        return;
      }
    }
    cout << "No\n";
  }
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  int tc;
  cin >> tc;
  while (tc --> 0) solve();
  return 0;
}
