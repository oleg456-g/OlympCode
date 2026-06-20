#include<bits/stdc++.h>
using namespace std;

using ll = long long;

const int R = 1 << 18;

int t[R * 2];

void build(const vector<int>& a) {
    for (int i = 0; i < a.size(); ++i) {
        t[i + R] = a[i];
    }
    for (int i = R - 1; i > 0; --i) {
        t[i] = max(t[i * 2], t[i * 2 + 1]);
    }
}

int get(int ql, int qr) {
    if (ql > qr) {
        return 0;
    }
    int ans = 0;
    for (ql += R - 1, qr += R - 1; ql <= qr; ql >>= 1, qr >>= 1) {
        if (ql & 1) ans = max(ans, t[ql++]);
        if (~qr & 1) ans = max(ans, t[qr--]);
    }
    return ans;
}

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);
    int n, m;
    cin >> n >> m;
    int x, b, c, mod;
    cin >> x >> b >> c >> mod;
    vector<pair<int, int>> otr(n);
    vector<pair<int, int>> evs;
    for (auto& [l, r] : otr) {
        cin >> l >> r;
        evs.emplace_back(l, -1);
        evs.emplace_back(r, 1);
    }
    vector<int> a(m);
    a[0] = x;
    for (int i = 1; i < m; ++i) {
        a[i] = (1LL * a[i - 1] * b + c) % mod;
    }
    build(a);
    ll ans = 0;
    sort(evs.begin(), evs.end());
    int bal = 0, lst = INT_MAX / 3;
    for (auto& [x, event] : evs) {
        if (event == 1) {
            ans = max(ans, 1LL * get(lst, x) * bal);
        } else {
            ans = max(ans, 1LL * get(lst, x - 1) * bal);
            lst = x;
        }
        bal -= event;
    }
    cout << ans;
}
