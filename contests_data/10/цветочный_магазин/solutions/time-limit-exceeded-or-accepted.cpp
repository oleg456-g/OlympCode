#include <bits/stdc++.h>

#define int long long
using namespace std;

signed main() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    vector<int> b(n);
    vector<int> x(q);

    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    for (int i = 0; i < n; ++i) {
        b[i] = 1;
    }
    for (int i = 0; i < q; ++i) {
        cin >> x[i];
    }
    for (int i = 0; i < q; ++i) {
        vector<int> c(n);
        for (int j = 0; j < n; ++j) {
            c[j] = a[j];
        }
        int left = x[i];
        int cnt = 0;
        while (true) {
            bool ok = true;
            for (int j = 0; j < n; ++j) {
                if (c[j] >= b[j]) {
                    c[j] -= b[j];
                } else {
                    int need = b[j] - c[j];
                    if (left >= need) {
                        left -= need;
                        c[j] = 0;
                    } else {
                        ok = false;
                        break;
                    }
                }
            }
            if (!ok) {
                break;
            }
            cnt++;
        }
        cout << cnt << " ";
    }
}
