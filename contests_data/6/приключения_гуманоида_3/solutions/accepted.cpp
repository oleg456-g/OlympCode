#include <bits/stdc++.h>

// #pragma GCC optimize("O3,unroll-loops")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#define LF '\n'
#define SP ' '
#define all(x) x.begin(), x.end()
#define rep(i, a, b) for (int i = a; i < (b); i++)
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;

using pii = pair<int, int>;
map<pii, int> dir = {
    {pii{0, +1}, 0},
    {pii{-1, 0}, 1},
    {pii{+1, 0}, 2},
    {pii{0, -1}, 3}
};

vector<int> operator-(const vector<int>& a, const vector<int>& b) {
    assert(a.size() == b.size());
    int n = a.size();
    vector<int> r(n, 0);
    rep(i, 0, n)
        r[i] = a[i] - b[i];

    return r;
}

void solve() {
    string s;
    cin >> s;
    int n, m, k;
    cin >> n >> m >> k;

    vector<vector<int>> cnt(s.size() + 1, vector<int>(4, 0));

    rep(i, 0, s.size()) {
        auto ch = s[i];
        cnt[i + 1] = cnt[i];
        cnt[i + 1][(ch - '2') / 2]++;
    }

    rep(i, k, s.size() + 1) {
        auto v = cnt[i] - cnt[i - k];
        int x = v[2] - v[1];
        int y = v[0] - v[3];
        if (x == n && y == m) {
            cout << "YES" << LF;
            return;
        }
    }

    cout << "NO" << LF;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;
    while (t --> 0) {
        solve();
    }
}
