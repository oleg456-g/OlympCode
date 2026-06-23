#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

void solve() {
    int n;
    cin >> n;
    int m = n * n;

    vector<int> p(m);
    iota(p.begin(), p.end(), 1);
    shuffle(p.begin(), p.end(), rnd);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << p[i * n + j] << " \n"[j + 1 == n];
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
