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
        ll l = s;
        ll r = s;
        assert(segs[0].first < l and r < segs[0].second);
        bool ok = true;
        for (int i = 1; i < n; i += 1) {
            l -= 1;
            r += k;
            while (l <= segs[i].first) {
                l += k + 1;
            }
            while (r >= segs[i].second) {
                r -= k + 1;
            }
            if (l > r) {
                ok = false;
                break;
            }
        }
        if (ok) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
