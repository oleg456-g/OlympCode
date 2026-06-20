#include <bits/stdc++.h>

using namespace std;

using ll = long long;
using ld = long double;

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()
#define rep(i, n) for (int i = 0; i < (n); i += 1)
#define len(a) ((int)(a).size())

int n;
ll h, k, s;
vector<pair<ll, ll>> segs;
vector<unordered_map<int, bool>> mem;

bool solve(int i, ll pos) {
    if (i == n - 1) {
        return true;
    }
    auto it = mem[i].find(pos);
    if(it != mem[i].end()){
        return it->second;
    }
    if (segs[i + 1].first < pos - 1 and pos - 1 < segs[i + 1].second) {
        if (solve(i + 1, pos - 1)) {
            return mem[i][pos] = true;
        }
    }
    if (segs[i + 1].first < pos + k and pos + k < segs[i + 1].second) {
        if (solve(i + 1, pos + k)) {
            return mem[i][pos] = true;
        }
    }
    return mem[i][pos] = false;
}

int32_t main() {
    if (1) {
        ios::sync_with_stdio(0);
        cin.tie(0);
        cout.tie(0);
    }
    int t;
    cin >> t;
    rep(itr, t) {
        cin >> n >> h >> k >> s;
        segs.resize(n);
        mem.assign(n, {});
        for (auto &[l, r] : segs) {
            cin >> l >> r;
        }
        if (solve(0, s)) {
            cout << "Yes" << "\n";
        } else {
            cout << "No" << "\n";
        }
    }
    return 0;
}
