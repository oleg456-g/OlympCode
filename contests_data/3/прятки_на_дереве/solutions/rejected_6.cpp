#include <bits/stdc++.h>

#ifdef LOCAL
#include "debug.h"
#else
#define debug(x...) 228
#endif

using namespace std;

typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;

#define fi first
#define se second

const int inf = int(1e9) + 1;
const ll infll = ll(1e18) + 1;
const int mod = int(1e9) + 7;
mt19937_64 rnd(chrono::high_resolution_clock::now().time_since_epoch().count());

template<typename T1, typename T2>
bool chmin(T1 &a, T2 b) {
    if (a > b) return a = b, true;
    return false;
}
template<typename T1, typename T2>
bool chmax(T1 &a, T2 b) {
    if (a < b) return a = b, true;
    return false;
}
const int N = 1e6 + 1;
vector<int> g[N];
int dist[N], par[N];
int tin[N], tout[N];
pair<char, int> qq[N];
vector<int> daun;
int timer = 0;
void dfs(int v, int d) {
    tin[v] = timer++;
    daun.push_back(v);
    dist[v] = d;
    for (int u : g[v]) {
        dfs(u, d + 1);
    }
    tout[v] = timer;
}
struct node {
    int mx;
    int cnt;
    node(int val = -inf) {
        mx = val;
        cnt = 1;
    }
};
node merge(node left, node right) {
    node ans = left;
    if (left.mx == right.mx) {
        ans.cnt += right.cnt;
    } else if (left.mx < right.mx) {
        ans = right;
    }
    return ans;
}
node tree[4 * N];
int md[4 * N];
void apply(int v, int x) {
    tree[v].mx += x;
    md[v] += x;
}
void push(int v) {
    apply(2 * v, md[v]);
    apply(2 * v + 1, md[v]);
    md[v] = 0;
}
void update(int v, int l, int r, int ql, int qr, int x) {
    if (max(ql, l) >= min(r, qr)) return;
    if (ql <= l && r <= qr) {
        apply(v, x);
        return;
    }
    int m = (l + r) / 2;
    push(v);
    update(2 * v, l, m, ql, qr, x);
    update(2 * v + 1, m, r, ql, qr, x);
    tree[v] = merge(tree[2 * v], tree[2 * v + 1]);
}
node query(int v, int l, int r, int ql, int qr) {
    if (max(ql, l) >= min(r, qr)) return node();
    if (ql <= l && r <= qr) return tree[v];
    int m = (l + r) / 2;
    push(v);
    return merge(query(2 * v, l, m, ql, qr), query(2 * v + 1, m, r, ql, qr));
}
int root = 0, n;
node query(int ql, int qr) {
    return query(1, 0, n, ql, qr);
}
void update(int ql, int qr, int x) {
    update(1, 0, n, ql, qr, x);
}
bool ancestor(int a, int b) {
    return tin[a] <= tin[b] && tout[a] >= tout[b];
}
int get_left_bs(int p, int v) {
    assert(ancestor(p, v));
    int left = 0, right = (int)g[p].size();
    while (right - left > 1) {
        int mid = (left + right) / 2;
        if (tin[g[p][mid]] > tin[v]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return left;
}
node query_tree(int v) {
    node ans;
    if (root == v) {
        ans = query(0, n);
    } else if (ancestor(root, v)) {
        int left = get_left_bs(root, v);
        ans = merge(query(0, tin[root] + 1), query(tout[root], n));
        if (left + 1 != g[root].size()) {
            ans = merge(ans, query(tin[g[root][left + 1]], tout[root]));
        }
        if (left != 0) {
            ans = merge(ans, query(tin[root] + 1, tout[g[root][left - 1]]));
        }
    } else {
        ans = query(tin[root], tout[root]);
    }
    return ans;
}
void go(int v) {
    if (v == par[root]) {
        update(tin[root], tout[root], +1);
        update(0, tin[root], -1);
        update(tout[root], n, -1);
    } else {
        update(tin[v], tout[v], -1);
        update(0, tin[v], +1);
        update(tout[v], n, +1);
    }
    root = v;
}

void solve_case() {
    int q, m;
    cin >> q >> m;
    int lst = 0;
    par[0] = -1;
    for (int iq = 0; iq < q; iq++) {
        int x;
        char c;
        cin >> c >> x;
        qq[iq].fi = c;
        x = (x - 1) % (lst + 1) + 1;
        x--;
        if (c == '+') {
            lst++;
            par[lst] = x;
            g[x].push_back(lst);
            qq[iq].se = lst;
        } else {
            qq[iq].se = x;
        }
    }
    dfs(0, 0);
    n = lst + 1;
    for (int i = 0; i < n; i++) {
        update(tin[i], tin[i] + 1, dist[i]);
    }
    update(tin[0], tin[0] + 1, +inf);
    for (int iq = 0; iq < q; iq++) {
        int v = qq[iq].se;
        if (qq[iq].fi == '+') {
            int was = query(tin[root], tout[root]).mx;
            update(1, 0, n, tin[v], tin[v] + 1, +inf);
            if (ancestor(root, v)) {
                int u = g[root][get_left_bs(root, v)];
                if (was <= query(tin[v], tin[v] + 1).mx && abs(query(tin[u], tout[u]).mx - max(query(0, tin[u]).mx, query(tout[u], n).mx)) >= 2) {
                    go(u);
                }
            } else {
                int p = par[root];
                if (was <= query(tin[v], tin[v] + 1).mx && abs(query(tin[root], tout[root]).mx - max(query(0, tin[root]).mx, query(tout[root], n).mx)) >= 2) {
                    go(p);
                }
            }
        } else {
            auto n = query_tree(v);
            cout << n.mx << ' ' << n.cnt << "\n";
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0), cout.tie(0);
#ifdef LOCAL
    freopen("inputik.txt", "r", stdin);
    clock_t tStart = clock();
#endif
    int T = 1;
    while (T--) {
        solve_case();
    }
#ifdef LOCAL
    fprintf(stderr, "\n>> Runtime: %.10fs\n", (double) (clock() - tStart) / CLOCKS_PER_SEC);
#endif
}
