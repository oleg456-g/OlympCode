#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template <typename T> bool chmin(T& a, T b) { if (a <= b) return false; a = b; return true; }
template <typename T> bool chmax(T& a, T b) { if (a >= b) return false; a = b; return true; }

const ll inf = 1e18;
const int N = (1 << 20) + 2;
ll a, b, A, B;
string s;
ll s10 = 0;

ll dp[2][N];

ll rec(ll val, int pos) { // pos: 0 1
    if (val > s10) {
        return inf;
    }
    if (dp[pos][val] != -1) {
        return dp[pos][val];
    }
    if (val == s10) {
        return dp[pos][val] = pos ? B : A;
    }
    ll nval = (pos ? (val << 1) : (val + 1));
    ll res = inf;
    for (int npos = 0; npos < 2; ++npos) {
        chmin(res, (npos != pos) * (A + B) + rec(nval, npos));
    }
    return dp[pos][val] = (res + (pos ? b : a));
}

int main() {
    cin >> a >> b >> A >> B >> s;
    for (int i = 0; i < s.size(); ++i) {
        s10 = 2 * s10 + s[i] - '0';
    }
    if (s10 == 0) {
        cout << 0 << endl;
    } else {
        for (int i = 0; i < 2; ++i) {
            fill(dp[i], dp[i] + N, -1);
        }

        for (ll i = s10; i > 0; --i) {
            rec(i, 0);
            rec(i, 1);
        }
        cout << rec(0, 0) + A << endl;
    }
}
