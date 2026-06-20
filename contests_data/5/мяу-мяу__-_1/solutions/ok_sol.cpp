#include <bits/stdc++.h>

using namespace std;

using ll = long long;

int const MOD = 998244353;

int mul(ll x, int y) {
  return int(x * y % MOD);
}

void solveTest() {
  int n;
  cin >> n;
  vector<vector<int>> graph(n);
  for (int i = 0; i < n - 1; ++i) {
    int u, v;
    cin >> u >> v;
    --u, --v;
    graph[u].push_back(v);
    graph[v].push_back(u);
  }
  int ans = 1;
  map<vector<int>, int> mp;
  auto dfs = [&](auto self, int v, int p = -1) -> int {
    vector<int> children;
    for (int u : graph[v]) {
      if (u == p) {
        continue;
      }
      children.push_back(self(self, u, v));
    }
    sort(children.begin(), children.end());
    int cur = 1;
    for (int i = 0; i < int(children.size()); ++i) {
      if (i > 0 && children[i] == children[i - 1]) {
        ans = mul(ans, ++cur);
      } else {
        cur = 1;
      }
    }
    if (!mp.count(children)) mp[children] = int(mp.size());
    return mp[children];
  };
  dfs(dfs, 0);
  cout << ans << "\n";
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int t; cin >> t;
  while (t--) {
    solveTest();
  }

  return 0;
}
