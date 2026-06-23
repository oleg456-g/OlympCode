// #pragma GCC target("avx2")
// #pragma GCC optimize("O3,unroll-loops")
#include <bits/stdc++.h>
 
using namespace std;
#define pb push_back
#define all(x) (x).begin(), (x).end()
 
// mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
mt19937_64 rng(179);
const int MAXN = 1e7 + 179, MOD = 998244353, INF = 1e9 + 7;
// TREAP

struct Treap{
  long long *Pr;
  int *X, *Val, *Res, *Left, *Right; 

  int last = 1, left_split = 0, right_split = 0, root = 0;

  Treap(int sz) {
    Pr = new long long[sz + 1];
    X = new int[sz + 1];
    Val = new int[sz + 1];
    Res = new int[sz + 1];
    Right = new int[sz + 1];
    Left = new int[sz + 1];
    Res[0] = -INF;
    Val[0] = -INF;
  }

  int new_node(int x, int val) {
    Pr[last] = rng();
    X[last] = x;
    Val[last] = val;
    Res[last] = val;
    Left[last] = 0;
    Right[last] = 0;
    return last++;
  }

  int merge(int l, int r) {
    if (!l)
      return r;
    if (!r)
      return l;
    if (Pr[r] > Pr[l]) {
      Left[r] = merge(l, Left[r]);
      Res[r] = max(Res[Left[r]], Res[Right[r]]);
      Res[r] = max(Res[r], Val[r]);
      return r;
    } else {
      Right[l] = merge(Right[l], r);
      Res[l] = max(Res[Left[l]], Res[Right[l]]);
      Res[l] = max(Res[l], Val[l]);
      return l;
    }
  }

  void split(int root, int val) {
    if (!root) {
      left_split = 0;
      right_split = 0;
      return;
    }
    if (X[root] <= val) {
      split(Right[root], val);
      Right[root] = left_split;
      left_split = root;
    } else {
      split(Left[root], val);
      Left[root] = right_split;
      right_split = root;
    }
    Res[root] = max(Res[Left[root]], Res[Right[root]]);
    Res[root] = max(Res[root], Val[root]); 
  }

  int max_less(int curr, int key) {
    if (curr == 0) {
      return -INF;
    } else if (X[curr] > key) {
      return max_less(Left[curr], key);
    } else if (X[curr] == key) {
      return max(max_less(Left[curr], key), Val[curr]);
    } else {
      return max({Res[Left[curr]], Val[curr], max_less(Right[curr], key)});
    }
  }

};

struct Segtree {
  vector<int> sizes;
  int sz = 1;

  Segtree(int n) {
    while (sz <= n) {
      sz *= 2;
    }
    sizes.assign(2 * sz, 0);
  }

  void set(int l, int r, int pos, int x, int y) {
    sizes[pos]++;
    if (r - l == 1) {
      return;
    }

    int mid = (l + r) / 2;
    if (x >= mid) {
      set(mid, r, pos * 2 + 2, x, y);
    } else {
      set(l, mid, pos * 2 + 1, x, y);
    }
  }

  void set(int x, int y) {
    set(0, sz, 0, x, y);
  }

};

struct MST{
  vector<Treap> vertice;
  int sz = 1;

  MST(int n, Segtree precalc_sz) {
    while (sz <= n) {
      sz *= 2;
    }
    vertice.reserve(2 * sz);
    for (int i = 0; i < 2 * sz; ++i) {
      vertice.emplace_back(precalc_sz.sizes[i]);
    }
  }

  void set(int l, int r, int pos, int x, int y, int val) {
    vertice[pos].split(vertice[pos].root, y);
    vertice[pos].root = vertice[pos].merge(vertice[pos].merge(vertice[pos].left_split, vertice[pos].new_node(y, val)), vertice[pos].right_split);
    if (r - l == 1) {
      return;
    }

    int mid = (l + r) / 2;
    if (x >= mid) {
      set(mid, r, pos * 2 + 2, x, y, val);
    } else {
      set(l, mid, pos * 2 + 1, x, y, val);
    }
  }

  void set(int x, int y, int val) {
    set(0, sz, 0, x, y, val);
  }

  int get(int l, int r, int pos, int x, int y) {
    if (r <= x) {
      return vertice[pos].max_less(vertice[pos].root, y);
    }
    if (x <= l) {
      return -INF;
    }
    int mid = (l + r) / 2;
    return max(get(l, mid, pos * 2 + 1, x, y), get(mid, r, pos * 2 + 2, x, y));
  }

  int get(int x, int y) {
    return get(0, sz, 0, x + 1, y);
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

  Segtree aa(max_coord), b(max_coord), c(max_coord + max_time);

  a.pb(0);
  r.pb(0);
  l.pb(0);

  for (int i = 0; i <= n; ++i) {
    int x = a[i];
    int y = r[i];
    aa.set(max_coord - x, max_coord + max_time - (x + y));
    
    b.set(x, x - y + max_time);
 
    c.set(x + y, y - x + max_coord);
  }


  MST delta_pls(max_coord, aa); // [x, y] -> [max_coord - x, max_coord + max_time - (x + y)] - (x + y)
  MST delta_mns(max_coord, b); // [x, y] -> [x, x - y + max_time] - (y - x)
  MST angle(max_coord + max_time, c);
  
  auto upd = [&](int x, int y, int val) {
    delta_pls.set(max_coord - x, max_coord + max_time - (x + y), val - (x + y));
    
    delta_mns.set(x, x - y + max_time, val - (y - x));
 
    angle.set(x + y, y - x + max_coord, val);
  };
 
  upd(0, 0, 0);
  
  int ans = 0;
 
  for (auto [r, pos, l]: req) {
    
    int x = pos, y = l - 1;
 
    int res_pls = delta_pls.get(max_coord - x, max_coord + max_time - (x + y)) + (x + r);
 
    int res_mns = delta_mns.get(x, x - y + max_time) + (r - x);
 
    int res_angle = angle.get(x + y, y - x + max_coord) + (r - l + 1);
 
    int dp = max({res_pls, res_mns, res_angle});
    
    if (dp >= 0) {
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
