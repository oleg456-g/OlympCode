#include<bits/stdc++.h>
using namespace std;

using ll = long long;

const int R = 1 << 18;

int t_block[R * 2], t_tail[R * 2];

void build(const vector<int>& tail, const vector<int>& block) {
    for (int i = 0; i < tail.size(); ++i) {
        t_tail[i + R] = tail[i];
    }
    for (int i = 0; i < block.size(); ++i) {
        t_block[i + R] = block[i];
    }
    for (int i = R - 1; i > 0; --i) {
        t_block[i] = max(t_block[i * 2], t_block[i * 2 + 1]);
        t_tail[i] = max(t_tail[i * 2], t_tail[i * 2 + 1]);
    }
}

int get_block(int ql, int qr, const vector<int>& block) {
    if (ql > qr) {
        return 0;
    }
    int ans = 0;
    for (int i = ql; i <= qr; ++i) {
        ans = max(ans, block[i - 1]);
    }
    return ans;
}

int get_tail(int ql, int qr, const vector<int>& tail) {
    if (ql > qr) {
        return 0;
    }
    int ans = 0;
    for (int i = ql; i <= qr; ++i) {
        ans = max(ans, tail[i - 1]);
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
    vector<int> tail, block;
    auto nxt_v = [&](int v) -> int {
        return (1LL * v * b + c) % mod;
    };
    auto build_block_tail = [&]() -> void {
        vector<int> was(mod, false);
        was[x] = 1;
        vector<int> a = {x};
        for (int i = 2;; ++i) {
            int nw = nxt_v(a.back());
            if (was[nw]) {
                for (int j = 0; j < was[nw] - 1; ++j) {
                    tail.push_back(a[j]);
                }
                for (int j = was[nw] - 1; j < a.size(); ++j) {
                    block.push_back(a[j]);
                }
                if (block.empty()) {
                    swap(tail, block);
                }
                return;
            }
            a.push_back(nw);
            was[nw] = a.size();
        }
    };
    build_block_tail();
    build(tail, block);
    auto get_qu = [&](int l, int r) -> int {
        int ans = 0;
        if (l > r) return ans;
        if (l <= tail.size()) {
            ans = max(ans, get_tail(l, min((int)tail.size(), r), tail));
            l = tail.size() + 1;
            if (l > r) return ans;
        }
        if (r - l + 1 >= block.size()) {
            ans = max(ans, get_block(1, block.size(), block));
            return ans;
        }
        int ql = (l - tail.size() - 1) % block.size() + 1;
        int qr = (r - tail.size() - 1) % block.size() + 1;
        if (ql <= qr) {
            ans = max(ans, get_block(ql, qr, block));
        }
        else {
            ans = max(ans, get_block(ql, (int)block.size(), block));
            ans = max(ans, get_block(1, qr, block));
        }
        return ans;
    };
    ll ans = 0;
    sort(evs.begin(), evs.end());
    int bal = 0, lst = INT_MAX / 3;
    for (auto& [x, event] : evs) {
        if (event == 1) {
            ans = max(ans, 1LL * get_qu(lst, x) * bal);
        }
        else {
            ans = max(ans, 1LL * get_qu(lst, x - 1) * bal);
            lst = x;
        }
        bal -= event;
    }
    cout << ans;
}