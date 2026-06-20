#include <iostream>
#include <vector>

using namespace std;

int ans = 0;
int k;
const int mod = 1e9 + 7;

void count(int v, vector<vector<int>> &g, vector<int> &used) {
    if (v == g.size()) {
        ++ans;
        return;
    }
    vector<bool> available(k + 1, true);
    for (int u : g[v]) {
        available[used[u]] = false;
    }
    for (int i = 1; i <= k; ++i) {
        if (available[i]) {
            used[v] = i;
            count(v + 1, g, used);
            used[v] = 0;
        }
    }
}

void solve() {
    int n, m;
    cin >> n >> m >> k;
    vector<vector<int>> g(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        g[u].push_back(v);
        g[v].push_back(u);
    }
    vector<int> used(n, 0);
    ans = 0;
    count(0, g, used);
    cout << ans << '\n';
}

int main() {
    ios_base::sync_with_stdio(false);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}