#include <bits/stdc++.h>

using namespace std;

const int md = 1e9 + 7;

void add(int &a, int b) {
    a += b;
    if (a >= md) {
        a -= md;
    }
}

int mul(int a, int b) {
    return a * 1ll * b % md;
}

int inv(int a) {
    int n = md - 2;
    int ret = 1;
    while (n > 0) {
        if (n & 1) {
            ret = mul(ret, a);
        }
        a = mul(a, a);
        n >>= 1;
    }
    return ret;
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
    vector<int> x = {a[0]}, y;
    int mx = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] > mx) {
            y.push_back(a[i]);
            mx = a[i];
        } else {
            x.push_back(a[i]);
        }
    }
    int nx = x.size();
    int ny = y.size();
    int av = 0;
    for (int t : y) {
        add(av, t);
    }
    if (!y.empty()) {
        av = mul(av, inv(ny));
    }
    vector<int> prefx(nx + 1);
    prefx[0] = 0;
    for (int i = 0; i < nx; ++i) {
        add(prefx[i + 1], x[i]);
        add(prefx[i + 1], prefx[i]);
    }
    vector<vector<int>> dp_cnt(nx + 1, vector<int>(ny + 1, 0));
    vector<vector<int>> dp_sum(nx + 1, vector<int>(ny + 1, 0));
    dp_cnt[0][0] = 1;
    for (int i = 0; i <= nx; ++i) {
        for (int j = 0; j <= ny; ++j) {
            int sum_here = 0;
            add(sum_here, prefx[i]);
            add(sum_here, mul(j, av));
            add(dp_sum[i][j], mul(dp_cnt[i][j], sum_here));
            if (i < nx) {
                add(dp_cnt[i + 1][j], dp_cnt[i][j]);
                add(dp_sum[i + 1][j], dp_sum[i][j]);
            }
            if (j < ny) {
                add(dp_cnt[i][j + 1], dp_cnt[i][j]);
                add(dp_sum[i][j + 1], dp_sum[i][j]);
            }
        }
    }
    int ans = dp_sum[nx][ny];
    for (int i = 1; i <= ny; ++i) {
        ans = mul(ans, i);
    }
    cout << ans << endl;
    return 0;
}
