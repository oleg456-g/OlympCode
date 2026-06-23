#include <bits/stdc++.h>
using namespace std;

static const long long MOD = 1000000007LL;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, L;
    cin >> n >> L;

    vector<string> words(n);
    for (int i = 0; i < n; ++i) cin >> words[i];

    if (n < 8) {
        cout << 0 << '\n';
        return 0;
    }

    // Ребра в порядке с хорошим ранним отсечением:
    // AB, AT, BT, BC, CT, CD, DT, DA
    vector<pair<int, int>> order = {
        {0, 1}, {0, 4}, {1, 4}, {1, 2},
        {2, 4}, {2, 3}, {3, 4}, {3, 0}
    };

    vector<vector<string>> opts(n);
    for (int i = 0; i < n; ++i) {
        opts[i].push_back(words[i]);
        string r = words[i];
        reverse(r.begin(), r.end());
        if (r != words[i]) opts[i].push_back(r);
    }

    vector<int> used(n, 0);
    array<int, 5> vertex;
    vertex.fill(-1);

    long long ans = 0;

    function<void(int)> dfs = [&](int pos) {
        if (pos == 8) {
            ans++;
            if (ans >= MOD) ans -= MOD;
            return;
        }

        auto [u, v] = order[pos];

        for (int i = 0; i < n; ++i) {
            if (used[i]) continue;

            for (const string &s : opts[i]) {
                int a = s.front() - 'a';
                int b = s.back() - 'a';

                if (vertex[u] != -1 && vertex[u] != a) continue;
                if (vertex[v] != -1 && vertex[v] != b) continue;

                int old_u = vertex[u];
                int old_v = vertex[v];
                vertex[u] = a;
                vertex[v] = b;
                used[i] = 1;

                dfs(pos + 1);

                used[i] = 0;
                vertex[u] = old_u;
                vertex[v] = old_v;
            }
        }
    };

    dfs(0);
    cout << ans % MOD << '\n';
    return 0;
}