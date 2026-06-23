#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

void solve() {
    int n;
    cin >> n;

    int m = n * n;
    uniform_int_distribution<int> dist(1, m);

    vector<vector<int>> a(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = dist(rnd);
        }
    }

    if (n >= 2) a[0][1] = a[0][0];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << a[i][j] << " \n"[j + 1 == n];
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--)
        solve();

    return 0;
}
