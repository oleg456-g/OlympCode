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
        bool result = false;
        rep(mask, (1 << min(30, (n - 1)))) {
            ll pos = s;
            bool ok = true;
            for (int i = 0; i < n - 1; i += 1) {
                if ((mask >> i) & 1) {
                    pos += k;
                } else {
                    pos -= 1;
                }
                if (!(segs[i + 1].first < pos and pos < segs[i + 1].second)) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                result = true;
                break;
            }
        }
        if (result) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
