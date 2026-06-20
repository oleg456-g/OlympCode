#include <vector>
#include <iostream>
#include <cassert>
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
    }
    for (int i = n / 2 + 1; i < n; i++) {
        ls.push_back(1);
        rs.push_back(i);
        xs.push_back(b[i]);
    }

    vector<int> res(a);

    for (int i = 0; i < ls.size(); i++) {
        for (int j = ls[i]-1; j <= rs[i]-1; j++) {
            res[j] ^= xs[i];
        }
    }

    for (int i = 0; i < n; i++) {
        assert(res[i] == res[0]);
    }

    int k = (int) ls.size();
    cout << k << '\n';
    for (int i = 0; i < k; i++) {
        cout << ls[i] << " " << rs[i] << " " << xs[i] << '\n';
    }
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    cin >> T;
    while (T--)
        solve();
    return 0;
}
