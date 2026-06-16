#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

#define rep(i, n) for(int i = 0; i < (n); ++i)
#define all(a) (a).begin(), (a).end()
#define ar array

using namespace std;

using ll = long long;
using ld = long double;
using db = double;
using vi = vector<int>;
using pi = pair<int, int>;
using vpi = vector<pi>;

const ll INF = 2e18;

template<typename T>
bool ckmin(T &a, const T &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}

const int mod = 1e9 + 7;

inline int add(int a, int b) {
    a += b;
    if (a >= mod) a -= mod;
    return a;
}

inline int mul(int a, int b) {
    return 1ll * a * b % mod;
}

inline int sub(int a, int b) {
    a -= b;
    if (a < 0) a += mod;
    return a;
}
const int MAXN = 5000 + 10;
int dp[MAXN][MAXN];
void solve() {
    int n;
    cin >> n;
    vector <int> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];
    vector <int> is_mx(n);
    int cur_max = -1;
    for (int i = 0; i < n; ++i) {
        if (a[i] > cur_max) {
            is_mx[i] = 1;
            cur_max = a[i];
        }
    }

    for (int i = 1; i <= n; ++i) {
        dp[n][i] = n - i + 1;
    }

    for (int i = n - 1; i >= 0; --i) {
        for (int j = 1; j <= i; ++j) {
            if (!is_mx[i]) {
                dp[i][j] = dp[i + 1][j];
            } else {
                dp[i][j] = add(mul(dp[i + 1][j + 1], j), mul(dp[i + 1][j], i + 1 - j));
            }
        }
    }

    int cur_vars = 1;
    int ans = 0;

    for (int i = 0; i < n; ++i) {
        if (is_mx[i]) {
            int kek = 0;
            for (int j = 1; j <= i + 1; ++j) {
                kek = add(kek, mul(cur_vars, dp[i + 1][j]));
            }
            ans = add(ans, mul(kek, a[i]));
            cur_vars = mul(cur_vars, i + 1);
        } else {
            ans = add(ans, mul(cur_vars, mul(dp[i + 1][i + 1], a[i])));
        }
    }
    cout << ans;
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout << setprecision(12) << fixed;
    int t = 1;
//    cin >> t;
    while (t--) {
        solve();
    }
}
