#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <set>
#include <algorithm>
 
using namespace std;
const long long inf = 1e9;
const int MAXN = 300'000;
const int LOG = 19;
int h[MAXN];
int up[LOG][MAXN];
int up1[LOG][MAXN];
int parent[MAXN];
int c = 1;
int v1 = 0, v2 = 0; // diam
int center = 0;
 
 
int go_up(int u, int k) {
    for (int i = LOG - 1; i >= 0; --i) {
        if (k >= (1 << i)) {
            k -= (1 << i);
            u = up[i][u];
        }
    }
    return u;
}

int go_up1(int u, int k) {
    for (int i = LOG - 1; i >= 0; --i) {
        if (k >= (1 << i)) {
            k -= (1 << i);
            u = up1[i][u];
        }
    }
    return u;
}
 
int lca(int u, int v) {
    if (h[u] < h[v]) {
        swap(u, v);
    }
    u = go_up(u, h[u] - h[v]);
    if (u == v) {
        return u;
    }
    for (int i = LOG - 1; i >= 0; --i) {
        if (up[i][u] != up[i][v]) {
            u = up[i][u];
            v = up[i][v];
        }
    }
    return up[0][u];
}
 
int dist(int u, int v) {
    return h[u] + h[v] - 2 * h[lca(u, v)];
}
 
void move_center(int added) {
    int len = dist(v1, v2);
    int x = dist(v1, added);
    bool check_needed = false;
    if (len < x) {
        len = x;
        v2 = added;
        check_needed = true;
    }
    int y = dist(v2, added);
    if (len < y) {
        len = y;
        v1 = added;
        check_needed = true;
    }
    if (check_needed) {
        int dst = dist(center, added);
        if (dst > len / 2) {
            if (dst > dist(added, parent[center])) {
                center = parent[center];
            } else {
                int u = go_up(added, dst - 1);
                center = u;
            }
        }
    }
}
 
vector<vector<int>> g;
 
void add_vertex(int p, int v) {
    g[p].push_back(v);
    g[v].push_back(p);
    h[v] = h[p] + 1;
    parent[v] = p;
    up[0][v] = p;
    for (int i = 1; i < LOG; ++i) {
        up[i][v] = up[i - 1][up[i - 1][v]];
    }
}
 
void dfs(int v, vector<vector<int>> &g, vector<int> &d, int p) {
    up1[0][v] = p;
    for (int i = 1; i < LOG; ++i) {
        up1[i][v] = up1[i - 1][up1[i - 1][v]];
    }
    for (int u : g[v]) {
        if (u != p) {
            d[u] = d[v] + 1;
            dfs(u, g, d, v);
        }
    }
}
 
vector<int> et;
int tin[MAXN];
int tout[MAXN];
int timer = 0;
const int MAX = 1 << 19;
pair<int, int> tree[MAX * 2];
 
void dfs1(int v, vector<vector<int>> &g, vector<int> &d, int p) {
    et.push_back(v);
    tin[v] = timer++;
    for (int u : g[v]) {
        if (u != p) {
            dfs1(u, g, d, v);
        }
    }
    et.push_back(v);
    tout[v] = timer++;
}
 
pair<int, int> merge(pair<int, int> a, pair<int, int> b) {
    pair<int, int> res = a;
    if (b.first > res.first) {
        res = b;
    } else if (b.first == res.first) {
        res.second += b.second;
    }
    return res;
}
 
pair<int, int> get(int l, int r) {
    l += MAX;
    r += MAX;
    pair<long long, int> res = { -inf, 0 };
    while (l < r) {
        if (l & 1) {
            res = merge(res, tree[l++]);
        }
        if (r & 1) {
            res = merge(res, tree[--r]);
        }
        l >>= 1;
        r >>= 1;
    }
    return res;
}
 
void upd(int i, pair<int, int> x) {
    i += MAX;
    tree[i] = x;
    while (i > 1) {
        i >>= 1;
        tree[i] = merge(tree[i * 2], tree[i * 2 + 1]);
    }
}
 
int get_count1(int l, int r) {
    int res = merge(get(0, l), get(r + 1, timer)).second;
    return res;
}
 
int ans(int v) {
    int total = tree[1].second;
    int dst = dist(v, center);
    if (v == center) {
        return total;
    }
    int u = go_up1(v, dst - 1);
    return get_count1(tin[u], tout[u]);
}
 
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    int q, m;
    cin >> q >> m;
    int answ = 0;
    bool flag = false;
    g.resize(q);
    vector<int> d;
    while (q--) {
        char t;
        int v;
        cin >> t >> v;
        v = (v - 1) % c + 1;
        if (t == '+') {
            add_vertex(v - 1, c);
            ++c;
            move_center(c - 1);
        } else {
            if (!flag) {
                d.resize(c);
                dfs(center, g, d, center);
                dfs1(center, g, d, -1);
                for (int i = 0; i < et.size(); ++i) {
                    upd(i, make_pair(d[et[i]], 1));
                }
            }
            flag = true;
            int x = max(dist(v1, v - 1), dist(v2, v - 1));
            int y = ans(v - 1) / 2;
            answ = x + y;
            cout << x << ' ' << y << '\n';
        }
    }
}
