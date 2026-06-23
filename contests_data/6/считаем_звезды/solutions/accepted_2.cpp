#include<bits/stdc++.h>
using namespace std;
#define pb  push_back
#define ll  long long
#define vi  vector<int>
#define vvi vector<vi >
#define all(x) x.begin(), x.end()
 
ll m = 25000;
ll n, q, x, y, z, r;
vvi p[40][40][40];
 
ll sq(ll x) { return x * x; }
 
ll f(vvi& v) {
    ll res = 0;
    for (auto& w : v)
        res += (sq(w[0] - x) + sq(w[1] - y) + sq(w[2] - z) < sq(r));
    return res;
}
 
int main() {
    ios_base::sync_with_stdio(false); cin.tie(0);
    cin >> n >> q;
    while (n--) {
        cin >> x >> y >> z; --x; --y; --z;
        p[x / m][y / m][z / m].pb({x, y, z});
    }
    ll ii, II, jj, JJ, kk, KK;
    while (q--) {
        cin >> x >> y >> z >> r; --x; --y; --z;
        ii = max(0ll, x - r + 1) / m;
        II = min(999999ll, x + r - 1) / m;
        jj = max(0ll, y - r + 1) / m;
        JJ = min(999999ll, y + r - 1) / m;
        kk = max(0ll, z - r + 1) / m;
        KK = min(999999ll, z + r - 1) / m;
        ll res = 0;
        for (ll i = ii; i <= II; ++i)
            for (ll j = jj; j <= JJ; ++j)
                for (ll k = kk; k <= KK; ++k)
                    res += f(p[i][j][k]);
        cout << res << endl;
    }
 
    return 0;
}