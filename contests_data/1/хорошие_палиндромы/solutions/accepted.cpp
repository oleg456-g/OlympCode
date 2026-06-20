#pragma GCC optimize("O3")
#include "bits/allocator.h"
#pragma GCC target("avx2")
#include "bits/stdc++.h"

using namespace std;

using ll = long long;
using ld = long double;

const int N = int(5e5) + 22;
const int mod = 998244353, p = 101;
int pw[N], a_pw[N], b_pw[N];

struct Node {
    int val = 0;
    int len = 0;
    int ca = 0;
    int cb = 0;

    Node() {
        val = len = ca = cb = 0;
    }

    Node(char ch, int cnt = 1) {
        val = ch == 'a' ? a_pw[cnt] : b_pw[cnt];
        len = cnt;
        ca = ch == 'a' ? cnt : 0;
        cb = ch =='b' ? cnt : 0;
    }
};

Node merge(const Node& a, const Node& b) {
    if (!a.len) return b;
    if (!b.len) return a;
    Node res;
    res.len = a.len + b.len;
    res.ca = a.ca + b.ca;
    res.cb = a.cb + b.cb;
    res.val = (a.val + b.val * 1ll * pw[a.len]) % mod;
    return res;
}

struct tree {
    vector<Node> t;

    void build(int v, int l, int r, const string& s) {
        if (l + 1 == r) {
            t[v] = Node(s[l]);
            return;
        }
        int m = (l + r) / 2;
        build(v * 2 + 1, l, m, s);
        build(v * 2 + 2, m, r, s);
        t[v] = merge(t[v * 2 + 1], t[v * 2 + 2]);
    }

    void build(const string& s) {
        const int n = int(s.size());
        t.resize(4 * n);
        build(0, 0, n, s);
    }

    void upd(int v, int l, int r, int i, char x) {
        if (l + 1 == r) {
            t[v] = Node(x);
            return;
        }
        int m = (l + r) / 2;
        if (i < m) {
            upd(v * 2 + 1, l, m, i, x);
        } else {
            upd(v * 2 + 2, m, r, i, x);
        }
        t[v] = merge(t[v * 2 + 1], t[v * 2 + 2]);
    }

    Node get(int v, int l, int r, int l1, int r1, int& a, int& b) {
        if (l >= r1 || l1 >= r) return Node();
        if (l1 <= l && r <= r1) {
            if ((!a || !t[v].ca) && (!b || !t[v].cb)) return t[v];
            if (a >= t[v].ca && b >= t[v].cb) {
                a -= t[v].ca;
                b -= t[v].cb;
                return Node();
            }
            if (a >= t[v].ca) {
                a -= t[v].ca;
                Node res = Node('b', t[v].cb - b);
                b = 0;
                return res;
            }
            if (b >= t[v].cb) {
                b -= t[v].cb;
                Node res = Node('a', t[v].ca - a);
                a = 0;
                return res;
            }
            if (l + 1 == r) {
                assert(false);
            }
        }
        int m = (l + r) / 2;
        Node left = get(v * 2 + 1, l, m, l1, r1, a, b);
        Node right = get(v * 2 + 2, m, r, l1, r1, a, b);
        return merge(left, right);
    }
};

void solve () {
    pw[0] = 1;
    for (int i = 1; i < N; i++) {
        pw[i] = pw[i - 1] * 1ll * p % mod;
        a_pw[i] = int('a') + a_pw[i - 1] * 1ll * p % mod;
        b_pw[i] = int('b') + b_pw[i - 1] * 1ll * p % mod;
    }
    string s; cin >> s;
    tree S, T;
    S.build(s);
    string rs = s;
    std::reverse(rs.begin(), rs.end());
    T.build(rs);
    const int n = int(s.size());
    int q; cin >> q;
    while (q--) {
        char tp; cin >> tp;
        if (tp == '!') {
            int i; cin >> i; i--;
            s[i] = s[i] == 'a' ? 'b' : 'a';
            S.upd(0, 0, n, i, s[i]);
            T.upd(0, 0, n, n - i - 1, s[i]);
        } else {
            int l, r;
            cin >> l >> r;
            l--, r--;
            int _ = 0, __ = 0;
            Node kek = S.get(0, 0, n, l, r + 1, _, __);
            int mn = min(kek.ca, kek.cb);
            if (!mn) {
                cout << 0 << '\n';
                continue;
            }
            int mid1 = (l + r) / 2;
            int mid2 = (l + r + 1) / 2;
            int L = -1, R = (mn + 1) / 2;
            while (L + 1 < R) {
                int M = (L + R) / 2;
                int x1 = M, x2 = M, x3 = M, x4 = M;
                Node lNode = S.get(0, 0, n, l, mid1 + 1, x1, x2);
                Node rNode = T.get(0, 0, n, n - r - 1, n - mid2, x3, x4);
                if (lNode.ca != rNode.ca || lNode.cb != rNode.cb) {
                    break;
                }
                if (lNode.val == rNode.val) {
                    R = M;
                } else {
                    L = M;
                }
            }
            cout << R << '\n';
        }
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    int t = 1;
//    cin >> t;
    while (t--) {
        solve();
    }
}
