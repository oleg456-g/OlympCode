#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const int R = 1 << 19;
int t[R * 2];

void build(const vector<int>& a) {
    fill(t, t + 2 * R, 0);
    for (int i = 0; i < a.size(); ++i) t[i + R] = a[i];
    for (int i = R - 1; i > 0; --i) t[i] = max(t[i * 2], t[i * 2 + 1]);
}

int get(int ql, int qr) {
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
    auto nxt = [&](int v) -> int {
        return (1LL * v * b + c) % mod;
    };
    int tort = nxt(x);
    int hare = nxt(nxt(x));
    while (tort != hare) {
        tort = nxt(tort);
        hare = nxt(nxt(hare));
    }
    int mu = 0;
    tort = x;
    while (tort != hare) {
        tort = nxt(tort);
        hare = nxt(hare);
        ++mu;
    }
    int lambda = 1;
    hare = nxt(tort);
    while (tort != hare) {
        hare = nxt(hare);
        ++lambda;
    }
    int totalLen = mu + lambda;
    vector<int> seq(totalLen);
    seq[0] = x;
    for (int i = 1; i < totalLen; ++i) seq[i] = nxt(seq[i - 1]);
    int maxCycle = 0;
    for (int i = mu; i < mu + lambda; ++i) maxCycle = max(maxCycle, seq[i]);
    build(seq);
    auto maxA = [&](ll l, ll r) -> int {
        if (l > r) return 0;
        if (r <= mu) return get((int)l, (int)r);
        int ans = 0;
        if (l <= mu) {
            ans = max(ans, get((int)l, mu));
            l = (ll)mu + 1;
            if (l > r) return ans;
        }
        ll len = r - l + 1;
        if (len >= lambda) return max(ans, maxCycle);
        ll l0 = l - ((ll)mu + 1);
        ll r0 = r - ((ll)mu + 1);
        int start = (int)(l0 % lambda);
        int end = (int)(r0 % lambda);
        int cycleL = mu + 1;
        int cycleR = mu + lambda;
        if (start <= end) {
            ans = max(ans, get(cycleL + start, cycleL + end));
        } else {
            ans = max(ans, get(cycleL + start, cycleR));
            ans = max(ans, get(cycleL, cycleL + end));
        }
        return ans;
    };
    vector<pair<ll, int>> events;
    events.reserve(2LL * n + 1);
    for (int i = 0; i < n; ++i) {
        ll l, r;
        cin >> l >> r;
        events.emplace_back(l, +1);
        if (r + 1 <= m + 1) events.emplace_back(r + 1, -1);
    }
    events.emplace_back(m + 1, 0);
    sort(events.begin(), events.end());
    ll ans = 0;
    ll cur = 0;
    ll prev = 1;
    for (size_t i = 0; i < events.size();) {
        ll pos = events[i].first;
        ll L = prev;
        ll Rr = pos - 1;
        if (L <= Rr && L <= m && Rr >= 1 && cur > 0) {
            L = max(L, 1LL);
            Rr = min(Rr, (ll)m);
            if (L <= Rr) {
                int mx = maxA(L, Rr);
                ans = max(ans, 1LL * cur * mx);
            }
        }
        while (i < events.size() && events[i].first == pos) {
            cur += events[i].second;
            ++i;
        }
        prev = pos;
    }
    cout << ans;
}
