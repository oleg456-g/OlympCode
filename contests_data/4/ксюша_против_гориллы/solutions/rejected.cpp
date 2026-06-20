#include <bits/stdc++.h>

using namespace std;

#define MAXN 200200

int n, n2;
int arr[MAXN], brr[MAXN << 1], buf[MAXN << 1];

bool check(int mask) {
    int k = 0;
    for (int i = 0; i < n2; ++i) {
        if (mask >> i & 1) arr[k++] = brr[i];
    }
    sort(arr, arr + n);
    memcpy(buf, arr, n * sizeof(*arr));
    partial_sum(arr, arr + n, buf + n);
    sort(buf, buf + n2);
    return !memcmp(buf, brr, n2 * sizeof(*brr));
}

void solve() {
    sort(brr, brr + (n << 1));
    for (int mask = 0; mask < (1 << n2); ++mask) {
        if (__builtin_popcount(mask) == n && check(mask))
            return;
    }
    assert(false);
}

int main() {
    int t; cin >> t;
    while (t--) {
        cin >> n, n2 = n << 1;
        for (int i = 0; i < n2; ++i)
            cin >> brr[i];
        solve();
        for (int i = 0; i < n; ++i)
            cout << arr[i] << ' ';
        cout << '\n';
    }
}