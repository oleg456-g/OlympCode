#include <bits/stdc++.h>
using namespace std;

mt19937 rnd(52);

void solve() {
    int n;
    cin >> n;
    int m = n * n;

    vector<int> c0, c1;
    c0.reserve((m + 1) / 2);
    c1.reserve(m / 2);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int id = i * n + j;
            if ((i + j) & 1) c1.push_back(id);
            else c0.push_back(id);
        }
    }

    vector<int> odd_cells = c0, even_cells = c1;
    if ((int)odd_cells.size() < (int)even_cells.size()) swap(odd_cells, even_cells);

    vector<int> is_odd_cell(m, 0);
    for (int id : odd_cells) is_odd_cell[id] = 1;

    vector<int> odd_values, even_values;
    odd_values.reserve((m + 1) / 2);
    even_values.reserve(m / 2);
    for (int x = 1; x <= m; ++x) {
        if (x & 1) odd_values.push_back(x);
        else even_values.push_back(x);
    }

    vector<array<int, 4>> nei(m);
    vector<int> deg(m, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int id = i * n + j;
            if (i > 0) nei[id][deg[id]++] = id - n;
            if (i + 1 < n) nei[id][deg[id]++] = id + n;
            if (j > 0) nei[id][deg[id]++] = id - 1;
            if (j + 1 < n) nei[id][deg[id]++] = id + 1;
        }
    }

    vector<int> val(m), best_val(m);
    vector<int> cell_bad(m), conf_pos(m), conf_list, tabu_until(m, 0);

    auto refresh_conf = [&](int x) {
        if (cell_bad[x] > 0) {
            if (conf_pos[x] == -1) {
                conf_pos[x] = (int)conf_list.size();
                conf_list.push_back(x);
            }
        } else {
            if (conf_pos[x] != -1) {
                int p = conf_pos[x];
                int y = conf_list.back();
                conf_list[p] = y;
                conf_pos[y] = p;
                conf_list.pop_back();
                conf_pos[x] = -1;
            }
        }
    };

    auto random_init = [&]() {
        shuffle(odd_values.begin(), odd_values.end(), rnd);
        shuffle(even_values.begin(), even_values.end(), rnd);
        for (int i = 0; i < (int)odd_cells.size(); ++i) val[odd_cells[i]] = odd_values[i];
        for (int i = 0; i < (int)even_cells.size(); ++i) val[even_cells[i]] = even_values[i];
    };

    int total_bad = 0;
    auto rebuild = [&]() {
        total_bad = 0;
        fill(cell_bad.begin(), cell_bad.end(), 0);
        conf_list.clear();
        fill(conf_pos.begin(), conf_pos.end(), -1);

        for (int u = 0; u < m; ++u) {
            for (int k = 0; k < deg[u]; ++k) {
                int v = nei[u][k];
                if (u < v && gcd(val[u], val[v]) != 1) {
                    ++total_bad;
                    ++cell_bad[u];
                    ++cell_bad[v];
                }
            }
        }

        for (int u = 0; u < m; ++u) {
            if (cell_bad[u] > 0) {
                conf_pos[u] = (int)conf_list.size();
                conf_list.push_back(u);
            }
        }
    };

    auto delta_swap = [&](int u, int v) {
        int xu = val[u], xv = val[v];
        int delta = 0;

        for (int k = 0; k < deg[u]; ++k) {
            int w = nei[u][k];
            delta += (gcd(xv, val[w]) != 1) - (gcd(xu, val[w]) != 1);
        }
        for (int k = 0; k < deg[v]; ++k) {
            int w = nei[v][k];
            delta += (gcd(xu, val[w]) != 1) - (gcd(xv, val[w]) != 1);
        }

        return delta;
    };

    auto apply_swap = [&](int u, int v) {
        int xu = val[u], xv = val[v];

        int touched[10], tc = 0;
        auto add_touched = [&](int x) {
            for (int i = 0; i < tc; ++i) if (touched[i] == x) return;
            touched[tc++] = x;
        };

        add_touched(u);
        add_touched(v);

        for (int k = 0; k < deg[u]; ++k) {
            int w = nei[u][k];
            int old_bad = (gcd(xu, val[w]) != 1);
            int new_bad = (gcd(xv, val[w]) != 1);
            int diff = new_bad - old_bad;
            if (diff) {
                total_bad += diff;
                cell_bad[u] += diff;
                cell_bad[w] += diff;
            }
            add_touched(w);
        }

        for (int k = 0; k < deg[v]; ++k) {
            int w = nei[v][k];
            int old_bad = (gcd(xv, val[w]) != 1);
            int new_bad = (gcd(xu, val[w]) != 1);
            int diff = new_bad - old_bad;
            if (diff) {
                total_bad += diff;
                cell_bad[v] += diff;
                cell_bad[w] += diff;
            }
            add_touched(w);
        }

        swap(val[u], val[v]);
        for (int i = 0; i < tc; ++i) refresh_conf(touched[i]);
    };

    auto check = [&](const vector<int>& cur) {
        for (int u = 0; u < m; ++u) {
            for (int k = 0; k < deg[u]; ++k) {
                int v = nei[u][k];
                if (u < v && gcd(cur[u], cur[v]) != 1) return false;
            }
        }
        return true;
    };

    int best_bad = (int)1e9;
    auto save_best = [&]() {
        if (total_bad < best_bad) {
            best_bad = total_bad;
            best_val = val;
        }
    };

    const double time_limit = 100.0;
    auto start = chrono::steady_clock::now();

    int base_tabu = max(7, n / 3);
    int extra_tabu = max(7, n / 3);
    int samples = (n <= 120 ? 96 : 48);
    int stagnation_limit = max(2000, n * n);

    while (chrono::duration<double>(chrono::steady_clock::now() - start).count() < time_limit) {
        random_init();
        rebuild();
        fill(tabu_until.begin(), tabu_until.end(), 0);
        save_best();
        if (best_bad == 0) break;

        int iter = 1;
        int no_improve = 0;
        int local_best = total_bad;

        while (total_bad > 0 && no_improve < stagnation_limit) {
            if (conf_list.empty()) break;
            if ((iter & 255) == 0 && chrono::duration<double>(chrono::steady_clock::now() - start).count() >= time_limit) break;

            int u = -1;
            for (int t = 0; t < 12; ++t) {
                int cand = conf_list[rnd() % conf_list.size()];
                if (tabu_until[cand] <= iter) {
                    u = cand;
                    break;
                }
            }
            if (u == -1) u = conf_list[rnd() % conf_list.size()];

            vector<int>& group = is_odd_cell[u] ? odd_cells : even_cells;
            int best_v = -1;
            int best_delta = (int)1e9;

            for (int s = 0; s < samples; ++s) {
                int v = group[rnd() % group.size()];
                if (v == u) continue;
                int delta = delta_swap(u, v);
                bool tabu = (tabu_until[u] > iter || tabu_until[v] > iter);
                if (tabu && total_bad + delta >= best_bad) continue;
                if (delta < best_delta || (delta == best_delta && (rnd() & 1))) {
                    best_delta = delta;
                    best_v = v;
                }
            }

            if (best_v == -1) {
                do best_v = group[rnd() % group.size()]; while (best_v == u);
            }

            apply_swap(u, best_v);

            ++iter;
            int ten = base_tabu + (rnd() % extra_tabu);
            tabu_until[u] = iter + ten;
            tabu_until[best_v] = iter + ten;

            if (total_bad < local_best) {
                local_best = total_bad;
                no_improve = 0;
            } else {
                ++no_improve;
            }

            if (total_bad < best_bad) {
                save_best();
                if (best_bad == 0) break;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << best_val[i * n + j] << " \n"[j + 1 == n];
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
