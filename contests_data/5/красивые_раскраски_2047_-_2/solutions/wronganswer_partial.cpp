#include <iostream>
#include <vector>
#include <cassert>
#include <deque>

using namespace std;

int ans = 0;
int k;
const int mod = 1e9 + 7;
int curr = 0;
int total = 0;

vector<int> find_sep(vector<vector<int>> &g) {
    const int n = g.size();
    vector<int> opt;
    int cost = mod;
    for (int i = 0; i < n; ++i) {
        deque<int> q{i};
        vector<int> dist(n, n);
        dist[i] = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop_front();
            for (int u : g[v]) {
                if (dist[u] > dist[v] + 1) {
                    dist[u] = dist[v] + 1;
                    q.push_back(u);
                }
            }
        }
        vector<vector<int>> layers(n);
        for (int i = 0; i < n; ++i) {
            layers[dist[i]].push_back(i);
        }
        int sm = 0;
        for (int i = 0; i + 1 < n; ++i) {
            sm += layers[i].size();
            if (sm + layers[i + 1].size() >= n / 2) {
                int s1 = sm;
                int s2 = sm + layers[i + 1].size();
                if (n - s1 - n / 2 <= n / 2 - (n - s2)) {
                    int curr = max(s1, (int)(n - s1 + layers[i].size()));
                    if (cost > curr) {
                        cost = curr;
                        opt = layers[i];
                    }
                    break;
                } else {
                    int curr = max((int)(s1 + layers[i + 1].size()), n - s1);
                    if (cost > curr) {
                        cost = curr;
                        opt = layers[i + 1];
                    }
                    break;
                }
            }
        }
    }
    assert(cost != mod);
    if (cost > 15) {
        total = -1;
    }
    cerr << cost << endl;
    return opt;
}

pair<pair<vector<vector<int>>, vector<vector<int>>>, vector<int>> separate(vector<vector<int>> &g, vector<int> &sep) {
    const int n = g.size();
    vector<bool> used(n, false);
    vector<bool> in_sep(n, false);
    deque<int> q;
    for (int i : sep) {
        used[i] = true;
        in_sep[i] = true;
    }
    int j = -1;
    for (int i = 0; i < n; ++i) {
        if (!used[i]) {
            j = i;
            break;
        }
    }
    if (j != -1) {
        q.push_back(j);
        while (!q.empty()) {
            int v = q.front();
            q.pop_front();
            for (int u : g[v]) {
                if (!used[u]) {
                    used[u] = true;
                    q.push_back(u);
                }
            }
        }
    }
    vector<int> left, right;
    for (int i : sep) {
        right.push_back(i);
    }
    for (int i : sep) {
        left.push_back(i);
    }
    for (int i = 0; i < n; ++i) {
        if (in_sep[i]) continue;
        if (used[i]) {
            left.push_back(i);
        } else {
            right.push_back(i);
        }
    }
    vector<int> ord(n);
    for (int i = 0; i < left.size(); ++i) {
        ord[left[i]] = i;
    }
    for (int i = 0; i < right.size(); ++i) {
        ord[right[i]] = i;
    }
    vector<vector<vector<int>>> gs(2);
    gs[0].resize(right.size());
    gs[1].resize(left.size());
    for (int i = 0; i < n; ++i) {
        if (in_sep[i]) {
            continue;
        }
        int c = used[i];
        for (int u : g[i]) {
            gs[c][ord[i]].push_back(ord[u]);
        }
    }
    for (int i : sep) {
        for (int j : g[i]) {
            if (in_sep[j]) {
                gs[0][ord[i]].push_back(ord[j]);
                gs[1][ord[i]].push_back(ord[j]);
            } else {
                gs[used[j]][ord[i]].push_back(ord[j]);
            }
        }
    }
    cerr << left.size() << ' ' << right.size() << '\n';
    return make_pair(make_pair(gs[0], gs[1]), ord);
}

void count(int v, vector<vector<int>> &g, vector<int> &used) {
    if (v == g.size()) {
        ++ans;
        return;
    }
    if (used[v]) {
        count(v + 1, g, used);
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

void calc(vector<vector<int>> &g, vector<int> &used) {
    ans = 0;
    count(0, g, used);
}

void work(int v, vector<int> &used, vector<vector<int>> &g1, vector<vector<int>> &g2, vector<vector<int>> &g, vector<int> &ord, vector<int> &sep) {
    if (v < 0) {
        vector<int> used1(g1.size(), 0);
        for (int i : sep) {
            used1[ord[i]] = used[i];
        }
        calc(g1, used1);
        int mem = ans;
        vector<int> used2(g2.size(), 0);
        for (int i : sep) {
            used2[ord[i]] = used[i];
        }
        calc(g2, used2);
        mem = (mem * 1ll * ans) % mod;
        total += mem;
        total %= mod;
        return;
    }
    vector<bool> available(k + 1, true);
    for (int u : g[sep[v]]) {
        available[used[u]] = false;
    }
    for (int i = 1; i <= k; ++i) {
        if (available[i]) {
            used[sep[v]] = i;
            work(v - 1, used, g1, g2, g, ord, sep);
            used[sep[v]] = 0;
        }
    }
}

int solve(vector<vector<int>> &g) {
    total = 0;
    const int n = g.size();
    vector<int> sep = find_sep(g);
    if (total == -1) {
        return 0;
    }
    auto [gs, ord] = separate(g, sep);
    auto [g1, g2] = gs;
    vector<int> used(n, 0);
    work(sep.size() - 1, used, g1, g2, g, ord, sep);
    return total;
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
    vector<vector<vector<int>>> gs(curr - 1);
    for (int i = 0; i + 1 < curr; ++i) {
        gs[i].resize(cnt[i]);
    }
    for (int i = 0; i < n; ++i) {
        int c = used[i] - 1;
        for (int u : g[i]) {
            gs[c][ord[i]].push_back(ord[u]);
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
        int ans = solve(g);
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
