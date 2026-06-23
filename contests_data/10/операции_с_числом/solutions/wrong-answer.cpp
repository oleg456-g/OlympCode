#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template <typename T> bool chmin(T& a, T b) { if (a <= b) return false; a = b; return true; }
template <typename T> bool chmax(T& a, T b) { if (a >= b) return false; a = b; return true; }

const ll inf = 1e18;


int main() {
    ll a, b, A, B;
    string s;
    cin >> a >> b >> A >> B >> s;
    int n = s.size();
    
    if (n == 1 && s[0] == '0') {
        cout << 0 << endl;
        return 0;
    }

    ll s10 = 0;
    ll res = 0;
    for (int i = 0; i < n; ++i) {
        ll ns10 = s10 * 2 + (s[i] - '0');
        ll r1 = (ns10 - s10) * a;
        ll r2 = b + a * (s[i] - '0');
        if (s10 * a < 1e18 && r1 <= r2) {
            res += r1;
            s10 = ns10;
        } else {
            res += r2;
        }
    }
    cout << res << endl;
}
