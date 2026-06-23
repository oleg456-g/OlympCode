#include <bits/stdc++.h>

#define int long long
using namespace std;

signed main() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    for (int i = 0; i < q; ++i) {
        int x;
        cin >> x;
        cout << a[0] + x/n << ' ';
    }
}
