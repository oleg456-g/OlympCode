#include <bits/stdc++.h>

using namespace std;

int dtree(vector<int> a) {
    if (a.size() % 2 == 0)return -1;
    int k = (a.size() + 1) / 2;
    if (k == 1)return -1;
    if (count(a.begin(), a.end(), k) == 0)return -1;
    a.erase(find(a.begin(), a.end(), k));
    set<int> s;
    for (auto x: a)s.insert(x);
    if (s.size() == k)return k;
    return -1;
}

void solve() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    while (n > 20) {}
    vector ans(n, vector<int>(n));
    for (int l = 0; l < n; l++) {
        for (int r = l; r < n; r++) {
            set<int> ks;
            for (int mask = 0; mask < (1 << (r - l + 1)); mask++) {
                vector<int> b;
                for (int i = 0; i < r - l + 1; i++) {
                    if ((mask >> i) & 1) b.push_back(a[i + l]);
                }
                int dd = dtree(b);
                if (dd != -1)ks.insert(dd);
            }
            ans[l][r] = (int) ks.size();
        }
    }
    while (q--) {
        int l, r;
        cin >> l >> r;
        l--;
        r--;
        cout << ans[l][r] << '\n';
    }
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}