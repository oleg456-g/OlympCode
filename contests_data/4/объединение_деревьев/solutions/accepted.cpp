// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")
// #pragma GCC optimize("trapv")
// I love Olya. She is sthe best girl)♡.
// .#...#.
// #.#.#.#
// #..#..#
// .#...#.
// ..#.#..
// ...#...

#include <bits/stdc++.h>
#include <cmath>
#include <ext/pb_ds/assoc_container.hpp>  // Общий файл.
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using ll = long long;
using ld = long double;
// using ull = unsigned ll;
using u32 = unsigned ll;
using vll = __int128;
using namespace __gnu_pbds;
typedef tree<pair<ll, ll>, null_type, less<pair<ll, ll>>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;

template <typename T>
istream& operator>>(istream& in, vector<T>& v) {
    for (auto& el : v) {
        in >> el;
    }
    return in;
}

template <typename T>
ostream& operator<<(ostream& out, vector<T>& v) {
    for (auto el : v) {
        out << el << " ";
    }
    return out;
}
mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());
const ll mod = 998244353;

ll binpow(ll x, ll step) {
    if (step == 0) {
        return 1ll;
    }
    ll ans = binpow(x, step / 2);
    ans = (ans * ans) % mod;
    if (step % 2) {
        return ans * x % mod;
    }
    return ans;
}

array<ll, 30> dfs(int v, vector<vector<ll>>& gr, vector<ll>& prx, vector<ll>& w, ll cur, ll& cnt) {
    cur = cur ^ w[v];
    prx[v] = cur;
    array<ll, 30> res;
    res.fill(0);
    for (auto el : gr[v]) {
        auto c = dfs(el, gr, prx, w, cur, cnt);
        for (int i = 0; i < 30; i++) {
            res[i] += c[i];
        }
    }
    if (gr[v].empty()) {
        cnt++;
        for (int j = 0; j < 30; j++) {
            res[j] += bool((1ll << j) & cur);
        }
    }
    return res;
}

void solve() {
    ll n1, n2;
    cin >> n1;
    vector<vector<ll>> gr1(n1), gr2;
    ll r1, r2;
    vector<ll> prx1(n1), prx2, w1(n1), w2;
    cin >> w1;
    for (int i = 0; i < n1; i++) {
        ll p;
        cin >> p;
        p--;
        if (p == i) {
            r1 = i;
        } else {
            gr1[p].push_back(i);
        }
    }
    ll cnt1, cnt2;
    cnt1 = cnt2 = 0;
    auto res1 = dfs(r1, gr1, prx1, w1, 0, cnt1);
    ll ans = 0;
    cin >> n2;
    w2.resize(n2);
    cin >> w2;
    prx2.resize(n2);
    gr2.resize(n2);
    for (int i = 0; i < n2; i++) {
        ll p;
        cin >> p;
        p--;
        if (p == i) {
            r2 = i;
        } else {
            gr2[p].push_back(i);
        }
    }
    auto res2 = dfs(r2, gr2, prx2, w2, 0, cnt2);
    for (int i = 0; i < n1; i++) {
        ll tr = 0;
        for (ll j = 0; j < 30; j++) {
            tr += (1ll << j) * res1[j];
            if (prx1[i] & (1ll << j)) {
                tr += (1ll << j) * (cnt2 - res2[j]);
            } else {
                tr += (1ll << j) * (res2[j]);
            }
        }
        if (gr1[i].empty()) {
            tr -= prx1[i];
        }
        ans = max(ans, tr);
    }
    for (int i = 0; i < n2; i++) {
        ll tr = 0;
        for (ll j = 0; j < 30; j++) {
            tr += (1ll << j) * res2[j];
            if (prx2[i] & (1ll << j)) {
                tr += (1ll << j) * (cnt1 - res1[j]);
            } else {
                tr += (1ll << j) * (res1[j]);
            }
        }
        if (gr2[i].empty()) {
            tr -= prx2[i];
        }
        ans = max(ans, tr);
    }
    cout << ans << "\n";
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll t = 1;  //
    cin >> t;
    while (t--) {
        solve();
    }
}
