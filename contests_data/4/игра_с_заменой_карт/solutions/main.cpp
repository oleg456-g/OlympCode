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
    int sum2 = 0;
    for (int i = 0; i < m; i++) {
        if (s[i] == '0' && i != suma) {
            sum2 = i;
        }
    }
    int add = (m + sum2 - suma) % m;
    cout << "Bob" << endl;
    while (true) {
        string cmd;
        cin >> cmd;
        if (cmd == "done") {
            return;
        }
        if (cmd == "unite") {
            int x, y;
            cin >> x >> y;
            continue;
        }
        if (cmd == "replace") {
            int x;
            cin >> x;
            cout << (x + add) % m << endl;
            add = (m - add) % m;
            continue;
        }
        assert(false);
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