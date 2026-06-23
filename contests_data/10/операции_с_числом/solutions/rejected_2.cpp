#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template <typename T> bool chmin(T& a, T b) { if (a <= b) return false; a = b; return true; }
template <typename T> bool chmax(T& a, T b) { if (a >= b) return false; a = b; return true; }

const ll inf = 1e18;

ll a, b, A, B;
string s;
ll s10 = 0;

ll rec(ll val, int pos) { // pos: 0 1
    if (val > s10) {
            return inf;
    }
    if (val == s10) {
        return pos ? B : A;
    }
    ll nval = (pos ? (val << 1) : (val + 1));
    ll res = inf;
    for (int npos = 0; npos < 2; ++npos) {
        chmin(res, (npos != pos) * (A + B) + rec(nval, npos));
    }
    return res + (pos ? b : a);
}

int main() {
    cin >> a >> b >> A >> B >> s;
    for (int i = 0; i < s.size(); ++i) {
        s10 = 2 * s10 + s[i] - '0';
    }
    if (s10 == 0) {
        cout << 0 << endl;
    } else {
        cout << rec(0, 0) + A << endl;
    }
}
