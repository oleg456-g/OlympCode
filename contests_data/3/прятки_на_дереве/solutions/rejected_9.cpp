#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <set>
#include <algorithm>

using namespace std;

const long long inf = 1e18;

struct node {
    node *l = nullptr;
    node *r = nullptr;
    node *parent = nullptr;
    long long x, add = 0, max;
    int size = 0, count;
    int y;

    node(int x): x(x) {
        y = rand();
        size = 1;
        max = x;
        count = 1;
    }
};

int s(node *v) {
    if (!v) {
        return 0;
    }
    return v->size;
}

long long max(node *v) {
    if (!v) {
        return -inf;
    }
    return v->max;
}

long long add(node *v) {
    if (!v) {
        return 0;
    }
    return v->add;
}

int cmax(node *v) {
    if (!v) {
        return 0;
    }
    return v->count;
}

void upd(node *v) {
    if (!v) {
        return;
    }
    v->size = s(v->l) + s(v->r) + 1;
    v->max = max(max(v->l) + add(v->l), max(max(v->r) + add(v->r), v->x));
    if (v->x == v->max) {
        v->count = 1;
    } else {
        v->count = 0;
    }
    if (max(v->l) + add(v->l) == v->max) {
        v->count += cmax(v->l);
    }
    if (max(v->r) + add(v->r) == v->max) {
        v->count += cmax(v->r);
    }
    if (v->l) {
        v->l->parent = v;
    }
    if (v->r) {
        v->r->parent = v;
    }
    v->parent = nullptr;
}

void push(node *v) {
    if (!v) {
        return;
    }
    if (v->l) {
        v->l->add += v->add;
    }
    if (v->r) {
        v->r->add += v->add;
    }
    v->x += v->add;
    v->max += v->add;
    v->add = 0;
}

node *merge(node *l, node *r) {
    if (!l) {
        return r;
    }
    if (!r) {
        return l;
    }
    if (l->y > r->y) {
        push(l);
        l->r = merge(l->r, r);
        upd(l);
        return l;
    }
    push(r);
    r->l = merge(l, r->l);
    upd(r);
    return r;
}

std::pair<node *, node *> split(node *v, int k) {
    if (!v) {
        return { nullptr, nullptr };
    }
    push(v);
    if (s(v->l) + 1 <= k) {
        std::pair<node *, node *> q = split(v->r, k - s(v->l) - 1);
        v->r = q.first;
        upd(v);
        return {v, q.second};
    }
    std::pair<node *, node *> q = split(v->l, k);
    v->l = q.second;
    upd(v);
    return {q.first, v};
}

int get_id(node *v) {
    int id = s(v->l);
    node *ptr = v->parent;
    while (ptr) {
        if (ptr->r == v) {
            id += s(ptr->l) + 1;
        }
        ptr = ptr->parent;
        v = v->parent;
    }
    return id;
}

const int MAXN = 300'000;
const int LOG = 19;
int h[MAXN];
int up[LOG][MAXN];
int parent[MAXN];
node *vin[MAXN];
node *vout[MAXN];
int c = 1;

int go_up(int u, int k) {
    for (int i = LOG - 1; i >= 0; --i) {
        if (k >= (1 << i)) {
            k -= (1 << i);
            u = up[i][u];
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

int v1 = 0, v2 = 0; // diam
int center = 0;
node *root = nullptr;

int getf(int v) {
    return get_id(vin[v]);
}

int getl(int v) {
    return get_id(vout[v]);
}

void add_seg(int l, int r, int x) {
    std::pair<node *, node *> q = split(root, r + 1);
    std::pair<node *, node *> q1 = split(q.first, l);
    q1.second->add += x;
    root = merge(q1.first, merge(q1.second, q.second));
}

int get_count(int l, int r) {
    std::pair<node *, node *> q = split(root, r + 1);
    std::pair<node *, node *> q1 = split(q.first, l);
    int res = cmax(q1.second);
    root = merge(q1.first, merge(q1.second, q.second));
    return res;
}

int get_count1(int l, int r) {
    std::pair<node *, node *> q = split(root, r);
    std::pair<node *, node *> q1 = split(q.first, l);
    node *t = merge(q1.first, q.second);
    int res = cmax(t);
    std::pair<node *, node *> q2 = split(t, l);
    root = merge(q2.first, merge(q1.second, q2.second));
    return res;
}

long long getVal(int i, node* v = root) {
    pair<node *, node *> q = split(v, i + 1);
    pair<node *, node *> q1 = split(q.first, i);
    long long res = q1.second->x + q1.second->add;
    root = merge(q1.first, merge(q1.second, q.second));
    return res;
}

void add_vertex(int p, int v) {
    h[v] = h[p] + 1;
    parent[v] = p;
    up[0][v] = p;
    for (int i = 1; i < LOG; ++i) {
        up[i][v] = up[i - 1][up[i - 1][v]];
    }
    int len = h[v];
    vin[v] = new node(len);
    vout[v] = new node(len);
    int i = getf(p);
    std::pair<node *, node *> q = split(root, i + 1);
    root = merge(q.first, merge(merge(vin[v], vout[v]), q.second));
}

int ans(int v) {
    return cmax(root);
}

void move_center(int added) {
    int len = dist(v1, v2);
    int x = dist(v1, added);
    bool check_needed = false;
    if (len < x) {
        len = x;
        v2 = added;
    }
    int y = dist(v2, added);
    if (len < y) {
        len = y;
        v1 = added;
    }
}

void move_along_edge(int to) {
    int from = center;
    int w = 1;
    if (to == parent[from]) {
        w = -1;
    }
    center = to;
    int L = getf(from);
    int R = getf(to);
    if (L > R) {
        swap(from, to);
        swap(L, R);
    }
    swap(L, R);
    R = getl(to);
    add_seg(0, 2 * c - 1, w);
    add_seg(L, R, -w * 2);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    vin[0] = new node(0);
    vout[0] = new node(0);
    root = merge(vin[0], vout[0]);
    int q, m;
    cin >> q >> m;
    int answ = 0;
    while (q--) {
        char t;
        int v;
        cin >> t >> v;
        if (m == 0) {
            v = (v - 1) % c + 1;
        } else {
            v = ((answ + (v - 1)) % c + c) % c + 1;
        }
        if (t == '+') {
            add_vertex(v - 1, c);
            ++c;
            move_center(c - 1);
        } else {
            int x = max(dist(v1, v - 1), dist(v2, v - 1));
            if (v != 1) {
                move_along_edge(v - 1);
            }
            int y = ans(v - 1) / 2;
            if (v != 1) {
                move_along_edge(0);
            }
            answ = x + y;
            cout << x << ' ' << y << '\n';
        }
    }
}
