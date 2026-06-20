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

  vector<vector<int>> prime_divs(n);
  for (int i = 0; i < n; i++) {
    for (int j = 2; j * j <= a[i]; j++) {
      if (a[i] % j == 0) {
        int cur = 0;
        while (a[i] % j == 0) {
          a[i] /= j;
          cur ^= 1;
        }
        if (cur) {
          prime_divs[i].push_back(j);
        }
      }
    }
    if (a[i] != 1) {
      prime_divs[i].push_back(a[i]);
    }
  }

  int result = n;
  for (int mask = 0; mask < 1 << (n - 1); mask++) {
    bool ok = true;
    int beg = 0;
    for (int i = 1; i <= n; i++) {
      if (i == n || (mask >> (i - 1) & 1)) {
        for (int l = beg; l < i && ok; l++) {
          set<int> st;
          for (int r = l; r < i; r++) {
            for (const auto& div : prime_divs[r]) {
              if (st.contains(div)) {
                st.erase(div);
              } else {
                st.insert(div);
              }
            }

            if (st.empty()) {
              ok = false;
              break;
            }
          }
        }

        beg = i;
      }
    }

    if (ok) {
      result = min(result, static_cast<int>(__builtin_popcount(mask) + 1));
    }
  }

  cout << result << '\n';
}

signed main() {
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  solve();
  return 0;
}
