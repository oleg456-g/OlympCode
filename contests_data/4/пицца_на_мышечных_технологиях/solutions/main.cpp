#include <bits/stdc++.h>

using namespace std;

const long long INF = 1e18 + 228;

void solve() {
    int n, k;
    cin >> n >> k;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    long long l = 0, r = INF;
    while (r - l > 1) {
        long long mid = (l + r) / 2;
        int ind1 = n, ind2 = -1;
        long long sum1 = mid, sum2 = mid;
        for (int i = 0; i < n; ++i) {
            if (a[i] >= sum1) {
                ind1 = i;
                break;
            } else {
                sum1 -= a[i];
            }
        }
        for (int i = n - 1; i >= 0; --i) {
            if (a[i] >= sum2) {
                ind2 = i;
                break;
            } else {
                sum2 -= a[i];
            }
        }
        if (ind2 - ind1 <= k) {
            r = mid;
        } else {
            l = mid;
        }
    }
    cout << r << '\n';
}

int main(int argc, char* argv[]) {
    int t;
    cin >> t;
    while (t--)
        solve();
}