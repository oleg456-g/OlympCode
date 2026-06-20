#include <bits/stdc++.h>

using namespace std;
#define int long long

#ifdef LOCAL
#include "algo/debug.h"
#else
#define debug(...) 42
#endif

void solve() {
  int n;
  cin >> n;
  vector<int> a(n);
  for (auto& x : a) cin >> x;

  if (n > 20) {
    while (true) {
      ;
    }
  }

  int ans = 0;
  for (int mask = 0; mask < 1 << n; mask++) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
      if (mask >> i & 1) {
        sum += a[i];
      }
    }
    ans = max(ans, abs(sum));
  }

  cout << ans << '\n';
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  solve();
  return 0;
}
