#include <bits/stdc++.h>

using namespace std;

using ll = long long;
using ld = long double;

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()
#define rep(i, n) for (int i = 0; i < (n); i += 1)
#define len(a) ((int)(a).size())

int32_t main() {
    if (1) {
        ios::sync_with_stdio(0);
        cin.tie(0);
        cout.tie(0);
    }
    int t;
    cin >> t;
    rep(itr, t) {
        int n;
        ll h, k, s;
        cin >> n >> h >> k >> s;
        vector<pair<ll, ll>> segs(n);
        for (auto &[l, r] : segs) {
            cin >> l >> r;
        }
        bool ok = true;
        ll cur = s;
        for (int i = 1; i < n; i += 1) {
            ll ncur = segs[i].first + 1;
            ok = ok and (ncur == cur - 1 or ncur == cur + k);
            cur = ncur;
        }
        if (ok) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
