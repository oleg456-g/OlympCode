#include <bits/stdc++.h>

// #pragma GCC optimize("O3,unroll-loops")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#define LF '\n'
#define SP ' '
#define all(x) x.begin(), x.end()
#define rep(i, a, b) for (int i = a; i < (b); i++)
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;

constexpr int INF = 1e6;
struct Node {
    int min{INF};
    int min_cw{INF}; // 2*i + a[i]
    int min_ccw{INF}; // -2*i + a[i]
};
Node combine(const Node& l, const Node& r) {
    return Node{
        min(l.min, r.min),
        min(l.min_cw, r.min_cw),
        min(l.min_ccw, r.min_ccw)
    };
}

constexpr int MAXN = 1e5;
int a[MAXN];
Node tr[4*MAXN];

void build_(int v, int l, int r) {
    if (l + 1 == r) {
        int i = l;
        tr[v] = {
            a[i],
            2*i + a[i],
            -2*i + a[i]
        };
        return;
    }
    int m = (l + r) / 2;
    build_(2*v+1, l, m);
    build_(2*v+2, m, r);
    tr[v] = combine(tr[2*v+1], tr[2*v+2]);
}
int n;
void build() {
    build_(0, 0, n);
}

Node query_(int ql, int qr, int v, int l, int r) {
    if (ql == qr)
        return Node{};
    if (ql == l && qr == r)
        return tr[v];

    int m = (l + r) / 2;
    return combine(
        query_(min(ql, m), min(qr, m), 2*v+1, l, m),
        query_(max(ql, m), max(qr, m), 2*v+2, m, r)
    );
}
Node query(int ql, int qr) {
    return query_(ql, qr, 0, 0, n);
}

int dist(int i, int j) {
    int d = abs(i - j);
    return min(d, n - d);
}
int m(int i) {
    return (i + (n / 2)) % n;
}

void solve() {
    int q;
    cin >> n >> q;
    n *= 2;
    rep(i, 0, n) cin >> a[i];

    build();

    rep(i, 0, q) {
        int u, v;
        cin >> u >> v;
        if (u == v) {
            cout << 0 << LF;
            continue;
        }
        u--; v--;
        int ans = dist(u, v);
        int D = dist(u, m(v));

        if ((v - u + n) % n >= (n / 2)) {
            // in cw order:
            // u m(v) m(u) v

            // using .min
            if (m(v) < u) {
                // cross through 2n, 1
                int local = min(query(u, n).min, query(0, m(v) + 1).min);
                local += D;
                ans = min(ans, local);
            } else {
                int local = query(u, m(v) + 1).min;
                local += D;
                ans = min(ans, local);
            }

            // using .min_cw
            if (m(u) < m(v)) {
                // cross through 2n, 1
                int local = min(
                    query(m(v), n).min_cw - 2*m(v),
                    query(0, m(u) + 1).min_cw + 2*(n - m(v))
                );
                local += D;
                ans = min(ans, local);
            } else {
                int local = query(m(v), m(u) + 1).min_cw - 2*m(v);
                local += D;
                ans = min(ans, local);
            }

            // using .min_ccw
            if (u < v) {
                // cross through 2n, 1
                int local = min(
                    query(v, n).min_ccw + 2*n + 2*u,
                    query(0, u + 1).min_ccw + 2*u
                );
                local += D;
                ans = min(ans, local);
            } else {
                int local = query(v, u + 1).min_ccw + 2*u;
                local += D;
                ans = min(ans, local);
            }
        } else {
            // in cw order:
            // u v m(u) m(v)
            // m(u) m(v) u v

            // using .min_cw
            if (v < u) {
                // cross through 2n, 1
                int local = min(
                    query(u, n).min_cw - 2*u,
                    query(0, v + 1).min_cw + 2*(n - u)
                );
                local += D;
                ans = min(ans, local);
            } else {
                int local = query(u, v + 1).min_cw - 2*u;
                local += D;
                ans = min(ans, local);
            }

            // using .min
            if (u < m(v)) {
                // cross through 2n, 1
                int local = min(
                    query(m(v), n).min,
                    query(0, u + 1).min
                );
                local += D;
                ans = min(ans, local);
            } else {
                int local = query(m(v), u + 1).min;
                local += D;
                ans = min(ans, local);
            }

            // using .min_ccw
            if (m(v) < m(u)) {
                // cross through 2n, 1
                int local = min(
                    query(m(u), n).min_ccw + 2*m(v) + 2*n,
                    query(0, m(v) + 1).min_ccw + 2*m(v)
                );
                local += D;
                ans = min(ans, local);
            } else {
                int local = query(m(u), m(v) + 1).min_ccw + 2*m(v);
                local += D;
                ans = min(ans, local);
            }
        }

        cout << ans << LF;
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t --> 0) {
        solve();
    }
}