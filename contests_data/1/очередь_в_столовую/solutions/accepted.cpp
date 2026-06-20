#include "bits/stdc++.h"

using namespace std;

using ll = long long;
using ld = long double;

void solve () {
    int n, d;
    cin >> n >> d;
    vector<pair<ll, ll>> a(n);
    ll s = 0;
    for (int i = 0; i < n; i++) {
        cin >> a[i].first >> a[i].second;
        a[i].first -= s;
        s += a[i].second;
    }
    int ans = n;
    while (ans && a[ans - 1].first >= d) {
        ans--;
    }
    cout << ans + 1 << '\n';
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int t = 1;
    cin >> t;
    while (t--) {
        solve();
    }
}
