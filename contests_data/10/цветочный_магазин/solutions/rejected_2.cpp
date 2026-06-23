#include <bits/stdc++.h>

using namespace std;
#define int long long

signed main() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    vector<int> x(q);
    for (int i = 0; i < q; ++i) {
        cin >> x[i];
    }
    for (int i = 0; i < q; ++i) {
        cout << x[i] / n << " ";
    }
}
