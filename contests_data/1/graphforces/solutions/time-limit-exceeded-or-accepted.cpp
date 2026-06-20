#include <bits/stdc++.h>
using namespace std;

const int LOG = 20;                 // 2^20 > 10^6

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> adj(n);
    adj.reserve(n);
    vector<pair<int,int>> edges;
    edges.reserve(m);
    for (int i = 0; i < m; ++i) {
        int v, u;
        cin >> v >> u;
        edges.emplace_back(v, u);
        adj[v].push_back(u);
        adj[u].push_back(v);
    }

    /* ---------- DFS : disc, low, parent, depth, tin, tout, up ---------- */
    vector<int> disc(n, -1), low(n), parent(n, -1), depth(n);
    vector<int> tin(n), tout(n);
    vector<array<int, LOG>> up(n);
    int timer = 0, timer2 = 0;
    struct Frame { int v, p, idx; };
    vector<Frame> st;
    st.emplace_back(0, -1, 0);
    while (!st.empty()) {
        Frame &f = st.back();
        int v = f.v, p = f.p;
        if (disc[v] == -1) {
            disc[v] = low[v] = timer++;
            parent[v] = p;
            depth[v] = (p == -1 ? 0 : depth[p] + 1);
            tin[v] = timer2++;
            up[v][0] = p;
            for (int k = 1; k < LOG; ++k) {
                int mid = up[v][k-1];
                up[v][k] = (mid == -1 ? -1 : up[mid][k-1]);
            }
        }
        if (f.idx < (int)adj[v].size()) {
            int u = adj[v][f.idx++];
            if (u == p) continue;
            if (disc[u] == -1) {
                st.emplace_back(u, v, 0);
            } else {
                low[v] = min(low[v], disc[u]);
            }
        } else {
            if (p != -1) low[p] = min(low[p], low[v]);
            tout[v] = timer2++;
            st.pop_back();
        }
    }

    /* ---------- component information for every vertex ---------- */
    vector<int> compCnt(n);                 // C(v)
    vector<int> compFromParent(n, -1);      // for a separating child x, id of component of parent that contains x
    for (int v = 0; v < n; ++v) {
        int cur = (parent[v] == -1 ? 0 : 1);   // id 0 reserved for parent side if it exists
        for (int u : adj[v]) {
            if (parent[u] == v && low[u] >= disc[v]) {
                compFromParent[u] = cur++;
            }
        }
        compCnt[v] = cur;
    }

    /* ---------- helper : component of vertex w after removing v ---------- */
    auto getComponent = [&](int v, int w) -> int {
        if (w == v) return -1;                     // loop
        if (compCnt[v] == 1) return 0;             // only one component
        // v is an articulation point
        if (tin[v] <= tin[w] && tout[w] <= tout[v]) {
            // w is in the subtree of v
            int cur = w;
            int diff = depth[w] - depth[v] - 1;
            for (int k = LOG - 1; k >= 0; --k) {
                if ((diff >> k) & 1) cur = up[cur][k];
            }
            int child = cur;                       // parent[child] == v
            if (low[child] >= disc[v]) {
                return compFromParent[child];      // separating child component
            } else {
                // belongs to parent side component (exists because v has a parent)
                return (parent[v] != -1 ? 0 : -1);
            }
        } else {
            // outside the subtree -> parent side component
            return (parent[v] != -1 ? 0 : -1);
        }
    };

    vector<int> answer(n);

    /* ---------- process each vertex independently ---------- */
    for (int v = 0; v < n; ++v) {
        int C = compCnt[v];
        if (C == 0) {                     // n == 1
            answer[v] = 1;
            continue;
        }

        // collect incident edges with their possible components
        vector<int> leftC1, leftC2;
        leftC1.reserve(adj[v].size());
        leftC2.reserve(adj[v].size());

        for (int u : adj[v]) {
            int w1 = (u - 1 + n) % n;
            int w2 = (u + 1) % n;
            int c1 = getComponent(v, w1);
            int c2 = getComponent(v, w2);
            if (c1 == -1 && c2 == -1) continue;          // both options are loops
            if (c1 != -1 && c2 != -1 && c1 == c2) c2 = -1; // both options lead to the same component
            leftC1.push_back(c1);
            leftC2.push_back(c2);
        }

        int L = (int)leftC1.size();          // number of usable incident edges
        if (L == 0) {                        // nothing can connect to any component
            answer[v] = C + 1;
            continue;
        }

        // build bipartite graph (components on the right)
        vector<vector<int>> compAdj(C);
        vector<int> compDeg(C, 0);
        for (int i = 0; i < L; ++i) {
            int c1 = leftC1[i];
            int c2 = leftC2[i];
            if (c1 != -1) {
                compAdj[c1].push_back(i);
                ++compDeg[c1];
            }
            if (c2 != -1) {
                compAdj[c2].push_back(i);
                ++compDeg[c2];
            }
        }

        // leaf‑removal to obtain maximum matching size
        queue<int> q;
        for (int c = 0; c < C; ++c)
            if (compDeg[c] == 1) q.push(c);

        vector<char> edgeUsed(L, 0);
        int matched = 0;

        while (!q.empty()) {
            int c = q.front(); q.pop();
            if (compDeg[c] != 1) continue;          // may have changed
            int e = -1;
            for (int ei : compAdj[c]) {
                if (!edgeUsed[ei]) { e = ei; break; }
            }
            if (e == -1) continue;                  // should not happen
            edgeUsed[e] = 1;
            ++matched;
            compDeg[c] = 0;                         // remove component c

            int other = -1;
            if (leftC1[e] != -1 && leftC1[e] != c) other = leftC1[e];
            else if (leftC2[e] != -1 && leftC2[e] != c) other = leftC2[e];

            if (other != -1 && compDeg[other] > 0) {
                --compDeg[other];
                if (compDeg[other] == 1) q.push(other);
            }
        }

        int remaining = 0;
        for (int c = 0; c < C; ++c)
            if (compDeg[c] > 0) ++remaining;

        matched += remaining;               // all remaining components can be matched

        answer[v] = C - matched + 1;
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << answer[i];
    }
    cout << '\n';
    return 0;
}
