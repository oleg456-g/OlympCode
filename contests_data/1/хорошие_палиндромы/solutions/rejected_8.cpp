#include "bits/stdc++.h"
using namespace std;
 
#define int long long
#define all(x) x.begin(), x.end()
 
bool is_palind(string &s, int l, int r) {
    for (int i = 0; i < (r - l + 1) / 2; i++) {
        if (s[l + i] != s[r - i]) {
            return false;
        }
    }
    return true;
}
 
pair<int, int> get_seg(string &s, int l, int r, int k, char sym) {
    int cnt = 0;
    int x = r;
    int y = l;
    for (int i = l; i <= r; i++) {
        if (s[i] == sym) {
            cnt++;
        }
        if (cnt == k) {
            x = i;
            break;
        }
    }
    cnt = 0;
    for (int i = r; i >= l; i--) {
        if (s[i] == sym) {
            cnt++;
        }
        if (cnt == k) {
            y = i;
            break;
        }
    }
    return {x, y};
}
 
bool check(string &s, int l, int r, int k) {
    auto [x1, y1] = get_seg(s, l, r, k + 1, 'a');
    if (y1 < x1) {
        return true;
    }
    auto [x2, y2] = get_seg(s, l, r, k + 1, 'b');
    if (y2 < x2) {
        return true;
    }
    if (x1 >= x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    if (y1 < y2) {
        return false;
    }
    if (x2 - l != r - y2) {
        return false;
    }
    return is_palind(s, x2, y2);
}
 
int ans(string &s, int l, int r) {
    int low = -1;
    int high = s.size();
    while (high - low > 1) {
        int m = (low + high) / 2;
        if (check(s, l, r, m)) {
            high = m;
        } else {
            low = m;
        }
    }
    return high;
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
            cout << ans(s, l, r) << '\n';
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
