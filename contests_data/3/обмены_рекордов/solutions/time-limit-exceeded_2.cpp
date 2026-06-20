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
    if (n > 10) {
        while (true) {}
    }
    queue<vector<int>> q;
    set<vector<int>> used;
    q.push(a);
    used.insert(a);
    int ans = 0;
    while (!q.empty()) {
        auto v = q.front();
        q.pop();
        for (int i = 0; i < n; ++i) {
            ans = add(ans, mul(v[i], n - i));
        } 
        int mx = 0;
        for (int i = 1; i < n; ++i) {
            mx = max(mx, v[i - 1]);
            if (mx < v[i]) {
                auto u = v;
                swap(u[i], u[i - 1]);
                if (!used.count(u)) {
                    used.insert(u);
                    q.push(u);
                }
            }
        }
    }
    cout << ans << endl;
    return 0;
}
