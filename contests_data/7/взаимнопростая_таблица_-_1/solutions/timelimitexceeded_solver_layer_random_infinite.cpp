#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

void solve() {
    int n;
    cin >> n;

    vector<vector<int>> a(n, vector<int>(n, 0));

    auto layer_bad = [&](int s) {
        int bad = 0;
        for (int i = 0; i < s - 1; ++i) {
            if (gcd(a[s - 1][i], a[s - 2][i]) != 1)
                bad++;
            
            if (gcd(a[s - 1][i], a[s - 1][i + 1]) != 1)
                bad++;
            
            if (gcd(a[i][s - 1], a[i][s - 2]) != 1)
                bad++;

            
            if (gcd(a[i][s - 1], a[i + 1][s - 1]) != 1)
                bad++;
        }
        return bad;
    };

    a[0][0] = 3;
    a[0][1] = 4;
    a[1][0] = 2;
    a[1][1] = 1;

    for (int s = 3; s <= n; ++s) {
        vector<pair<int, int>> odd_cells, even_cells;
        for (int j = 0; j < s; ++j) {
            if ((s - 1 + j) % 2 == 0) odd_cells.push_back({s - 1, j});
            else even_cells.push_back({s - 1, j});
        }
        for (int i = 0; i < s - 1; ++i) {
            if ((i + s - 1) % 2 == 0) odd_cells.push_back({i, s - 1});
            else even_cells.push_back({i, s - 1});
        }

        vector<int> odd_nums, even_nums;
        for (int x = (s - 1) * (s - 1) + 1; x <= s * s; ++x) {
            if (x % 2 == 1) odd_nums.push_back(x);
            else even_nums.push_back(x);
        }

        while (true) {
            shuffle(odd_nums.begin(), odd_nums.end(), rnd);
            shuffle(even_nums.begin(), even_nums.end(), rnd);
            for (int i = 0; i < (int)odd_cells.size(); ++i) a[odd_cells[i].first][odd_cells[i].second] = odd_nums[i];
            for (int i = 0; i < (int)even_cells.size(); ++i) a[even_cells[i].first][even_cells[i].second] = even_nums[i];
            if (layer_bad(s) == 0) break;
        }
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
