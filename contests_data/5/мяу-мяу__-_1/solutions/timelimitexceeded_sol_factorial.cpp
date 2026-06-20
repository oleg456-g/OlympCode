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
  vector adj(n, vector<int>(n));
  for (int i = 0; i < n - 1; ++i) {
    int u, v;
    cin >> u >> v;
    --u, --v;
    adj[u][v] = adj[v][u] = 1;
  }
  int ans = 0;
  vector<int> p(n);
  iota(p.begin(), p.end(), 0);
  do {
    bool ok = true;
    for (int i = 0; ok && i < n; ++i) {
      for (int j = 0; j < i; ++j) {
        if (adj[i][j] != adj[p[i]][p[j]]) {
          ok = false;
          break;
        }
      }
    }
    ans += ok;
  } while (next_permutation(p.begin() + 1, p.end()));
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
