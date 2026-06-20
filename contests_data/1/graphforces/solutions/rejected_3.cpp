#include <bits/stdc++.h>
using namespace std;

struct Frame {
    int v, p, idx;
    Frame(int _v, int _p, int _i) : v(_v), p(_p), idx(_i) {}
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    if (!(cin >> n >> m)) return 0;
    vector<vector<int>> adj(n);
    adj.reserve(n);
    for (int i = 0; i < m; ++i) {
        int v, u;
        cin >> v >> u;
        adj[v].push_back(u);
        adj[u].push_back(v);
    }

    /* ----------  DFS, disc/low/parent/depth/tin/tout  ---------- */
    vector<int> disc(n, -1), low(n), parent(n, -1), depth(n);
    vector<int> tin(n), tout(n);
    int timer = 0, timer2 = 0;
    const int LOG = 20;                     // 2^20 > 10^6
    vector<array<int, LOG>> up(n);
    for (int i = 0; i < n; ++i) up[i].fill(-1);

    // iterative DFS from vertex 0 (graph is connected)
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
                int mid = up[v][k - 1];
                up[v][k] = (mid == -1 ? -1 : up[mid][k - 1]);
            }
        }
        if (f.idx < (int)adj[v].size()) {
            int u = adj[v][f.idx++];
            if (u == p) continue;
            if (disc[u] == -1) {
                st.emplace_back(u, v, 0);
            } else {
                // back edge
                low[v] = min(low[v], disc[u]);
            }
        } else {
            tout[v] = timer2++;
            if (p != -1) low[p] = min(low[p], low[v]);
            st.pop_back();
        }
    }

    /* ----------  component ids after removing each vertex  ---------- */
    vector<int> compParentID(n, -1);          // id of the parent side component (0 or -1)
    vector<int> compFromParent(n, -1);        // for a vertex that is a separating child
    vector<int> compCnt(n, 0);
    int maxC = 0;
    for (int v = 0; v < n; ++v) {
        int cur = 0;
        if (parent[v] != -1) {
            compParentID[v] = cur++;
        }
        for (int u : adj[v]) {
            if (parent[u] == v) {            // u is a child of v in the DFS tree
                if (low[u] >= disc[v]) {
                    compFromParent[u] = cur++;
                }
            }
        }
        compCnt[v] = cur;
        maxC = max(maxC, cur);
    }
    vector<char> isArtic(n, 0);
    for (int v = 0; v < n; ++v) if (compCnt[v] > 1) isArtic[v] = 1;

    /* ----------  helper: first child of anc on the path to desc ---------- */
    auto getDirectChild = [&](int anc, int desc) -> int {
        int cur = desc;
        int diff = depth[desc] - depth[anc] - 1;
        for (int k = LOG - 1; k >= 0; --k) {
            if (diff & (1 << k)) cur = up[cur][k];
        }
        return cur;               // now parent[cur] == anc
    };

    /* ----------  component of w with respect to vertex v ---------- */
    auto component = [&](int v, int w) -> int {
        if (w == v) return -1;                 // loop
        if (!isArtic[v]) return 0;             // whole graph is one component
        // v is articulation
        if (tin[v] <= tin[w] && tout[w] <= tout[v]) {
            // w is in the subtree of v
            int child = getDirectChild(v, w);
            if (low[child] >= disc[v]) {
                return compFromParent[child];   // separating child component
            } else {
                return compParentID[v];         // parent side component
            }
        } else {
            // outside the subtree → parent side component
            return compParentID[v];
        }
    };

    /* ----------  compute answers ---------- */
    vector<int> answer(n);
    vector<char> has(maxC, 0);          // reused for every articulation vertex

    for (int v = 0; v < n; ++v) {
        int C = compCnt[v];
        if (C == 0) {                       // n = 1
            answer[v] = 1;
            continue;
        }
        if (C == 1) {                       // not an articulation point
            bool hasEdge = false;
            for (int u : adj[v]) {
                int w1 = (u - 1 + n) % n;
                int w2 = (u + 1) % n;
                if (w1 != v || w2 != v) {
                    hasEdge = true;
                    break;
                }
            }
            answer[v] = hasEdge ? 1 : 2;
            continue;
        }
        // articulation vertex, C >= 2
        for (int i = 0; i < C; ++i) has[i] = 0;
        for (int u : adj[v]) {
            int w1 = (u - 1 + n) % n;
            int w2 = (u + 1) % n;
            int c1 = component(v, w1);
            int c2 = component(v, w2);
            if (c1 != -1) has[c1] = 1;
            if (c2 != -1 && c2 != c1) has[c2] = 1;
        }
        int nz = 0;
        for (int i = 0; i < C; ++i) if (has[i]) ++nz;
        answer[v] = C - nz + 1;
    }

    /* ----------  output ---------- */
    for (int i = 0; i < n; ++i) {
        cout << answer[i] << (i + 1 == n ? '\n' : ' ');
    }
    return 0;
}
