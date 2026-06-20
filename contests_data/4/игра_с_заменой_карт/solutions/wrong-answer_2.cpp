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
            cout << (x + 1) % m << endl;
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