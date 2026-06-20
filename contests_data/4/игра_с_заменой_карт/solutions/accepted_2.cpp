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
    ll n, m;
    cin >> n >> m;
    vector<ll> a(n);
    cin >> a;
    string s;
    cin >> s;
    ll sum = 0;
    for (auto el : a) {
        sum += el;
        sum %= m;
    }
    ll f = -1;
    for (int i = 0; i < m; i++) {
        if (s[i] == '0' && i != sum) {
            f = i;
        }
    }
    if (s[sum] == '1' || f == -1) {
        cout << "Alice" << endl;
        if (s[sum] == '0') {
            cout << "replace " << a[0] << endl;
            ll nw;
            cin >> nw;

            ll s2 = nw;
            for (int i = 1; i < n; i++) {
                cout << "unite " << s2 << " " << a[i] << endl;
                s2 = (s2 + a[i]) % m;
            }
            cout << "done" << endl;
        } else {
            ll s2 = a[0];
            for (int i = 1; i < n; i++) {
                cout << "unite " << s2 << " " << a[i] << endl;
                s2 = (s2 + a[i]) % m;
            }
            cout << "done" << endl;
        }
    } else {
        cout << "Bob" << endl;

        string c;
        cin >> c;
        while (c != "done") {

            ll fp;
            cin >> fp;
            if (c != "unite") {
                cout << (f - sum + fp + m) % m << endl;
                swap(f, sum);
            } else {
                cin >> fp;
            }
            cin >> c;
        }
    }
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