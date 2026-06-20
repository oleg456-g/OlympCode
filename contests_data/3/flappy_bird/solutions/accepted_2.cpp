#include <bits/stdc++.h>

using namespace std;

using ll = long long;
using ld = long double;

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()
#define rep(i, n) for (int i = 0; i < (n); i += 1)
#define len(a) ((int)(a).size())

int32_t main() {
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
        ll l = s;
        ll r = s;
        assert(segs[0].first < l and r < segs[0].second);
        bool ok = true;
        for (int i = 1; i < n; i += 1) {
            l -= 1;
            r += k;
            ll tl = -1;
            ll tr = 1e9;
            while (tl + 1 < tr) {
                ll tm = (tl + tr) / 2;
                if (l + (k + 1) * tm < segs[i].second) {
                    tl = tm;
                } else {
                    tr = tm;
                }
            }
            if (tl == -1) {
                ok = false;
                break;
            }
            r = min(r, l + (k + 1) * tl);
            tl = -1;
            tr = 1e9;
            while (tl + 1 < tr) {
                ll tm = (tl + tr) / 2;
                if (r - (k + 1) * tm > segs[i].first) {
                    tl = tm;
                } else {
                    tr = tm;
                }
            }
            if (tl == -1) {
                ok = false;
                break;
            }
            l = max(l, r - (k + 1) * tl);
            assert(l <= r);
        }
        if (ok) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
