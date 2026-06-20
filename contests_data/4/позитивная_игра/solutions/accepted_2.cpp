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
using vll = __int128_t;
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
vector<vector<pair<ll, ll>>> gr;

void get(vector<pair<string, int>>& fs, vector<string>& s, ll k, ll n) {
    vector<ll> lcp(fs.size() - 1);
    for (int i = 0; i < fs.size() - 1; i++) {
        for (int j = 0; j < min(fs[i].first.size(), fs[i + 1].first.size()) && fs[i].first[j] == fs[i + 1].first[j];
             j++) {
            lcp[i] = j + 1;
        }
    }
    for (int i = 0; i < fs.size() - 1; i++) {
        if (fs[i].second < n) {
            ll cur = lcp[i];
            for (ll j = i + 1; j < fs.size() && cur >= k; j++) {
                if (fs[j].second > n) {
                    gr[fs[i].second].push_back({fs[j].second - n, cur});
                }
                if (j != lcp.size() - 1) {
                    cur = min(cur, lcp[j]);
                }
            }
        }
    }
}

struct dsu {
    vector<ll> p;
    dsu(ll n) {
        p.resize(n, -1);
    }
    ll get(ll x) {
        if (p[x] == -1) {
            return x;
        }
        return p[x] = get(p[x]);
    }

    void merge(int x, int y) {
        x = get(x);
        y = get(y);
        p[y] = x;
    }
};
void solve() {
    ll n, m;
    cin >> n >> m;
    vector<array<ll, 3>> hd(m);
    for (int i = 0; i < m; i++) {
        ll a, b, c;
        cin >> a >> b >> c;
        a--;
        b--;
        hd[i] = {c, a, b};
    }
    sort(hd.begin(), hd.end());
    vector<ll> rdp(1ll << n, 1e16);

    for (int j = m - 1; j > -1; j--) {
        auto [c, a, b] = hd[j];
        for (ll ms = 0; ms < (1ll << n); ms++) {
            if ((ms & (1ll << a)) && (ms & (1ll << b))) {
                dsu mach(n);
                ll sum = 0;
                ll col = 0;
                for (int f = j + 1; f < m; f++) {
                    auto [c1, a1, b1] = hd[f];
                    if (((1ll << a1) & ms) && ((1ll << b1) & ms) && mach.get(a1) != mach.get(b1)) {
                        sum += c1;
                        mach.merge(a1, b1);
                        col++;
                    }
                }
                if (col == __builtin_popcountll(ms) - 1) {
                    rdp[ms] = min(rdp[ms], sum);
                }
            }
        }
    }
    ll ans = 1e16;
    for (ll ms = 1; ms < (1ll << n); ms++) {
        dsu mach(n);
        ll sum = rdp[ms];
        ll col = 0;
        vector<ll> ss;
        for (int j = 0; j < n; j++) {
            if ((1ll << j) & ms) {
                ss.push_back(j);
            }
        }
        for (int j = 1; j < ss.size(); j++) {
            mach.p[ss[j]] = ss[0];
        }
        for (int f = 0; f < m; f++) {
            auto [c1, a1, b1] = hd[f];
            if (mach.get(a1) != mach.get(b1)) {
                sum += c1;
                mach.merge(a1, b1);
                col++;
            }
        }
        if (col + ss.size() - 1 == n - 1) {
            ans = min(ans, sum);
        }
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