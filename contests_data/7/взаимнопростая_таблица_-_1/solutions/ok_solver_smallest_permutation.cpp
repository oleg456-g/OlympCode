#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n;
    cin >> n;

    int x = 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << x++ << " \n"[j + 1 == n];
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
