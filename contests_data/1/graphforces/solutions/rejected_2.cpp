#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<pair<int,int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; ++i) {
        int a, b;
        cin >> a >> b;
        edges.emplace_back(a, b);
    }

    // neighbours of each vertex
    vector<vector<int>> nbr(n);
    for (auto [a, b] : edges) {
        nbr[a].push_back(b);
        nbr[b].push_back(a);
    }

    // position of a neighbour inside nbr[v]
    vector<vector<int>> pos(n, vector<int>(n, -1));
    for (int v = 0; v < n; ++v) {
        for (int i = 0; i < (int)nbr[v].size(); ++i) {
            pos[v][nbr[v][i]] = i;
        }
    }

    vector<int> answer(n, n); // will be overwritten

    for (int v = 0; v < n; ++v) {
        int deg = (int)nbr[v].size();
        int totalMask = 1 << deg;
        int best = n; // maximal possible components
        for (int mask = 0; mask < totalMask; ++mask) {
            int adj[15] = {0}; // n ≤ 15
            // build graph for this mask
            for (auto [a, b] : edges) {
                if (a == v || b == v) {
                    int other = (a == v) ? b : a;
                    int idx = pos[v][other];
                    bool plus = (mask >> idx) & 1;
                    int x = (other + (plus ? 1 : -1) + n) % n;
                    adj[v] |= (1 << x);
                    adj[x] |= (1 << v);
                } else {
                    adj[a] |= (1 << b);
                    adj[b] |= (1 << a);
                }
            }

            // count components
            int visited = 0;
            int comps = 0;
            for (int i = 0; i < n; ++i) {
                if (!(visited & (1 << i))) {
                    ++comps;
                    queue<int> q;
                    q.push(i);
                    visited |= (1 << i);
                    while (!q.empty()) {
                        int cur = q.front(); q.pop();
                        int nbMask = adj[cur];
                        while (nbMask) {
                            int nxt = __builtin_ctz(nbMask);
                            nbMask &= nbMask - 1;
                            if (!(visited & (1 << nxt))) {
                                visited |= (1 << nxt);
                                q.push(nxt);
                            }
                        }
                    }
                }
            }
            if (comps < best) best = comps;
            if (best == 1) break; // cannot be better
        }
        answer[v] = best;
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << answer[i];
    }
    cout << '\n';
    return 0;
}
