#include <bits/stdc++.h>

using namespace std;
#define int long long

signed main() {
    int n, q;
    cin >> n >> q;
    vector<int> a(n);
    int suma = 0;
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
        suma += a[i];
    }
    sort(a.begin(), a.end());
    int pref[n + 1];
    pref[0] = 0;
    for (int i = 1; i <= n; ++i) {
        pref[i] = pref[i - 1] + a[i - 1];
    }
    for (int i = 0; i < q; ++i) {
        int x;
        cin >> x;
        int l = 0;
        int r = (x + suma) / n + 1;
        while (l + 1 < r) {
            int mid = (l + r) / 2;
            int need = 0;
            int it = lower_bound(a.begin(), a.end(), mid) - a.begin();
            need = it * mid - pref[it];
            if (need <= x) {
                l = mid;
            } else {
                r = mid;
            }
        }
        cout << l << "\n";
    }
}
