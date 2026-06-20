#include <bits/stdc++.h>

using namespace std;

void solve() {
    int n, m;
    cin >> n >> m;
    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    string s;
    cin >> s;
    cout << "Alice" << endl;
    while (a.size() > 1) {
        int x = a.back();
        a.pop_back();
        int y = a.back();
        a.pop_back();
        cout << "unite " << x << ' ' << y << endl;
        a.push_back((x + y) % m);
    }
    cout << "done" << endl;
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