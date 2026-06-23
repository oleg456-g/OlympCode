#include <bits/stdc++.h>

#define int long long
using namespace std;

signed main() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    vector<int> b(n);
    vector<int> x(q);
    int max_b = 0;
    int id = -1;
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    for (int i = 0; i < n; ++i) {
        b[i] = 1;
        if (max_b < b[i]) {
            max_b = b[i];
            id = i;
        }
    }
    for (int i = 0; i < q; ++i) {
        cin >> x[i];
    }
    for (int i = 0; i < q; ++i) {
        int l = 0;
        int r = (x[i] + a[id]) / max_b + 1;
        while (l + 1 < r) {
            int mid = (l + r) / 2;
            int left = x[i];
            bool ok = true;
            for (int j = 0; j < n; ++j) {
                if (b[j] * mid > a[j]) {
                    int need = b[j] * mid - a[j];
                    if (need > left) {
                        ok = false;
                        break;
                    }
                    left -= need;
                }
            }
            if (ok) {
                l = mid;
            } else {
                r = mid;
            }
        }
        cout << l << ' ';
    }
}
