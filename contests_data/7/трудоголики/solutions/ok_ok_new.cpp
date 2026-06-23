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
 
  fenwick(int x, int y) : n(x + 10), m(y + 10) {
    tree.assign(x + 20, {});
  }
 
  void set(int x, int y, int val) {
    if (x < 0 || y < 0) {
      return;
    }
    x++;
    y++;
    for (int i = x; i <= n; i += (i & (-i))) {
      for (int j = y; j <= m; j += (j & (-j))) {
          tree[i][j] = max(tree[i][j], val);
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
  int n;
  cin >> n;
  vector<int> a(n), l(n), r(n);
  vector<int> x_ = {1}, x_pls_y = {1}, x_mns_y = {1};
  
  for (int i = 0; i < n; ++i) {
    cin >> a[i] >> l[i] >> r[i];
    --r[i];
    int x = a[i], y = r[i];
    x_.pb(x);
    x_pls_y.pb(x + y);
    x_pls_y.pb(x + l[i] - 1);
    x_mns_y.pb(x - y);
    x_mns_y.pb(x - (l[i] - 1));
  }
 
  sort(all(x_));
  sort(all(x_pls_y));
  sort(all(x_mns_y));
 
  x_.resize(unique(all(x_)) - x_.begin());
  x_pls_y.resize(unique(all(x_pls_y)) - x_pls_y.begin());
  x_mns_y.resize(unique(all(x_mns_y)) - x_mns_y.begin());
 
  vector<tuple<int, int, int>> req;
 
  for (int i = 0; i < n; ++i) {
    req.pb({r[i], a[i], l[i]});
  }
 
  sort(all(req));
 
  fenwick delta_pls(x_.size(), x_pls_y.size()); // [x, y] -> [max_coord - x, max_coord + max_time - (x + y)] - (x + y)
  fenwick delta_mns(x_.size(), x_mns_y.size()); // [x, y] -> [x, x - y + max_time] - (y - x)
  fenwick angle(x_pls_y.size(), x_mns_y.size());
  
  auto upd = [&](int x, int y, int val) {
    int ind_x = lower_bound(all(x_), x) - x_.begin();
    int ind_x_pls_y = lower_bound(all(x_pls_y), x + y) - x_pls_y.begin();
    int ind_x_mns_y = lower_bound(all(x_mns_y), x - y) - x_mns_y.begin();
 
    delta_pls.set(x_.size() - ind_x - 1, x_pls_y.size() - ind_x_pls_y - 1, val - (x + y) + INF);
    delta_mns.set(ind_x, ind_x_mns_y, val - (y - x) + INF);
    angle.set(ind_x_pls_y, x_mns_y.size() - ind_x_mns_y - 1, val + INF);
  };
 
  auto get = [&](int r, int pos, int l) {
    int x = pos, y = l - 1;
    int ind_x = lower_bound(all(x_), x) - x_.begin();
    int ind_x_pls_y = lower_bound(all(x_pls_y), x + y) - x_pls_y.begin();
    int ind_x_mns_y = lower_bound(all(x_mns_y), x - y) - x_mns_y.begin();
 
    int a = delta_pls.get(x_.size() - ind_x - 1, x_pls_y.size() - ind_x_pls_y - 1)  + (x + r);
    int b = delta_mns.get(ind_x, ind_x_mns_y) + (r - x);
    int c = angle.get(ind_x_pls_y, x_mns_y.size() - ind_x_mns_y - 1) + (r - l + 1);
 
    int dp = (long long) max({a, b, c}) - INF;
    
    return dp;
  };
  
  upd(1, 0, 0);
  int ans = 0;
 
  for (auto [r, pos, l]: req) {
    int dp = get(r, pos, l);
 
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
