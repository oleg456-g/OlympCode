#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
typedef unsigned long long ull;

#define F first
#define S second

void solve() {
    ll n, m;
    cin >> n >> m;
    vector<ll> v(n);
    for (auto &el: v)
        cin >> el;
    string s;
    cin >> s;
    ll cnt0 = 0;
    for (auto &el: s)
        cnt0 += el == '0';
    ll sum = 0;
    for (auto &el: v)
        sum += el;
    sum %= m;
    if (s[sum] == '1' || (s[sum] == '0' && cnt0 == 1)) {
        cout << "Alice" << endl;
        cout << "done" << endl;
    } else {
        cout << "Bob" << endl;
        while (true) {
            string c;
            cin >> c;
            if (c == "done")
                break;
            if (c == "unite") {
                ll x, y;
                cin >> x >> y;
            } else if (c == "replace") {
                ll x;
                cin >> x;
                cout << (x + 1) % m << endl;
            }
        }
    }
}

signed main() {
    int tt = 1;
    cin >> tt;
    for (int _ = 0; _ < tt; _++)
        solve();
}
