#include "bits/stdc++.h"
using namespace std;

void solve() {
    int n;
    cin >> n;
    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    vector<int> b(n + 1);
    b[0] = a[0];
    for (int i = 1; i < n; i++) {
        b[i] = a[i] ^ a[i - 1];
    }
    b[n] = a[n - 1];
    vector<int> ls, rs, xs;
    for (int i = 1; i <= n / 2; i++) {
        ls.push_back(i + 1);
        rs.push_back(n);
        xs.push_back(b[i]);
        b[n] ^= b[i];
    }
    for (int i = n / 2 + 1; i < n; i++) {
        ls.push_back(1);
        rs.push_back(i);
        xs.push_back(b[i]);
        b[0] ^= b[i];
    }
    int k = (int) ls.size();
    cout << k << '\n';
    for (int i = 0; i < k; i++) {
        cout << ls[i] << " " << rs[i] << " " << xs[i] << '\n';
    }
}

signed main() {
    //cout << fixed << setprecision(20);
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    cin >> T;
    //cin >> G;
    while (T--)
        solve();
    return 0;
}
