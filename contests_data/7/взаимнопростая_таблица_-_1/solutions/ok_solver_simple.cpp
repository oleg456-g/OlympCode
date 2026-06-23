#include <bits/stdc++.h>
using namespace std;

int n;

void solve() {
    cin >> n;

    int m = n * n;

    vector<int> spf(m + 1);
    for (int i = 2; i <= m; ++i) if (!spf[i]) {
        for (int j = i; j <= m; j += i) if (!spf[j]) spf[j] = i;
    }

    vector<double> bad(m + 1);
    for (int x = 1; x <= m; ++x) {
        int y = x;
        double s = 0;
        while (y > 1) {
            int p = spf[y];
            while (y % p == 0) y /= p;
            if (p & 1) s += 1.0 / p;
        }
        bad[x] = s;
    }

    vector<pair<int, int>> c0, c1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if ((i + j) & 1) c1.push_back({i, j});
            else c0.push_back({i, j});
        }
    }

    vector<pair<int, int>> odd_cells = c0, even_cells = c1;
    if ((int)odd_cells.size() < (int)even_cells.size()) swap(odd_cells, even_cells);

    auto deg = [&](pair<int, int> p) {
        int i = p.first, j = p.second;
        return (i > 0) + (i + 1 < n) + (j > 0) + (j + 1 < n);
    };

    sort(odd_cells.begin(), odd_cells.end(), [&](pair<int, int> a, pair<int, int> b) {
        if (deg(a) != deg(b)) return deg(a) > deg(b);
        return a < b;
    });

    vector<int> odd, even;
    odd.reserve((m + 1) / 2);
    even.reserve(m / 2);
    for (int x = 1; x <= m; ++x) {
        if (x & 1) odd.push_back(x);
        else even.push_back(x);
    }

    sort(odd.begin(), odd.end(), [&](int a, int b) {
        if (bad[a] != bad[b]) return bad[a] < bad[b];
        return a < b;
    });
    sort(even.begin(), even.end(), [&](int a, int b) {
        if (bad[a] != bad[b]) return bad[a] > bad[b];
        return a > b;
    });

    vector<vector<int>> a(n, vector<int>(n));
    for (int i = 0; i < (int)odd_cells.size(); ++i) {
        a[odd_cells[i].first][odd_cells[i].second] = odd[i];
    }

    auto hard = [&](pair<int, int> p) {
        int i = p.first, j = p.second;
        double s = 0;
        if (i > 0) s += bad[a[i - 1][j]];
        if (i + 1 < n) s += bad[a[i + 1][j]];
        if (j > 0) s += bad[a[i][j - 1]];
        if (j + 1 < n) s += bad[a[i][j + 1]];
        return s;
    };

    sort(even_cells.begin(), even_cells.end(), [&](pair<int, int> p, pair<int, int> q) {
        if (hard(p) != hard(q)) return hard(p) > hard(q);
        return deg(p) > deg(q);
    });

    vector<int> avail = even;
    for (auto [i, j] : even_cells) {
        int pick = -1;
        for (int id = 0; id < (int)avail.size(); ++id) {
            int x = avail[id];
            if (i > 0 && gcd(x, a[i - 1][j]) != 1) continue;
            if (i + 1 < n && gcd(x, a[i + 1][j]) != 1) continue;
            if (j > 0 && gcd(x, a[i][j - 1]) != 1) continue;
            if (j + 1 < n && gcd(x, a[i][j + 1]) != 1) continue;
            pick = id;
            break;
        }
        if (pick == -1) pick = 0;
        a[i][j] = avail[pick];
        avail[pick] = avail.back();
        avail.pop_back();
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
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
