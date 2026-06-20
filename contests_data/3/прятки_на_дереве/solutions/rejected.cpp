#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;
vector <vector <int>> g;
vector <int> gl, bp, p, tin, sz;
int cha = 0;

void dfs1(int v) {
    sz[v] = 1;
    tin[v] = cha++;
    if (gl[p[v]] - gl[bp[p[v]]] == gl[bp[p[v]]] - gl[bp[bp[p[v]]]]) bp[v] = bp[bp[p[v]]];
    else bp[v] = p[v];
    for (int i : g[v]) {
        p[i] = v;
        gl[i] = gl[v] + 1;
        dfs1(i);
        sz[v] += sz[i];
    }
}

inline int parent(int a, int k) {
    while (k) {
        int delta = gl[a] - gl[bp[a]];
        if (delta <= k) {
            k -= delta;
            a = bp[a];
        } else {
            k--;
            a = p[a];
        }
    }
    return a;
}

inline int lca(int a, int b) {
    if (gl[a] < gl[b]) swap(a, b);
    a = parent(a, gl[a] - gl[b]);
    if (a == b) return a;
    while (p[a] != p[b]) {
        if (bp[a] != bp[b]) {
            a = bp[a];
            b = bp[b];
        } else {
            a = p[a];
            b = p[b];
        }
    }
    return p[a];
}

pair <int, int> merge(pair <int, int> a, pair <int, int> b) {
    if (a.first == b.first) return make_pair(a.first, a.second + b.second);
    return max(a, b);
}

struct SegTree {
    int n;
    vector <pair <int, int>> t;
    vector <int> p;

    SegTree(int n) : n(n) {
        t.resize(4 * n, make_pair(-1, 0));
        p.resize(4 * n);
    }

    inline void push(int v) {
        p[2 * v] += p[v];
        p[2 * v + 1] += p[v];
        t[2 * v].first += p[v];
        t[2 * v + 1].first += p[v];
        p[v] = 0;
    }

    void set_val(int v, int l, int r, int i, int x) {
        if (l > i || i > r) return;
        if (l == r) {
            t[v] = make_pair(x, 1);
            return;
        }
        push(v);
        int m = (l + r) / 2;
        set_val(2 * v, l, m, i, x);
        set_val(2 * v + 1, m + 1, r, i, x);
        t[v] = merge(t[2 * v], t[2 * v + 1]);
    }

    inline void set_val(int i, int x) {
        set_val(1, 0, n - 1, i, x);
    }

    void update(int v, int l, int r, int A, int B, int x) {
        if (l > B || A > r) return;
        if (A <= l && r <= B) {
            t[v].first += x;
            p[v] += x;
            return;
        }
        push(v);
        int m = (l + r) / 2;
        update(2 * v, l, m, A, B, x);
        update(2 * v + 1, m + 1, r, A, B, x);
        t[v] = merge(t[2 * v], t[2 * v + 1]);
    }

    inline void update(int l, int r, int x) {
        update(1, 0, n - 1, l, r, x);
    }

    pair <int, int> get_seg(int v, int l, int r, int A, int B) {
        if (l > B || A > r) return make_pair(-1, 0);
        if (A <= l && r <= B) return t[v];
        push(v);
        int m = (l + r) / 2;
        return merge(get_seg(2 * v, l, m, A, B), get_seg(2 * v + 1, m + 1, r, A, B));
    }

    pair <int, int> get_seg(int l, int r) {
        return get_seg(1, 0, n - 1, l, r);
    }
};

inline int dist(int a, int b) {
    int lc = lca(a, b);
    return gl[a] + gl[b] - 2 * gl[lc];
}

inline bool check_pod(int a, int b) {
    //b in a
    return tin[b] >= tin[a] && tin[b] <= tin[a] + sz[a] - 1;
}

pair <int, int> find_middle(int a, int b) {
    int lc = lca(a, b);
    int d = gl[a] + gl[b] - 2 * gl[lc];
    if (d % 2 == 0) {
        int ans_v;
        if (gl[a] == gl[b]) ans_v = lc;
        if (gl[a] > gl[b]) ans_v = parent(a, d / 2);
        else ans_v = parent(b, d / 2);
        return make_pair(ans_v, ans_v);
    } else {
        pair <int, int> ans;
        if (gl[a] > gl[b]) {
            ans.first = parent(a, d / 2);
            a = p[a];
        } else {
            ans.first = parent(b, d / 2);
            b = p[b];
        }
        if (gl[a] == gl[b]) ans.second = lc;
        if (gl[a] > gl[b]) ans.second = parent(a, d / 2);
        else ans.second = parent(b, d / 2);
        return ans;
    }
}

