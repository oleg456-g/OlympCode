#include "bits/stdc++.h"

using namespace std;

using ll = long long;
using ld = long double;

template<typename T, class F = function<T(const T&, const T&)>>

class SparseTable {
public :
    int n;
    vector<vector<T>> st;
    F func;

    SparseTable(const vector<T>& a, const F& f) : func(f) {
        n = (int) a.size();
        int maxlg = 32 - __builtin_clz(n);
        st.resize(maxlg);
        st[0] = a;
        for (int i = 1; i < maxlg; i++) {
            st[i].resize(n - (1 << i) + 1);
            for (int j = 0; j < n - (1 << i) + 1; j++) {
                st[i][j] = func(st[i - 1][j], st[i - 1][j + (1 << (i - 1))]);
            }
        }
    }

    T get(int l, int r) const {
        assert(0 <= l && l <= r && r <= n - 1);
        int lg = 32 - __builtin_clz(r - l + 1) - 1;
        return func(st[lg][l], st[lg][r - (1 << lg) + 1]);
    }
}; // SparseTable<int> A(a, [&] (int x, int y) {return max(x, y);}); - cоздание

void solve () {
    int n, m;
    cin >> n >> m;
    int X, B, C, mod;
    cin >> X >> B >> C >> mod;
    vector<pair<int, int>> segs(n);
    for (auto& [l, r] : segs) {
        cin >> l >> r;
        l--, r--;
    }
    ll ans = 0;
    {
        const int N = min(m, int(2e5) + 22);
        vector<int> pref(N + 1);
        for (auto& [l, r] : segs) {
            if (l <= N) pref[l]++;
            if (r + 1 <= N) pref[r + 1]--;
        }
        for (int i = 1; i < N; i++) {
            pref[i] += pref[i - 1];
        }
        for (int i = 0; i < N; i++) {
            ans = max(ans, X * 1ll * pref[i]);
            X = (X * 1ll * B + C) % mod;
        }
        if (N == m) {
            cout << ans << '\n';
            return;
        }
        m -= N;
        vector<pair<int, int>> new_segs;
        for (auto& [l, r] : segs) {
            l -= N, r -= N;
            l = max(l, 0);
            if (l <= r) {
                new_segs.push_back({l, r});
            }
        }
        segs = new_segs;
        n = int(segs.size());
    }
    vector<int> value = {X};
    while (1) {
        int x = value.back();
        x = (x * 1ll * B + C) % mod;
        if (x == value[0]) break;
        value.push_back(x);
    }
    const int sz = int(value.size());
    for (int i = 0; i < sz; i++) {
        value.push_back(value[i]);
    }
    SparseTable<int> A(value, [&] (int x, int y) {return max(x, y);});
    int mx = *std::max_element(value.begin(), value.end());
    vector<pair<int, int>> event;
    for (auto& [l, r] : segs) {
        event.push_back({l, 1});
        event.push_back({r + 1, -1});
    }
    std::sort(event.begin(), event.end());
    int cnt = 0;
    for (int i = 0;;) {
        int j = i;
        while (j < int(event.size()) && event[i].first == event[j].first) {
            cnt += event[j].second;
            j++;
        }
        if (j == int(event.size())) {
            break;
        }
        int l = event[i].first;
        int r = event[j].first;
        if (r - l >= sz) {
            ans = max(ans, cnt * 1ll * mx);
        } else {
            r--;
            l %= sz, r %= sz;
            if (l <= r) {
                ans = max(ans, cnt * 1ll * A.get(l, r));
            } else {
                ans = max(ans, cnt * 1ll * A.get(l, r + sz));
            }
        }
        i = j;
    }
    cout << ans << '\n';
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
