#include "bits/stdc++.h"
using namespace std;
 
#define int long long
#define all(x) x.begin(), x.end()

bool is_palind(string &s) {
    int n = (int) s.size();
    for (int i = 0; i < n / 2; i++) {
        if (s[i] != s[n - 1 - i]) {
            return false;
        }
    }
    return true;
}

int get_ans(string s) {
    int cnt = 0;
    while (!is_palind(s)) {
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == 'a') {
                s.erase(i, 1);
                break;
            }
        }
        for (int i = s.size() - 1; i >= 0; i--) {
            if (s[i] == 'a') {
                s.erase(i, 1);
                break;
            }
        }
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == 'b') {
                s.erase(i, 1);
                break;
            }
        }
        for (int i = s.size() - 1; i >= 0; i--) {
            if (s[i] == 'b') {
                s.erase(i, 1);
                break;
            }
        }
        cnt++;
    }
    return cnt;
}

void solve() {
    string s;
    cin >> s;
    int q;
    cin >> q;
    while (q--) {
        char type;
        cin >> type;
        if (type == '!') {
            int i;
            cin >> i;
            i--;
            s[i] = (s[i] == 'a' ? 'b' : 'a');
        } else {
            int l, r;
            cin >> l >> r;
            l--;
            r--;
            cout << get_ans(s.substr(l, r - l + 1)) << '\n';
        }
    }
}
 
signed main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    //cin >> T;
    while (T--) {
        solve();
    }
    return 0;
}
