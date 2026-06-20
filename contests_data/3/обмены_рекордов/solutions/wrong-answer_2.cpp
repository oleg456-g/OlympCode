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
    vector<int> records;
    int mx = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] > mx) {
            records.push_back(i);
            mx = a[i];
        }
    }
    int ans = 0;
    if (records.empty()) {
        vector<int> pref(n + 1);
        pref[0] = 0;
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = add(pref[i], a[i]);
        }
        int sum_pref = 0;
        for (int i = 0; i < n; ++i) {
            sum_pref = add(sum_pref, pref[i + 1]);
        }
        ans = sum_pref;
    } else if (records.size() == 1) {
        vector<int> pref(n + 1);
        pref[0] = 0;
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = add(pref[i], a[i]);
        }
        int sum_pref = 0;
        for (int i = 0; i < n; ++i) {
            sum_pref = add(sum_pref, pref[i + 1]);
        }
        int p = records[0];
        for (int i = 0; i <= p; ++i) {
            int cur = sum_pref;
            cur = sub(cur, sub(pref[p], pref[i]));
            cur = add(cur, mul(a[p], p - i));
            ans = add(ans, cur);
            // cout << cur << endl;
        }
    } else {
        // int q = records[0];
        // int p = records[1];
        // for (int j = q; j >= 0; --j) {   
        // vector<int> pref(n + 1);
        // pref[0] = 0;
        // for (int i = 0; i < n; ++i) {
        //     pref[i + 1] = add(pref[i], a[i]);
        // }
        // int sum_pref = 0;
        // for (int i = 0; i < n; ++i) {
        //     sum_pref = add(sum_pref, pref[i + 1]);
        // }
        //     for (int i = 0; i <= p; ++i) {
        //         int cur = sum_pref;
        //         cur = sub(cur, sub(pref[p], pref[i]));
        //         cur = add(cur, mul(a[p], p - i));
        //         ans = add(ans, cur);
        //     }   
        //     if (j > 0) {
        //         swap(a[j], a[j - 1]);
        //     }
        // }
    }
    cout << ans << "\n";
    return 0;
}
