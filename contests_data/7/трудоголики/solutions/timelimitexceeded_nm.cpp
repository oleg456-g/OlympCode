// #pragma GCC target("avx2")
// #pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <bits/stdc++.h>
 
using namespace std;
#define pb push_back
#define ll long long
#define all(x) (x).begin(), (x).end()
 
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
const int MAXN = 1e6 + 10, MOD = 998244353, INF = 1e9 + 7;

void solve() {
  int n;
  cin >> n;
  vector<int> a(n), l(n), r(n);
  for (int i = 0; i < n; ++i) {
    cin >> a[i] >> l[i]  >> r[i];
    --r[i];
  }

  int max_r = *max_element(all(r)), max_a = *max_element(all(a));
  vector<int> l_pos(max_a + 1, -1), r_pos(max_a + 1, -1);
  for (int i = 0; i < n; ++i) {
    l_pos[a[i]] = l[i];
    r_pos[a[i]] = r[i];
  }

  vector<vector<int>> dp(max_r + 1, vector<int>(max_a + 1, -INF));
  dp[0][1] = 0;

  for (int time = 1; time <= max_r; ++time) {
    for (int pos = 0; pos <= max_a; ++pos) {
      int in = (l_pos[pos] <= time && time <= r_pos[pos]);
      if (pos != 0) {
        dp[time][pos] = max(dp[time][pos], dp[time - 1][pos - 1]);
      }
      dp[time][pos] = max(dp[time][pos], dp[time - 1][pos] + in);
      if (pos != max_a) {
        dp[time][pos] = max(dp[time][pos], dp[time - 1][pos + 1]);
      }
    }
  }
  cout << *max_element(all(dp[max_r]));
}
signed main() {
  ios::sync_with_stdio(0);
  cin.tie(0);
 
  int T = 1;
//   cin >> T;
  while (T--) {
    solve();
  }
}