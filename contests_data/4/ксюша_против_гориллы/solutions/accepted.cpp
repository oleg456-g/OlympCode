#include <bits/stdc++.h>
#pragma GCC optimize("O3")
using namespace std;
typedef long long ll;
typedef long double ld;
typedef unsigned long long ull;

#define F first
#define S second
#define endl '\n'

mt19937 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

uniform_int_distribution<int> uid(0, 10000000);

int rnd(int n) {
    return uid(rng) % n;
}

double rndd() { return double(rand()) / RAND_MAX; }

void solve() {
    ll n;
    cin >> n;
    map <ll, ll> mp;
    for (ll i = 0; i < 2 * n; i++) {
        ll x;
        cin >> x;
        mp[x]++;
    }
    ll sum = 0;
    while (!mp.empty()) {
        cout << mp.begin()->F << ' ';
        sum += mp.begin()->F;
        mp[mp.begin()->F]--;
        if (mp.begin()->S == 0)
            mp.erase(mp.begin());
        assert(mp.find(sum) != mp.end());
        mp[sum]--;
        if (mp[sum] == 0)
            mp.erase(sum);
    }
    cout << endl;
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    auto start_time = chrono::high_resolution_clock::now();

    int tt = 1;
    cin >> tt;
    for (int _ = 0; _ < tt; _++)
        solve();

    auto end_time = chrono::high_resolution_clock::now();
    cerr << "Execution time: " <<
            chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;
}
