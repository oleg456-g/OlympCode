#include <bits/stdc++.h>

using namespace std;

const int md = 1e9 + 7;

int add(int a, int b) {
    return a + b < md ? a + b : a + b - md;
}

int sub(int a, int b) {
    return a - b >= 0 ? a - b : a - b + md;
}

int mul(int a, int b) {
    return a * 1ll * b % md;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    int f = 1;
    for (int i = 1; i <= n; ++i) {
        f = mul(f, i);
    }
    int inv2 = (md + 1) / 2;
    f = mul(f, n + 1);
    f = mul(f, inv2);
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum = add(sum, a[i]);
    }
    int ans = mul(f, sum);
    cout << ans << "\n";
    return 0;
}
