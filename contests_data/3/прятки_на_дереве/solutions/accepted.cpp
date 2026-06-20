#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

mt19937 gn(13111);

int MN = 300010;
struct node;
vector<vector<int>> g;
vector<pair<node *, node *>> tin;

struct node {
    int v, h;
    int y = gn();
    node *l = nullptr;
    node *r = nullptr;
    node *p = nullptr;
    int m, k;
    int f = 0;
    int mn;

    node(int v_, int h_) {
        v = v_;
        mn = h_;
        h = h_;
        m = h_;
        k = 1;
    }

    void push() {
        if (f) {
            if (l) {
                l->f += f;
                l->h += f;
                l->m += f;
                l->mn += f;
            }
            if (r) {
                r->f += f;
                r->h += f;
                r->m += f;
                r->mn += f;
            }
            f = 0;
        }
    }


    void upd() {
        m = h;
        k = 1;
        mn = h;
        if (l) {
            mn = min(mn, l->mn);
            if (l->m > m) {
                m = l->m;
                k = l->k;
            } else if (l->m == m) {
                k += l->k;
            }
        }
        if (r) {
            mn = min(mn, r->mn);
            if (r->m > m) {
                m = r->m;
                k = r->k;
            } else if (r->m == m) {
                k += r->k;
            }
        }
    }
};

void upd_p(node *v, node *p) {
    if (v) {
        if (p) {
            p->push();
        }
        v->p = p;
        if (p) {
            p->upd();
        }
    }
}


node *merge2(node *a, node *b) {
    if (a == nullptr) {
        if (b) {
            b->push();
            b->upd();
        }
        return b;
    }
    if (b == nullptr) {
        a->push();
        a->upd();
        return a;
    }
    a->push();
    b->push();
    a->upd();
    b->upd();
    if (a->y < b->y) {
        node *t = merge2(a, b->l);
        b->l = t;
        upd_p(t, b);
        b->upd();
        return b;
    } else {
        node *t = merge2(a->r, b);
        a->r = t;
        upd_p(t, a);
        a->upd();
        return a;
    }
}

void push_up(node *v) {
    if (v) {
        if (v->p) {
            push_up(v->p);
        }
        v->push();
        v->upd();
    }
}

node *merge(node *a, node *b) {
    auto p = merge2(a, b);
    push_up(p);
    return p;
}

pair<node *, node *> split_l(node *, node *);

pair<node *, node *> split_r(node *, node *);

pair<node *, node *> split_l(node *a, node *b) {
    if (a->p == nullptr) {
        return {b, a};
    }
    a->push();
    a->upd();
    if (a->p->l == a) {
        return split_l(a->p, b);
    } else {
        node *t = a->p;
        a->p = nullptr;
        t->r = b;
        upd_p(b, t);
        t->upd();
        return split_r(t, a);
    }
}

pair<node *, node *> split_r(node *a, node *b) {
    if (a->p == nullptr) {
        return {a, b};
    }
    a->push();
    a->upd();
    if (a->p->r == a) {
        return split_r(a->p, b);
    } else {
        node *t = a->p;
        a->p = nullptr;
        t->l = b;
        upd_p(b, t);
        t->upd();
        return split_l(t, a);
    }
}


pair<node *, node *> split_l(node *a) {
    push_up(a);
    a->push();
    a->upd();
    node *t = a->l;
    if (t) {
        t->push();
        t->upd();
    }
    a->l = nullptr;
    upd_p(t, nullptr);
    a->upd();
    auto p = split_l(a, t);
    push_up(p.first);
    push_up(p.second);
    return p;
}

pair<node *, node *> split_r(node *a) {
    push_up(a);
    a->push();
    a->upd();
    node *t = a->r;
    if (t) {
        t->push();
        t->upd();
    }
    a->r = nullptr;
    upd_p(t, nullptr);
    a->upd();
    auto p = split_r(a, t);
    push_up(p.first);
    push_up(p.second);
    return p;
}


int getm(node *v) {
    if (v) return v->m;
    return 0;
}

int getk(node *v) {
    if (v) return v->k;
    return 0;
}

int getmn(node *v) {
    if (v) return v->mn;
    return MN;
}

int get_ROOT(node *v) {
    push_up(v);
    if (v->h == 0) return v->v;
    while (v->mn > 1) {
        v = v->p;
    }
    while (v->h != 1) {
        v->push();
        if (getmn(v->l) <= 1) {
            v = v->l;
            v->push();
            while (getmn(v->r) <= 1) {
                v = v->r;
                v->push();
            }
        } else if (getmn(v->r) <= 1) {
            v = v->r;
            v->push();
            while (getmn(v->l) <= 1) {
                v = v->l;
                v->push();
            }
        } else {
            break;
        }
    }
    return v->v;
}

void add(node *v, int val) {
//    if (v) {
    v->f += val;
    v->m += val;
    v->mn += val;
    v->h += val;
//    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    g.reserve(MN);
    tin.reserve(MN);
    int q, m;
    cin >> q >> m;
    int sz = 1;
    int la = 0;
    tin.push_back({new node(0, 0), new node(0, 0)});
    node *root = merge(tin[0].first, tin[0].second);
    g.push_back({});
    int ROOT = 0;
    while (q--) {
        char c;
        int x;
        cin >> c >> x;
        x = (la * m + x - 1) % sz;
        if (c == '+') {
            push_up(tin[x].first);
            int h = tin[x].first->h;
            tin.push_back({new node(sz, h + 1), new node(sz, h + 1)});
            g[x].push_back(sz);
            g.push_back({x});
            int NR = get_ROOT(tin[x].first);
            int h2 = root->m;
            auto p1 = split_l(tin[x].first);
            auto p2 = split_l(tin[x].second);

            root = merge(p1.first, merge(merge(p2.first, merge(tin[sz].first, tin[sz].second)), p2.second));
            if (h == h2) {
                p1 = split_l(tin[NR].first);
                p2 = split_r(tin[NR].second);
                if (h + 1 - max(getm(p1.first), getm(p2.second)) >= 2) {
                    // p2.first - NR
                    add(p2.first, -1);
                    auto p3 = split_l(tin[NR].second);
                    node *t = merge(p1.first, p2.second);
                    add(t, 1);
                    t = merge(p3.first, t);
                    root = merge(t, p3.second);
                    ROOT = NR;
                } else {
                    node *t = merge(p1.first, p2.first);
                    root = merge(t, p2.second);
                }
            }
            sz++;
        } else {
            push_up(tin[x].first);
            int h = tin[x].first->h;
            if (x == ROOT) {
                cout << root->m << ' ' << root->k / 2 << '\n';
                la = root->m + root->k / 2;
                continue;
            }
            int NR = get_ROOT(tin[x].first);
            auto p1 = split_l(tin[NR].first);
            auto p2 = split_r(tin[NR].second);
            int m1 = getm(p1.first);
            int k1 = getk(p1.first);
            int m2 = getm(p2.second);
            int k2 = getk(p2.second);
            if (m1 == m2) k1 += k2;
            if (m1 < m2) {
                swap(k1, k2);
                swap(m1, m2);
            }
            k1 /= 2;
            cout << h + m1 << ' ' << k1 << '\n';
            la = h + m1 + k1;
            root = merge(merge(p1.first, p2.first), p2.second);
        }
    }
    return 0;
}

