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
const ll mod = 1e9 + 7;

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
        if (fs[i].second >= n) {
            ll cur = lcp[i];
            for (ll j = i + 1; j < fs.size() && cur >= k; j++) {
                if (fs[j].second < n) {
                    gr[fs[i].second - n].push_back({fs[j].second, cur});
                }
                if (j != lcp.size() - 1) {
                    cur = min(cur, lcp[j]);
                }
            }
        }
    }
}
void solve() {
    gr.clear();
    ll n, k;
    cin >> n >> k;
    vector<string> s(n);
    cin >> s;
    gr.resize(n);
    unordered_map<ll, ll> our_id;
    for (int i = 0; i < n; i++) {
        s[i].push_back('a' + 26);
        ll ph = 0;
        ll h1 = 1;
        ll ph2 = 0;
        ll h2 = 1;
        for (int c = 0; c < s[i].size(); c++) {
            ph += (s[i][c] - 'a' + 1) * h1;
            ph %= mod;
            h1 = (h1 * 29) % mod;
            ph2 += (s[i][c] - 'a' + 1) * h2;
            ph2 %= mod;
            h2 = (h2 * 31) % mod;
            if ((c + 1) >= k) {
                if (our_id.find(ph + mod * ph2) == our_id.end()) {
                    our_id[ph + mod * ph2] = gr.size();
                    gr.push_back({});
                }
                gr[our_id[ph + mod * ph2]].push_back({i, c});
            }
        }
        s[i].pop_back();
    }
    for (int i = 0; i < n; i++) {
        ll ph = 0;
        ll h1 = 1;
        ll ph2 = 0;
        ll h2 = 1;
        for (int c = s[i].size() - 1; c > -1; c--) {
            ph = ph * 29 + (s[i][c] - 'a' + 1);
            ph %= mod;
            h1 = (h1 * 29) % mod;
            ph2 = ph2 * 31 + (s[i][c] - 'a' + 1);
            ph2 %= mod;
            h2 = (h2 * 31) % mod;
            if ((s[i].size() - c) >= k) {
                ll nph = ph;
                ll nph2 = ph2;
                if (c != 0) {
                    nph = (ph * 29 + (s[i][c - 1] - 'a' + 1)) % mod;
                    nph2 = (ph2 * 31 + (s[i][c - 1] - 'a' + 1)) % mod;
                }
                for (int j = 1; j <= 27; j++) {
                    ll tph = (ph + h1 * j) % mod;
                    ll tph2 = (ph2 + h2 * j) % mod;
                    if (tph + mod * tph2 != nph + mod * nph2 && our_id.find(tph + mod * tph2) != our_id.end()) {
                        gr[i].push_back({our_id[tph + mod * tph2], 0});
                    }
                }
            }
        }
    }
    vector<ll> dst(gr.size(), 1e16);
    set<pair<ll, ll>> hd;
    dst[0] = 0;
    hd.insert({0, 0});
    while (!hd.empty()) {
        auto [d, v] = *hd.begin();
        hd.erase(hd.begin());
        for (auto [u, d2] : gr[v]) {
            if (dst[u] > dst[v] + d2) {
                hd.erase({dst[u], u});
                dst[u] = dst[v] + d2;
                hd.insert({dst[u], u});
            }
        }
    }
    for (int i = 1; i < n; i++) {
        if (dst[i] == 1e16) {
            cout << -1 << " ";
        } else {
            cout << dst[i] << " ";
        }
    }
    cout << "\n";
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