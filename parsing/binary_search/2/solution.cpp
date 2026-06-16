#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <set>
#include <deque>

using namespace std;

int main() {
    int n, m, l, r;
    long long mid, k;
    cin >> n >> m;
    vector<long long> a(n, 0);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    for (int i = 0; i < m; i++) {
        cin >> k;
        l = -1;
        r = n;
        if (k <= a[0]) {
            cout << a[0] << '\n';
            continue;
        }
        if (k >= a[n - 1]) {
            cout << a[n - 1] << '\n';
            continue;
        }
        while (r - l > 1) {
            mid = (l + r) / 2;
            if (k < a[mid]) {
                r = mid;
            } else {
                l = mid;
            }
        }
        if (abs(a[l] - k) <= abs(a[r] - k)) {
            cout << a[l] << '\n';
        } else {
            cout << a[r] << '\n';
        }
    }
}