inline int get_flex(int a, int b) {
    int lc = lca(a, b);
    int d = gl[a] + gl[b] - 2 * gl[lc];
    if (d % 2 == 0) {
        if (gl[a] == gl[b]) return lc;
        else if (gl[a] > gl[b]) return parent(a, d / 2);
        else return parent(b, d / 2);
    } else {
        if (gl[a] > gl[b]) return parent(a, d / 2);
        else return parent(b, d / 2 + 1);
    }
}

void solve() {
    int q, m;
    cin >> q >> m;
    g.emplace_back();
    vector <int> type(q);
    vector <int> num(q);
    int now = 1;
    for (int i = 0; i < q; ++i) {
        char c;
        cin >> c;
        cin >> num[i];
        num[i] = (num[i] - 1) % now + 1;
        --num[i];
        if (c == '+') {
            g.emplace_back();
            type[i] = 1;
            g[num[i]].push_back(now++);
        }
    }
    tin.resize(now);
    sz.resize(now);
    gl.resize(now);
    bp.resize(now);
    p.resize(now);
    dfs1(0);
    int d1 = 0, d2 = 0, m1 = 0, m2 = 0;
    SegTree t1(now), t2(now);
    t1.set_val(0, 0);
    t2.set_val(0, 0);

    auto move = [&](int x, int y, SegTree& t) {
        int l, r, pl;
        if (gl[x] > gl[y]) {
            l = tin[x];
            r = tin[x] + sz[x] - 1;
            pl = 1;
        } else {
            l = tin[y];
            r = tin[y] + sz[y] - 1;
            pl = -1;
        }
        t.update(l, r, pl);
        t.update(0, l - 1, -pl);
        t.update(r + 1, now - 1, -pl);
    };

    auto get_ans = [&](int mid, int en, SegTree& t) {
        if (!check_pod(mid, en)) return t.get_seg(tin[mid], tin[mid] + sz[mid] - 1).second;
        else {
            int kek = parent(en, gl[en] - gl[mid] - 1);
            return merge(t.get_seg(0, tin[kek] - 1), t.get_seg(tin[kek] + sz[kek], now - 1)).second;
        }
    };

    int kek = 0;

    for (int i = 0; i < q; ++i) {
        if (type[i] == 1) {
            kek++;
            int new_dist1 = dist(kek, d1);
            int new_dist2 = dist(kek, d2);
            t1.set_val(tin[kek], dist(kek, m1));
            t2.set_val(tin[kek], dist(kek, m2));
            int cur_dist = dist(d1, d2);
            if (new_dist1 > cur_dist) {
                d2 = kek;
            } else if (new_dist2 > cur_dist) {
                d1 = kek;
            } else {
                continue;
            }
            pair <int, int> tmp = find_middle(d1, d2);
            if (tmp.first == tmp.second) {
                if (m1 != tmp.first) {
                    move(m1, tmp.first, t1);
                    m1 = tmp.first;
                } else {
                    move(m2, tmp.first, t2);
                    m2 = tmp.first;
                }
            } else {
                if (tmp.first == m1) {
                    move(m1, tmp.second, t1);
                    m1 = tmp.second;
                } else {
                    move(m1, tmp.first, t1);
                    m1 = tmp.first;
                }
            }
        } else {
            int v = num[i];
            int dist1 = dist(v, d1);
            int dist2 = dist(v, d2);
            if (dist1 == dist2 && v == get_flex(d1, d2)) {
                cout << max(dist1, dist2) << ' ' << t1.get_seg(0, now - 1).second << "\n";
            } else if (dist1 > dist2) {
                cout << max(dist1, dist2) << ' ' << get_ans(get_flex(d1, d2), v, t1) << "\n";
            } else {
                cout << max(dist1, dist2) << ' ' << get_ans(get_flex(d2, d1), v, t2) << "\n";
            }
        }
    }
}

signed main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    solve();
}
