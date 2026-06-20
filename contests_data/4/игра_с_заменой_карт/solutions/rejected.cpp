#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
typedef unsigned long long ull;

#define F first
#define S second
// #define endl '\n'

mt19937 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

uniform_int_distribution<int> uid(0, 10000000);

int rnd(int n) {
    return uid(rng) % n;
}

double rndd() { return double(rand()) / RAND_MAX; }

void solve() {
    ll n, m;
    cin >> n >> m;
    vector<ll> v(n);
    for (auto &el: v)
        cin >> el;
    string s;
    cin >> s;
    ll cnt0 = 0;
    for (auto &el: s)
        cnt0 += el == '0';
    ll sum = 0;
    for (auto &el: v)
        sum += el;
    sum %= m;
    if (s[sum] == '1' || (s[sum] == '0' && cnt0 == 1)) {
        cout << "Alice" << endl;
        while (v.size() > 1) {
            cout << "unite " << v[v.size() - 2] << ' ' << v[v.size() - 1] << endl;
            v[v.size() - 2] += v[v.size() - 1];
            v[v.size() - 2] %= m;
            v.pop_back();
        }
        if (s[sum] == '0') {
            cout << "replace " << v.back() << endl;
            ll x;
            cin >> x;
        }
        cout << "done" << endl;
    } else {
        cout << "Bob" << endl;
        ll w1 = sum, w2 = 0;
        while (s[w2] == '1' || w2 == w1)
            w2++;
        cerr << w1 << ' ' << w2 << endl;
        while (true) {
            string c;
            cin >> c;
            if (c == "done")
                return;
            if (c == "unite") {
                ll x, y;
                cin >> x >> y;
            } else if (c == "replace") {
                ll x;
                cin >> x;
                if (sum == w1) {
                    cout << (x + w2 - w1 + m) % m << endl;
                } else {
                    cout << (x + w1 - w2 + m) % m << endl;
                }
            }
        }
    }
}

signed main() {
    // ios_base::sync_with_stdio(false);
    // cin.tie(nullptr);
    // cout.tie(nullptr);

    auto start_time = chrono::high_resolution_clock::now();

    int tt = 1;
    cin >> tt;
    for (int _ = 0; _ < tt; _++)
        solve();

    auto end_time = chrono::high_resolution_clock::now();
    cerr << "Execution time: " <<
            chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;
}

