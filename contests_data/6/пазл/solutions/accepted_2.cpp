// Может показаться, что это лажа, но это решение доказуемо работает.
// Важный нюанс состоит в том, чтобы проверять фигурки в нужном порядке.

#include <bits/stdc++.h>

using namespace std;
using pi = pair<int, int>;

const vector<vector<pi>> figures_shifts{
    {{0,0}, {0,1}, {1,0}, {1,1}},
    {{0,0}, {0,1}, {1,0}, {1,2}, {2,1}, {2,2}},
    {{0,0}, {0,1}, {1,-1}, {1,1}, {2,-1}, {2,0}}
};

void solve() {
    int n, m; cin >> n >> m;
    vector<string> v(n); for (auto& w : v) cin >> w;
    vector<vector<int>> ans(n, vector<int>(m));
    int fc = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (v[i][j] == '#') continue;
            
            for (int t = 0; t < 3; ++t) {
                bool ok = true;
                for (auto [dx, dy] : figures_shifts[t]) {
                    if (i+dx < 0 || i+dx >= n || j+dy < 0 || j+dy >= m) { ok = false; break; }
                    if (v[i+dx][j+dy] == '#') { ok = false; break; }
                }
                if (ok) {
                    ++fc;
                    for (auto [dx, dy] : figures_shifts[t]) v[i+dx][j+dy] = '#', ans[i+dx][j+dy] = fc;
                }
            }

            if (v[i][j] == '.') {
                cout << "No" << '\n';
                return;
            }
        }
    }

    cout << "Yes" << '\n';
    for (auto& w : ans) {
        for (int x : w) cout << x << ' ';
        cout << '\n';
    }
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    int t = 1;
    cin >> t;
    while (t--) solve();
}