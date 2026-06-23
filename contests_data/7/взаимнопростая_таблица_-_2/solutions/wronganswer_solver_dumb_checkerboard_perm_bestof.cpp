#include <bits/stdc++.h>
using namespace std;

mt19937 rnd((unsigned)chrono::steady_clock::now().time_since_epoch().count());

int conflicts(const vector<int>& a, int n) {
    int bad = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int id = i * n + j;
            if (i + 1 < n && gcd(a[id], a[id + n]) != 1) ++bad;
            if (j + 1 < n && gcd(a[id], a[id + 1]) != 1) ++bad;
        }
    }
    return bad;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    int m = n * n;

    vector<int> c0, c1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int id = i * n + j;
            if ((i + j) & 1) c1.push_back(id);
            else c0.push_back(id);
        }
    }

    vector<int> odd_cells = c0, even_cells = c1;
    if ((int)odd_cells.size() < (int)even_cells.size()) swap(odd_cells, even_cells);

    vector<int> odd, even;
    for (int x = 1; x <= m; ++x) {
        if (x & 1) odd.push_back(x);
        else even.push_back(x);
    }

    vector<int> a(m), best(m);
    int best_bad = (int)1e9;
    const int RETRIES = 1000;

    for (int t = 0; t < RETRIES; ++t) {
        shuffle(odd.begin(), odd.end(), rnd);
        shuffle(even.begin(), even.end(), rnd);

        for (int i = 0; i < (int)odd_cells.size(); ++i) a[odd_cells[i]] = odd[i];
        for (int i = 0; i < (int)even_cells.size(); ++i) a[even_cells[i]] = even[i];

        int bad = conflicts(a, n);
        if (bad < best_bad) {
            best_bad = bad;
            best = a;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << best[i * n + j] << " \n"[j + 1 == n];
        }
    }
    return 0;
}
