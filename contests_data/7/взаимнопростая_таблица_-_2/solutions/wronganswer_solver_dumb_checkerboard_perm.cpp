#include <bits/stdc++.h>
using namespace std;

mt19937 rnd((unsigned)chrono::steady_clock::now().time_since_epoch().count());

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    int m = n * n;

    vector<pair<int, int>> c0, c1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if ((i + j) & 1) c1.push_back({i, j});
            else c0.push_back({i, j});
        }
    }

    vector<pair<int, int>> odd_cells = c0, even_cells = c1;
    if ((int)odd_cells.size() < (int)even_cells.size()) swap(odd_cells, even_cells);

    vector<int> odd, even;
    for (int x = 1; x <= m; ++x) {
        if (x & 1) odd.push_back(x);
        else even.push_back(x);
    }
    shuffle(odd.begin(), odd.end(), rnd);
    shuffle(even.begin(), even.end(), rnd);

    vector<vector<int>> a(n, vector<int>(n));
    for (int i = 0; i < (int)odd_cells.size(); ++i) a[odd_cells[i].first][odd_cells[i].second] = odd[i];
    for (int i = 0; i < (int)even_cells.size(); ++i) a[even_cells[i].first][even_cells[i].second] = even[i];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << a[i][j] << " \n"[j + 1 == n];
        }
    }
    return 0;
}
