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
void solve() {
    ll n;
    cin >> n;
    vector<ll> b(2 * n);
    cin >> b;
    vector<ll> a(n);
    sort(b.begin(), b.end());
    vector<ll> prefs(n, -1);
    prefs[0] = a[0] = b[0];
    ll yk = 1;
    ll yk2 = 1;
    for (int i = 2; i < 2 * n; i++) {
        if (prefs[yk] == -1 || prefs[yk] > b[i]) {
            a[yk2] = b[i];
            prefs[yk2] = prefs[yk2 - 1] + a[yk2];
            yk2++;
        } else {
            yk++;
        }
    }
    cout << a << "\n";
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