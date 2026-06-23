#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    if (n == 1) {
        cout << 1 << "\n";
        return 0;
    }

    vector<vector<int>> a(n, vector<int>(n, 0));

    auto layer_bad = [&](int s) {
        int bad = 0;
        for (int i = 0; i < s - 1; ++i) {
            bad += (gcd(a[s - 1][i], a[s - 2][i]) != 1);
            bad += (gcd(a[s - 1][i], a[s - 1][i + 1]) != 1);
            bad += (gcd(a[i][s - 1], a[i][s - 2]) != 1);
            bad += (gcd(a[i][s - 1], a[i + 1][s - 1]) != 1);
        }
        return bad;
    };

    a[0][0] = 3;
    a[0][1] = 4;
    a[1][0] = 2;
    a[1][1] = 1;

    for (int s = 3; s <= n; ++s) {
        vector<pair<int, int>> c0, c1;
        for (int j = 0; j < s; ++j) {
            if ((s - 1 + j) & 1) c1.push_back({s - 1, j});
            else c0.push_back({s - 1, j});
        }
        for (int i = 0; i < s - 1; ++i) {
            if ((i + s - 1) & 1) c1.push_back({i, s - 1});
            else c0.push_back({i, s - 1});
        }

        vector<int> odd, even;
        for (int x = (s - 1) * (s - 1) + 1; x <= s * s; ++x) {
            if (x & 1) odd.push_back(x);
            else even.push_back(x);
        }

        auto *odd_cells = &c0, *even_cells = &c1;
        if ((int)odd.size() != (int)c0.size()) {
            odd_cells = &c1;
            even_cells = &c0;
        }

        while (true) {
            shuffle(odd.begin(), odd.end(), rnd);
            shuffle(even.begin(), even.end(), rnd);
            for (int i = 0; i < (int)odd_cells->size(); ++i) a[(*odd_cells)[i].first][(*odd_cells)[i].second] = odd[i];
            for (int i = 0; i < (int)even_cells->size(); ++i) a[(*even_cells)[i].first][(*even_cells)[i].second] = even[i];

            int bad = layer_bad(s);
            while (bad > 0) {
                bool improved = false;
                for (int t = 0; t < 400; ++t) {
                    auto *cells = (rnd() & 1) ? odd_cells : even_cells;
                    if (cells->size() < 2) continue;
                    int p = rnd() % cells->size();
                    int q = rnd() % cells->size();
                    if (p == q) continue;

                    auto [i1, j1] = (*cells)[p];
                    auto [i2, j2] = (*cells)[q];
                    swap(a[i1][j1], a[i2][j2]);
                    int nb = layer_bad(s);
                    if (nb <= bad) {
                        if (nb < bad) improved = true;
                        bad = nb;
                        if (bad == 0) break;
                    } else {
                        swap(a[i1][j1], a[i2][j2]);
                    }
                }
                if (bad == 0) break;
                if (!improved) break;
            }

            if (layer_bad(s) == 0) break;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << a[i][j] << " \n"[j + 1 == n];
        }
    }
    return 0;
}
