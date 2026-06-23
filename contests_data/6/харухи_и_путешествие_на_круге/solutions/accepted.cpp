#include <bits/stdc++.h>

using namespace std;

struct RMQ {
    int n;
    vector<int> sgt;

    template<typename F>
    RMQ(int n, const F& f) : n(n), sgt(2*n) {
        for (int i = 0; i < n; ++i) sgt[i+n] = f(i);
        for (int i = n; --i; ) sgt[i] = min(sgt[2*i], sgt[2*i+1]);
    }

    int operator()(int l, int r) {
        int res = numeric_limits<int>::max();
        for (l += n, r += n; l <= r; l >>= 1, r >>= 1) 
            res = min({res, sgt[l++], sgt[r--]});
        return res;
    }
};

struct CycleDistances {
    int n, m;
    RMQ rmq_a, rmq_b, rmq_c;

    CycleDistances(int n, vector<int> a) : n(n), m(2*n),
        rmq_a(4*n, [&](int i) { return a[i%m]; }),
        rmq_b(4*n, [&](int i) { return a[i%m] + 2*i; }),
        rmq_c(4*n, [&](int i) { return a[i%m] - 2*i; }) {
    }

    int operator()(int u, int v) {
        int ans = min((u-v+m)%m, (v-u+m)%m);
        v = (v+n)%m;
        if (u < v) u += m;

        ans = min(ans, rmq_a(v, u) + u - v);
        ans = min(ans, rmq_c(v, u) + u + v + m);
        ans = min(ans, rmq_a(u, v+m) + v + m - u);
        ans = min(ans, rmq_b(u, v+m) - u - v);

        ans = min(ans, rmq_c(0, v) + u + v);
        ans = min(ans, rmq_c(v, m) + u + v + 2*m);

        ans = min(ans, rmq_b(v+m, 2*m-1)-u-v-m);
        ans = min(ans, rmq_b(0, v+m)-u-v+3*m);
        return ans;
    }
};

void solve() {
    int n, q; cin >> n >> q;
    vector<int> a(2*n); for (int& x : a) cin >> x;
    CycleDistances s(n, a);

    while (q--) {
        int u, v; cin >> u >> v; --u, --v;
        int ans = s(u, v);
        cout << ans << '\n';
    }
}

void stress() {
    int n; cin >> n;
    int m = 2*n;

    auto dist = [=](int i, int j) { return min((i-j+m)%m, (j-i+m)%m); };

    mt19937 rng(1337228);
    for (int t = 10'000; t--; ) {
        vector<int> a(m);
        for (int& x : a) x = rng() % n;
        CycleDistances s(n, a);
        
        for (int i = 0; i < m; ++i)
        for (int j = 0; j < m; ++j) {
            int jans = dist(i, j);
            for (int x = 0; x < m; ++x) {
                int y = (x + n) % m;
                jans = min(jans, dist(i, x) + a[x] + dist(y, j));
            }

            int pans = s(i, j);
            if (pans != jans) {
                cout << n << ' ' << 1 << '\n';
                for (int x : a) cout << x << ' '; cout << '\n';
                cout << i+1 << " " << j+1 << '\n';
                cout << "jans=" << jans << ", but pans=" << pans << endl;
                return;
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);

    // stress();
    int t = 1;
    cin >> t;
    while (t--) solve();
}