#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

int conflicts(const vector<int>& p, int n) {
    int bad = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int id = i * n + j;
            if (i + 1 < n && gcd(p[id], p[id + n]) != 1) ++bad;
            if (j + 1 < n && gcd(p[id], p[id + 1]) != 1) ++bad;
        }
    }
    return bad;
}

void solve() {
    int n;
    cin >> n;
    int m = n * n;

    vector<int> p(m), best(m);
    iota(p.begin(), p.end(), 1);

    int best_bad = (int)1e9;
    const int RETRIES = 1000;

    for (int t = 0; t < RETRIES; ++t) {
        shuffle(p.begin(), p.end(), rnd);
        int bad = conflicts(p, n);
        if (bad < best_bad) {
            best_bad = bad;
            best = p;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << best[i * n + j] << " \n"[j + 1 == n];
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
