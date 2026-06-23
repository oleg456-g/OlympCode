#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int t = 1;
  // cin >> t;

  const long double EPS = 1e-18L;

  while (t--) {
    long double w, x, a, b;
    cin >> w >> x >> a >> b;

    if (fabsl(w - x) <= EPS) {
      cout << 1 << '\n';
      continue;
    }

    long double p = (w - x) / w;
    long double q = a / b;

    ll left_k = 0, right_k = 1;

    while (powl(p, right_k) + EPS >= q) {
      right_k *= 2;
    }

    while (right_k - left_k > 1) {
      ll k = (left_k + right_k) / 2;
      if (powl(p, k) > q + EPS) {
        left_k = k;
      } else {
        right_k = k;
      }
    }

    ll k = left_k;
    ll ans = k + 1;

    long double pk = powl(p, k);

    // y = floor(p^k * (w - x) / q - (w - x))
    long double y_real = pk * (w - x) / q - (w - x);
    ll y = (ll) floorl(y_real + EPS);

    ans += (ll) floorl(((w - x) + y) / x + EPS);

    cout << ans << '\n';
  }

  return 0;
}