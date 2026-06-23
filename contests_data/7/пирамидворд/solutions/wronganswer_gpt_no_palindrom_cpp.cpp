#include <bits/stdc++.h>
using namespace std;

static const long long MOD = 1000000007LL;

struct PartInfo {
    int b;
    array<int, 5> loops{};
    array<vector<pair<int, int>>, 5> prev;
};

vector<array<int, 5>> generate_partitions() {
    vector<array<int, 5>> res;
    vector<vector<int>> blocks;

    function<void(int)> rec = [&](int v) {
        if (v == 5) {
            array<int, 5> cls{};
            for (int i = 0; i < (int)blocks.size(); ++i) {
                for (int x : blocks[i]) cls[x] = i;
            }
            res.push_back(cls);
            return;
        }

        for (auto &block : blocks) {
            block.push_back(v);
            rec(v + 1);
            block.pop_back();
        }

        blocks.push_back({v});
        rec(v + 1);
        blocks.pop_back();
    };

    rec(0);
    return res;
}

long long falling(long long x, int k) {
    if (k > x) return 0;
    long long res = 1;
    for (int i = 0; i < k; ++i) {
        res = res * ((x - i) % MOD) % MOD;
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, L;
    cin >> n >> L;

    vector<vector<long long>> cnt_diff(26, vector<long long>(26, 0));
    vector<long long> cnt_same(26, 0);

    // Предполагается, что палиндромов нет.
    for (int i = 0; i < n; ++i) {
        string s;
        cin >> s;
        int a = s.front() - 'a';
        int b = s.back() - 'a';

        if (a == b) {
            cnt_same[a]++;
        } else {
            if (a > b) swap(a, b);
            cnt_diff[a][b]++;
        }
    }

    long long diff[26][26][9]{};
    for (int i = 0; i < 26; ++i)
        for (int j = 0; j < 26; ++j)
            diff[i][j][0] = 1;

    for (int i = 0; i < 26; ++i) {
        for (int j = i + 1; j < 26; ++j) {
            for (int k = 1; k <= 8; ++k) {
                diff[i][j][k] = diff[j][i][k] = falling(cnt_diff[i][j], k);
            }
        }
    }

    long long same[26][9]{};
    for (int x = 0; x < 26; ++x) {
        same[x][0] = 1;
        for (int k = 1; k <= 8; ++k) {
            same[x][k] = falling(cnt_same[x], k) * ((1LL << k) % MOD) % MOD;
        }
    }

    vector<pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {0, 4}, {1, 4}, {2, 4}, {3, 4}
    };

    vector<PartInfo> parts;
    for (auto cls : generate_partitions()) {
        int b = *max_element(cls.begin(), cls.end()) + 1;
        vector<int> loops(b, 0);
        vector<vector<int>> mult(b, vector<int>(b, 0));
        vector<int> deg(b, 0);

        for (auto [u, v] : edges) {
            int a = cls[u], c = cls[v];
            if (a == c) {
                loops[a]++;
                deg[a] += 2;
            } else {
                mult[a][c]++;
                mult[c][a]++;
                deg[a]++;
                deg[c]++;
            }
        }

        vector<int> order(b);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int x, int y) {
            if (deg[x] != deg[y]) return deg[x] > deg[y];
            return loops[x] > loops[y];
        });

        PartInfo info;
        info.b = b;

        for (int i = 0; i < b; ++i) info.loops[i] = loops[order[i]];
        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < i; ++j) {
                int m = mult[order[j]][order[i]];
                if (m) info.prev[i].push_back({j, m});
            }
        }

        parts.push_back(info);
    }

    long long ans = 0;
    array<int, 5> color{};
    array<int, 26> used{};

    function<void(const PartInfo&, int, long long)> dfs =
        [&](const PartInfo &info, int pos, long long cur) {
            if (pos == info.b) {
                ans += cur;
                if (ans >= MOD) ans -= MOD;
                return;
            }

            for (int x = 0; x < 26; ++x) {
                if (used[x]) continue;

                long long ways = same[x][info.loops[pos]];
                if (ways == 0) continue;

                bool ok = true;
                for (auto [j, m] : info.prev[pos]) {
                    long long add = diff[x][color[j]][m];
                    if (add == 0) {
                        ok = false;
                        break;
                    }
                    ways = ways * add % MOD;
                }

                if (!ok) continue;

                used[x] = 1;
                color[pos] = x;
                dfs(info, pos + 1, cur * ways % MOD);
                used[x] = 0;
            }
        };

    for (const auto &info : parts) {
        dfs(info, 0, 1);
    }

    cout << ans % MOD << '\n';
    return 0;
}