#include <bits/stdc++.h>

using namespace std;
#define int long long

#ifdef LOCAL
#include "algo/debug.h"
#else
#define debug(...) 42
#endif

void solve() {
  int n, h, k, s;
  cin >> n >> h >> k >> s;
  vector<array<int, 2>> a(n);
  for (auto& [l, r] : a) {
    cin >> l >> r;
  }

  bool ok = true;
  for (int i = 1; i < n; i++) {
    int x = a[i - 1][0] + 1;
    int y = a[i][0] + 1;

    ok &= ((x + k) == y) || ((x - 1) == y);
  }

  cout << (ok ? "Yes" : "No") << '\n';
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  int tc;
  cin >> tc;
  while (tc --> 0) solve();
  return 0;
}
