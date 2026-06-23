#include <bits/stdc++.h>

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#define LF '\n'
#define SP ' '
#define all(x) x.begin(), x.end()
#define rep(i, a, b) for (int i = a; i < (b); i++)
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;

constexpr int MAX_BOUND = 1000;

using pii = pair<int, int>;
pair<int, int> walk(string_view s) {
    int x = 0;
    int y = 0;
    for (auto ch : s) {
        if (ch == '2') y++;
        else if (ch == '4') x--;
        else if (ch == '6') x++;
        else if (ch == '8') y--;
        else throw;
    }
    return pii{x, y};
}

void solve() {
    string s;
    cin >> s;

    int n, m, k;
    cin >> n >> m >> k;

    int kk = k - (abs(n) + abs(m));
    // cout << "kk: " << kk << LF;
    if (kk < 0 || kk % 2 != 0) {
        // cout << "case 2" << LF;
        cout << "NO" << LF;
        return;
    }

    if (s.size() <= MAX_BOUND) {
        // try substring[i;i+k]
        string_view sv = s;
        for (int i = 0; i + k <= s.size(); i++) {
            auto [x, y] = walk(sv.substr(i, k));
            if (x == n && y == m) {
                cout << "YES" << LF;
                return;
            }
        }
        cout << "NO" << LF;
    } else {
        // random walk travels on average O(sqrt(t)) from 0

        constexpr int MAGIC_CONST = 5;
        if (abs(n) + abs(m) > MAGIC_CONST * sqrtl(ld{s.size()})) {
            // cout << "case 1" << LF;
            cout << "NO" << LF;
            return;
        }
        cout << "YES" << LF;
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cerr.tie(0);

    int t;
    cin >> t;
    while (t --> 0) {
        solve();
    }
}
