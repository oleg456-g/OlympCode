#include <bits/stdc++.h>
using namespace std;

constexpr int LOG = 20;                 // 2^20 > 10^6

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;

    /* ---------- read edges and build adjacency list ---------- */
    vector<pair<int,int>> edges(m);
    vector<int> deg(n, 0);
    for (int i = 0; i < m; ++i) {
        int v, u;  cin >> v >> u;
        edges[i] = {v, u};
        ++deg[v]; ++deg[u];
    }
    vector<vector<int>> adj(n);
    for (int i = 0; i < n; ++i) adj[i].reserve(deg[i]);
    for (auto [v,u] : edges) {
        adj[v].push_back(u);
        adj[u].push_back(v);
    }

    /* ---------- iterative DFS – disc, low, parent, depth, tin, tout ---------- */
    vector<int> disc(n, -1), low(n), parent(n, -1), depth(n);
    vector<int> tin(n), tout(n);
    vector<int> it(n, 0);
    int timer = 0, timer2 = 0;
    vector<int> st;
    st.reserve(n);
    st.push_back(0);
    while (!st.empty()) {
        int v = st.back();
        if (disc[v] == -1) {                     // first time we see v
            disc[v] = low[v] = timer++;
            tin[v] = timer2++;
        }
        if (it[v] < (int)adj[v].size()) {        // still neighbours to process
            int u = adj[v][it[v]++];
            if (u == parent[v]) continue;
            if (disc[u] == -1) {                 // tree edge
                parent[u] = v;
                depth[u]  = depth[v] + 1;
                st.push_back(u);
            } else {                             // back edge
                low[v] = min(low[v], disc[u]);
            }
        } else {                                 // all neighbours processed
            tout[v] = timer2++;
            if (parent[v] != -1) low[parent[v]] = min(low[parent[v]], low[v]);
            st.pop_back();
        }
    }

    /* ---------- binary lifting table ---------- */
    vector<array<int, LOG>> up(n);
    for (int v = 0; v < n; ++v) up[v][0] = parent[v];
    for (int k = 1; k < LOG; ++k)
        for (int v = 0; v < n; ++v) {
            int mid = up[v][k-1];
            up[v][k] = (mid == -1 ? -1 : up[mid][k-1]);
        }

    auto isAncestor = [&](int a, int b) -> bool {
        return tin[a] <= tin[b] && tout[b] <= tout[a];
    };

    auto lift = [&](int v, int d) -> int {
        for (int k = LOG-1; k >= 0; --k)
            if (d & (1<<k)) v = up[v][k];
        return v;
    };

    auto directChild = [&](int anc, int desc) -> int {
        // anc is ancestor of desc, anc != desc
        int cur = desc;
        int diff = depth[desc] - depth[anc] - 1;
        return lift(cur, diff);
    };

    /* ---------- component ids after removing each vertex ---------- */
    vector<int> compIdFromParent(n, -1);   // for a separating child : id of its component in the parent
    vector<int> compCnt(n, 0);            // C(v) – number of components after deleting v
    for (int v = 0; v < n; ++v) {
        int cur = (parent[v] == -1 ? 0 : 1);          // id 0 = parent side component (if it exists)
        for (int u : adj[v]) {
            if (parent[u] == v) {                     // u is a child of v in the DFS tree
                if (parent[v] == -1) {                // root – every child separates
                    compIdFromParent[u] = cur++;
                } else {
                    if (low[u] >= disc[v]) {          // separating child
                        compIdFromParent[u] = cur++;
                    }
                }
            }
        }
        compCnt[v] = cur;
    }

    /* ---------- function returning component id of vertex w after removing v ---------- */
    auto component = [&](int v, int w) -> int {
        if (w == v) return -1;                // loop
        if (compCnt[v] == 1) return 0;        // only one component
        // v is an articulation point (C(v) >= 2)
        if (isAncestor(v, w)) {               // w lies in subtree of v
            int child = directChild(v, w);    // first child on the path v → w
            if (parent[v] == -1) {            // root – every child has its own component
                return compIdFromParent[child];
            } else {
                if (low[child] >= disc[v])    // separating child
                    return compIdFromParent[child];
                else
                    return 0;                 // parent side component
            }
        } else {
            // w is outside the subtree of v → parent side component (exists because v is not root)
            return 0;
        }
    };

    /* ---------- compute answers ---------- */
    vector<int> answer(n);
    vector<int> seen(n, 0);
    int curStamp = 1;

    for (int v = 0; v < n; ++v) {
        int stamp = curStamp++;
        int distinct = 0;
        for (int u : adj[v]) {
            int w1 = (u - 1 + n) % n;
            int w2 = (u + 1) % n;
            int c1 = component(v, w1);
            if (c1 != -1 && seen[c1] != stamp) {
                seen[c1] = stamp;
                ++distinct;
            }
            int c2 = component(v, w2);
            if (c2 != -1 && c2 != c1 && seen[c2] != stamp) {
                seen[c2] = stamp;
                ++distinct;
            }
        }
        answer[v] = compCnt[v] - distinct + 1;
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << answer[i];
    }
    cout << '\n';
    return 0;
}
