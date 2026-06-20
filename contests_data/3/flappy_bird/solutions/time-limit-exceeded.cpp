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
        vector<ll> cur = {s};
        for (int i = 1; i < n; i += 1) {
            vector<ll> ncur;
            for (auto x : cur) {
                if (segs[i].first < x - 1 and x - 1 < segs[i].second) {
                    ncur.push_back(x - 1);
                }
                if (segs[i].first < x + k and x + k < segs[i].second) {
                    ncur.push_back(x + k);
                }
            }
            sort(all(ncur));
            ncur.resize(unique(all(ncur)) - ncur.begin());
            cur = ncur;
        }
        if (!cur.empty()) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
