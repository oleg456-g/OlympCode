#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template <typename T> bool chmin(T& a, T b) { if (a <= b) return false; a = b; return true; }
template <typename T> bool chmax(T& a, T b) { if (a >= b) return false; a = b; return true; }

const ll inf = 1e18;
ll a, b, A, B;
string s;

ll greed() {
    ll s10 = 1;
    int i;
    for (i = 1; i < s.size(); ++i) {
        ll ns10 = 2 * s10 + (s[i] - '0');
        ll pot = A + B + b + (s[i] == '1' ? A + B + a : 0ll);
        if ((ns10 - s10) * a > pot) {
            break;
        }
        s10 = ns10;
    }
    ll res = A + s10 * a;
    int pos = 0;
    for (; i < s.size(); ++i) {
        res += (pos == 0 ? A + B : 0ll) + b;
        pos = 1;
        if (s[i] == '1') {
            pos = 0;
            res += A + B + a;
        }
    }
    res += (pos == 0 ? A : B);
    return res;
}

int main() {
    cin >> a >> b >> A >> B >> s;
    if (s.size() == 1 && s[0] == '0') {
        cout << 0 << '\n';
        return 0;
    }

    cout << greed() << '\n';
}
