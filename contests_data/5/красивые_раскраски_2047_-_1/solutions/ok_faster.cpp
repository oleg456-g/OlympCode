#include <iostream>
#include <vector>

using namespace std;

int ans = 0;
int k;
const int mod = 1e9 + 7;
int curr = 0;

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

void dfs(int v, vector<vector<int>> &g, vector<int> &used) {
    used[v] = curr;
    for (int u : g[v]) {
        if (!used[u]) {
            dfs(u, g, used);
        }
    } 
}

vector<vector<vector<int>>> components(vector<vector<int>> &g) {
    curr = 1;
    const int n = g.size();
    vector<int> used(n, 0);
    for (int i = 0; i < n; ++i) {
        if (!used[i]) {
            dfs(i, g, used);
            ++curr;
        }
    }
    vector<int> ord(n);
    vector<int> cnt(curr, 0);
    for (int i = 0; i < n; ++i) {
        ord[i] = cnt[used[i] - 1];
        ++cnt[used[i] - 1];
    }
    vector<vector<vector<int>>> gs(curr);
    for (int i = 0; i < curr; ++i) {
        gs[i].resize(cnt[i]);
    }
    for (int i = 0; i < n; ++i) {
        int c = used[i] - 1;
        for (int u : g[i]) {
            gs[c][ord[i]].push_back(ord[u]);
            gs[c][ord[u]].push_back(ord[i]);
        }
    }
    return gs;
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
    vector<vector<vector<int>>> c = components(g);
    vector<int> used(n, 0);
    int answ = 1;
    for (auto g : c) {
        ans = 0;
        count(0, g, used);
        answ = (answ * 1ll * ans) % mod;
    }
    cout << answ << '\n';
}

int main() {
    ios_base::sync_with_stdio(false);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
}