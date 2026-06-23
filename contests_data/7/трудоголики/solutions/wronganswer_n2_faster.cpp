// #pragma GCC target("avx2,bmi2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")
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
  }

  vector<tuple<int, int, int>> req = {{0, 0, 0}};

  for (int i = 0; i < n; ++i) {
    req.pb({r[i], a[i], l[i]});
  }

  sort(all(req));
  
  for (int i = 0; i < n; ++i) {
    r[i] = get<0>(req[i]);
    a[i] = get<1>(req[i]);
    l[i] = get<2>(req[i]);
  }

  vector<int> dp(n + 1, -INF);
  dp[0] = 0;
  for (int i = 1; i <= n; ++i) {
    auto [r1, pos, l] = req[i];
    for (int j = 0; j < i; ++j) {
      dp[i] = max(dp[i], dp[j] + min(r1 - l + 1, r1 - (r[j] + abs(pos - a[j]))));
    }
  }
  cout << *max_element(all(dp)) << '\n';
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