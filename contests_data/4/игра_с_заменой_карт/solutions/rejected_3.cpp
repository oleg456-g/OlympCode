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
    int suma = accumulate(a.begin(), a.end(), 0) % m;
    if (s[suma % m] == '1') {
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
        return;
    }
    if (count(s.begin(), s.end(), '1') == m - 1) {
        cout << "Alice" << endl;
        while (a.size() > 1) {
            int x = a.back();
            a.pop_back();
            int y = a.back();
            a.pop_back();
            cout << "unite " << x << ' ' << y << endl;
            a.push_back((x + y) % m);
        }
        cout << "replace " << a.back() << endl;
        int y;
        cin >> y;
        cout << "done" << endl;
        return;
    }
    cout << "Alice" << endl;
    while (a.size() > 1) {
        int x = a.back();
        a.pop_back();
        int y = a.back();
        a.pop_back();
        cout << "unite " << x << ' ' << y << endl;
        a.push_back((x + y) % m);
    }
    int x = a.back();
    while (s[x] == '0') {
        cout << "replace " << x << endl;
        int y;
        cin >> y;
        x = y;
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