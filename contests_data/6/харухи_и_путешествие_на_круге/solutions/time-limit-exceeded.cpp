#include <bits/stdc++.h>

// #pragma GCC optimize("O3,unroll-loops")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#define LF '\n'
#define SP ' '
#define all(x) x.begin(), x.end()
#define rep(i, a, b) for (int i = a; i < (b); i++)
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;

int n, q;
int dist(int i, int j) {
    int d = abs(i - j);
    return min(d, n - d);
}

void solve() {
    cin >> n >> q;
    n *= 2;

    vector<int> a(n);
    rep(i, 0, n) cin >> a[i];

    rep(i, 0, q) {
        int u, v;
        cin >> u >> v;
        u--; v--;
        int ans = dist(u, v);
        rep(j, 0, n) {
            int j_mirrored = (j + (n / 2)) % n;
            int local_ans = dist(u, j) + a[j] + dist(j_mirrored, v);
            ans = min(ans, local_ans);
        }
        cout << ans << LF;
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t --> 0) {
        solve();
    }
}
