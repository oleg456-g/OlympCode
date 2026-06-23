// #pragma GCC target("avx2")
// #pragma GCC optimize("O3,unroll-loops")
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
 
using namespace __gnu_pbds;
using namespace std;
#define pb push_back
#define ll long long
#define all(x) (x).begin(), (x).end()
 
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
const int MAXN = 1e5 + 10, MOD = 998244353, INF = 1e9 + 7;
 
struct fenwick{
  vector<gp_hash_table<int, int>> tree;
  int n, m;
 
  fenwick(int n, int m) : n(n), m(m) {
    tree.assign(n + 1, {});
  }
 
  void set(int x, int y, int val) {
    if (x < 0 || y < 0) {
      return;
    }
    x++;
    y++;
    for (int i = x; i <= n; i += (i & (-i))) {
      for (int j = y; j <= m; j += (j & (-j))) {
        // if (tree[i].find(j) == tree[i].end()) {
        //   tree[i][j] = val;
        // } else {
          tree[i][j] = max(tree[i][j], val);
        // }
      }
    }
  }
 
  int get(int x, int y) {
    if (x < 0 || y < 0) {
      return -INF;
    }
    x++;
    y++;
    int ans = -INF;
    for (int i = x; i > 0; i -= (i & (-i))) {
      for (int j = y; j > 0; j -= (j & (-j))) {
        if (tree[i].find(j) != tree[i].end())
          ans = max(ans, tree[i][j]);
      }
    }
    return ans;
  }
};
 
void solve() {
  int n, max_coord = -INF, max_time = -INF;
  cin >> n;
  vector<int> a(n), l(n), r(n);
  for (int i = 0; i < n; ++i) {
    cin >> a[i] >> l[i]  >> r[i];
    max_coord = max(max_coord, a[i]);
    max_time = max(max_time, r[i]);
  }
  vector<tuple<int, int, int>> req;
 
  for (int i = 0; i < n; ++i) {
    req.pb({r[i], a[i], l[i]});
  }
 
  sort(all(req));
  max_coord++;
  max_coord++;
  max_time++;
 
  fenwick delta_pls(max_coord, max_coord + max_time); // [x, y] -> [max_coord - x, max_coord + max_time - (x + y)] - (x + y)
  fenwick delta_mns(max_coord, max_coord + max_time); // [x, y] -> [x, x - y + max_time] - (y - x)
  fenwick angle(max_coord + max_time, max_coord + max_time);
  
  auto upd = [&](int x, int y, int val) {
    delta_pls.set(max_coord - x, max_coord + max_time - (x + y), val - (x + y) + INF);
    
    delta_mns.set(x, x - y + max_time, val - (y - x) + INF);
 
    angle.set(x + y, y - x + max_coord, val + INF);
  };
 
  upd(0, 0, 0);
  
  int ans = 0;
 
  for (auto [r, pos, l]: req) {
    
    int x = pos, y = l - 1;
 
    int res_pls = delta_pls.get(max_coord - x, max_coord + max_time - (x + y)) + (x + r);
 
    int res_mns = delta_mns.get(x, x - y + max_time) + (r - x);
 
    int res_angle = angle.get(x + y, y - x + max_coord) + (r - l + 1);
 
    int dp = max({res_pls, res_mns, res_angle}) - INF;
 
    if (dp > 0) {
      upd(pos, r, dp);
    }
    ans = max(ans, dp);
  }
 
  cout << ans << '\n';
}
 
 
signed main() {
  ios::sync_with_stdio(0);
  cin.tie(0);
 
  int T = 1;
  // cin >> T;
  while (T--) {
    solve();
  }
}
