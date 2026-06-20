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
  int neg = 0, pos = 0;
  vector<int> a(n);
  for (auto& x : a) {
    cin >> x;
    if (x < 0) {
      neg -= x;
    } else {
      pos += x;
    }
  }

  cout << max(neg, pos) << '\n';
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  solve();
  return 0;
}
