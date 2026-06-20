#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    cin >> t;
    while (t--) {
        ll n, h, k, s;
        cin >> n >> h >> k >> s;
        vector<ll> l(n), r(n);
        k++;
        for (int i = 0; i < n; i++) {
            cin >> l[i] >> r[i];
            l[i] += i;
            r[i] += i;
            l[i] -= s;
            r[i] -= s;
            if (r[i] < 0) {
                r[i] = 0;
            } else {
                r[i] = (r[i] / k + (r[i] % k != 0));
            }
            if (l[i] < 0) {
                l[i] = 0;
            } else {
                l[i] = l[i] / k + 1;
            }
        }
        for (int i = 1; i < n; i++) {
            l[i] = max(l[i], l[i - 1]);
        }
        for (int i = n - 2; i >= 0; i--) {
            r[i] = min(r[i], r[i + 1]);
        }
        s = 0;
        bool ok = true;
        for (int i = 1; i < n && ok; i++) {
            if (r[i] <= l[i]) ok = false;
            s++;
            if (s >= r[i]) s--;
            if (s < l[i] || s >= r[i]) ok = false;
        }
        if (ok) {
            cout << "Yes\n";
        } else {
            cout << "No\n";
        }
    }
    return 0;
}
