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
    int mx = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] > mx) {
            is_record[i] = 1;
        }
        mx = max(mx, a[i]);
    }
    int ans = 0;
    for (int p = 0; p < n; ++p) {
        vector<int> dp(n);
        if (is_record[p]) {
            for (int i = 0; i <= p; ++i) {
                dp[i] = 1;
            }
        } else {
            dp[p] = 1;
        }
        for (int i = p + 1; i < n; ++i) {
            if (is_record[i]) {
                vector<int> ndp(n);
                for (int j = 0; j < i; ++j) {
                    ndp[j] = add(ndp[j], mul(dp[j], i - j));
                    ndp[j + 1] = add(ndp[j + 1], mul(dp[j], j + 1));
                }
                dp = ndp;
            }
        }
        int sum = 0; 
        for (int i = 0; i < n; ++i) {
            sum = add(sum, mul(dp[i], n - i));
        }
        for (int i = 0; i < p; ++i) {
            if (is_record[i]) {
                sum = mul(sum, i + 1);
            }
        }
        sum = mul(sum, a[p]);
        ans = add(ans, sum);
    }  
    cout << ans << endl;
    return 0;
}
