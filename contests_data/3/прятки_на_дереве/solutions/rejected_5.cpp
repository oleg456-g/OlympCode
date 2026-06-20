#include <iostream>
#include <vector>

using namespace std;

void dfs(int v, vector<int> &d, vector<vector<int>> &g) {
    d[v] = max(d[v], 1);
    for (int u : g[v]) {
        if (!d[u]) {
            d[u] = d[v] + 1;
            dfs(u, d, g);
        }
    }
}

int main() {
    int q, m;
    cin >> q >> m;
    vector<vector<int>> g(q);
    int c = 1;
    int answ = 0;
    for (int i = 0; i < q; ++i) {
        char t;
        int v;
        cin >> t >> v;
        if (!m) {
            v = (v - 1) % c + 1;
        } else {
            v = ((answ + (v - 1)) % c + c) % c + 1;
        }
        if (t == '+') {
            g[v - 1].push_back(c++);
            g[c - 1].push_back(v - 1);
        } else {
            vector<int> d(c, 0);
            dfs(v - 1, d, g);
            int mx = *max_element(d.begin(), d.end());
            int res = count(d.begin(), d.end(), mx);
            cout << mx - 1 << ' ' << res << '\n';
            answ = mx - 1 + res;
        }
    }
}
