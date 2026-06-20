#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);
    int n, m;
    cin >> n >> m;
    int x, b, c, mod;
    cin >> x >> b >> c >> mod;
    vector<pair<int, int>> otr(n);
    for (auto& [l, r] : otr) {
        cin >> l >> r;
    }
    vector<int> a(m + 1);
    a[1] = x;
    for (int i = 2; i <= m; ++i) {
        a[i] = (1LL * a[i - 1] * b + c) % mod;
    }
    auto get = [&](int p) -> int {
        int ans = 0;
        for (auto& [l, r] : otr) {
            ans += l <= p && p <= r;
        }
        return ans;
    };
    ll ans = 0;
    for (int i = 1; i <= m; ++i) {
        ans = max(ans, 1LL * a[i] * get(i));
    }
    cout << ans;
}