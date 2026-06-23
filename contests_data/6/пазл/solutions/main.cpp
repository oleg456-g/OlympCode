#include <bits/stdc++.h>

using namespace std;
using pi = pair<int, int>;

const vector<vector<pi>> figures_shifts{
    {{0,0}, {0,1}, {1,0}, {1,1}},
    {{0,0}, {0,1}, {1,0}, {1,2}, {2,1}, {2,2}},
    {{0,0}, {0,1}, {1,-1}, {1,1}, {2,-1}, {2,0}}
};

bool CheckFigure(vector<pi> figure) {
    sort(figure.begin(), figure.end());
    for (const auto& standard_shifts : figures_shifts) {
        if (figure.size() != standard_shifts.size()) continue;

        bool ok = true;
        const auto [ax, ay] = figure[0];
        for (size_t i = 1; i != figure.size(); ++i) {
            const auto [bx, by] = figure[i];
            if (make_pair(bx-ax, by-ay) != standard_shifts[i])
                ok = false;
        }

        if (ok) return true;
    }
    return false;
}

vector<vector<int>> GenerateCoverage(int n, int m, vector<string> v) {
    vector<vector<int>> ans(n, vector<int>(m)), no_solution;

    vector<vector<int>> id(n, vector<int>(m, -1));
    vector<pi> pos;
    int vc = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (v[i][j] == '#') continue;
            id[i][j] = vc++;
            pos.emplace_back(i, j);
        }
    }

    if (!vc) return ans;

    vector<vector<int>> go(vc);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (v[i][j] == '#') continue;

            if (j+1 < m && v[i][j+1] != '#') {
                go[id[i][j]].emplace_back(id[i][j+1]);
                go[id[i][j+1]].emplace_back(id[i][j]);
                j = j+1;
            } else if (j+2 < m && v[i][j+2] != '#') {
                go[id[i][j]].emplace_back(id[i][j+2]);
                go[id[i][j+2]].emplace_back(id[i][j]);
                j = j+2;
            } else {
                return no_solution;
            }
        }
    }

    for (int j = 0; j < m; ++j) {
        for (int i = 0; i < n; ++i) {
            if (v[i][j] == '#') continue;

            if (i+1 < n && v[i+1][j] != '#') {
                go[id[i][j]].emplace_back(id[i+1][j]);
                go[id[i+1][j]].emplace_back(id[i][j]);
                i = i+1;
            } else if (i+2 < n && v[i+2][j] != '#') {
                go[id[i][j]].emplace_back(id[i+2][j]);
                go[id[i+2][j]].emplace_back(id[i][j]);
                i = i+2;
            } else {
                return no_solution;
            }
        }
    }

    for (int i = 0; i < vc; ++i) {
        if (go[i].size() != 2) return no_solution;
    }

    int fc = 0;
    for (int i = 0; i < vc; ++i) {
        auto [sx, sy] = pos[i];
        if (ans[sx][sy]) continue;

        vector<pi> figure; ++fc;
        for (int v = go[i][0], p = i; !ans[sx][sy]; ) {
            auto [cx, cy] = pos[v];
            figure.emplace_back(cx, cy);
            ans[cx][cy] = fc;
            swap(p ^= go[v][0] ^ go[v][1], v);
        }

        if (!CheckFigure(figure)) return no_solution;
    }

    return ans;
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);

    int t;
    cin >> t;
    while (t--) {
        int n, m; cin >> n >> m;
        vector<string> v(n);
        for (auto& s : v) cin >> s;
        auto ans = GenerateCoverage(n, m, v);
        if (ans.empty()) {
            cout << "No" << '\n';
        } else {
            cout << "Yes" << '\n';
            for (auto& w : ans) {
                for (int x : w) cout << x << ' ';
                cout << '\n';
            }
        }
    }
}