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
    vector<int> is_record(n);
    int mx = 0;
    for (int i = 0; i < n; ++i) {
        if (a[i] > mx) {
            is_record[i] = 1;
        }
        mx = max(mx, a[i]);
    }
    vector<int> dp(n);
    for (int i = 0; i < n; ++i) {
        dp[i] = n - i;
    }
    vector<int> pref(n + 1);
    pref[0] = 1;
    for (int i = 0; i < n; ++i) {
        pref[i + 1] = mul(pref[i], is_record[i] ? i + 1 : 1); 
    }
    int ans = 0;
    for (int i = n - 1; i >= 0; --i) {
        int cur = dp[i];
        if (is_record[i]) {      
            for (int j = 0; j < i; ++j) {
                cur = add(cur, dp[j]);
            }
        }
        ans = add(ans, mul(cur, mul(pref[i], a[i])));
        if (is_record[i]) {
            vector<int> ndp(n);
            for (int j = 0; j < i; ++j) {
                ndp[j] = add(ndp[j], mul(dp[j], i - j));
                ndp[j] = add(ndp[j], mul(dp[j + 1], j + 1));
            }
            dp = ndp;
        }
    }
    cout << ans << endl;    
    return 0;
}
