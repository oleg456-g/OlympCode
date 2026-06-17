#include "bits/stdc++.h"

using namespace std;

using ll = long long;
using ld = long double;

ll get(ll l, ll r, ll x) {
    return (r + x) / x - (l > 0 ? (l - 1 + x) / x : 0);
}

void solve () {
    ll L, R;
    cin >> L >> R;
    cout << get(L, R, 4) - get(L, R, 100) + get(L, R, 400) << '\n';
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int t = 1;
//    cin >> t;
    while (t--) {
        solve();
    }
}
