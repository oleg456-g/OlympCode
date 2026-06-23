#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    auto start_time = chrono::steady_clock::now();
    int m = n * n;

    vector<pair<int, int>> color0, color1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if ((i + j) & 1) color1.push_back({i, j});
            else color0.push_back({i, j});
        }
    }

    vector<pair<int, int>> odd_cells = color0;
    vector<pair<int, int>> even_cells = color1;
    if ((int)odd_cells.size() < (int)even_cells.size()) {
        swap(odd_cells, even_cells);
    }

    vector<int> odd_values, even_values;
    for (int x = 1; x <= m; ++x) {
        if (x & 1) odd_values.push_back(x);
        else even_values.push_back(x);
    }

    vector<vector<int>> board(n, vector<int>(n));
    vector<vector<int>> best_board;
    int best_conflicts = (int)1e9;

    auto count_conflicts = [&](const vector<vector<int>>& a) {
        int bad = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i + 1 < n && gcd(a[i][j], a[i + 1][j]) != 1) ++bad;
                if (j + 1 < n && gcd(a[i][j], a[i][j + 1]) != 1) ++bad;
            }
        }
        return bad;
    };

    auto cell_conflicts = [&](int i, int j) {
        int bad = 0;
        if (i > 0) bad += (gcd(board[i][j], board[i - 1][j]) != 1);
        if (i + 1 < n) bad += (gcd(board[i][j], board[i + 1][j]) != 1);
        if (j > 0) bad += (gcd(board[i][j], board[i][j - 1]) != 1);
        if (j + 1 < n) bad += (gcd(board[i][j], board[i][j + 1]) != 1);
        return bad;
    };

    auto random_fill = [&]() {
        vector<int> odd = odd_values;
        vector<int> even = even_values;
        shuffle(odd.begin(), odd.end(), rnd);
        shuffle(even.begin(), even.end(), rnd);

        for (int k = 0; k < (int)odd_cells.size(); ++k) {
            int i = odd_cells[k].first;
            int j = odd_cells[k].second;
            board[i][j] = odd[k];
        }
        for (int k = 0; k < (int)even_cells.size(); ++k) {
            int i = even_cells[k].first;
            int j = even_cells[k].second;
            board[i][j] = even[k];
        }
    };


    while (chrono::duration<double>(chrono::steady_clock::now() - start_time).count() < 1.9) {
        random_fill();
        int cur_conflicts = count_conflicts(board);

        for (int step = 0; step < n * n * 60 && cur_conflicts > 0; ++step) {
            vector<pair<int, int>>& cells = (rnd() & 1) ? odd_cells : even_cells;

            int p = rnd() % cells.size();
            int q = rnd() % cells.size();
            if (p == q) continue;

            int i1 = cells[p].first;
            int j1 = cells[p].second;
            int i2 = cells[q].first;
            int j2 = cells[q].second;

            int before = cell_conflicts(i1, j1) + cell_conflicts(i2, j2);
            if (abs(i1 - i2) + abs(j1 - j2) == 1 && gcd(board[i1][j1], board[i2][j2]) != 1) {
                --before;
            }

            swap(board[i1][j1], board[i2][j2]);

            int after = cell_conflicts(i1, j1) + cell_conflicts(i2, j2);
            if (abs(i1 - i2) + abs(j1 - j2) == 1 && gcd(board[i1][j1], board[i2][j2]) != 1) {
                --after;
            }

            if (after <= before) {
                cur_conflicts += after - before;
            } else {
                swap(board[i1][j1], board[i2][j2]);
            }
        }

        if (cur_conflicts < best_conflicts) {
            best_conflicts = cur_conflicts;
            best_board = board;
            if (best_conflicts == 0) break;
        }
    }

    if (best_board.empty()) best_board = board;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << best_board[i][j] << " \n"[j + 1 == n];
        }
    }

    return 0;
}
