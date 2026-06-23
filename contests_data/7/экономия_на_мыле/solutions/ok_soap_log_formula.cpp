#include <bits/stdc++.h>

using namespace std;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  long long A, B, W, X;
  cin >> W >> X >> A >> B;

  if (X == W) {
    cout << 1 << '\n';
    return 0;
  }

  long double log_q = log1pl(-(long double)X / (long double)W);
  long double target = logl((long double)A) - logl((long double)B);

  long long k = (long long)(target / log_q) + 1;
  if (k < 1) k = 1;

  while (k > 0 && (long double)(k - 1) * log_q < target) --k;
  while ((long double)k * log_q >= target) ++k;

  long double log_y = logl((long double)W) + logl((long double)B)
                    - logl((long double)A) + (long double)k * log_q;
  long double log_x = logl((long double)X);

  long long m = (long long)expl(log_y - log_x);
  const long double EPS = 1e-12L;

  while (logl((long double)(m + 1)) + log_x <= log_y + EPS) ++m;
  while (m > 0 && logl((long double)m) + log_x > log_y + EPS) --m;

  cout << k + m << '\n';
  return 0;
}