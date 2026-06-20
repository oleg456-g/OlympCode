#include "bits/stdc++.h"
using namespace std;

#define all(x) x.begin(), x.end()
#define ll long long

const int MAXN = 500'010;
const int MOD = 1'000'000'007;
const int H = 37;

struct Mint {
    int val = 0;
    
    Mint() = default;
    
    Mint(int _val): val(_val) {}
    
    Mint operator+(Mint other) {
        other.val = (other.val + val >= MOD ? other.val + val - MOD : other.val + val);
        return other;
    }
    
    Mint operator-(Mint other) {
        other.val = (val - other.val < 0 ? val - other.val + MOD : val - other.val);
        return other;
    }
    
    Mint operator*(Mint other) {
        return (ll) val * (ll) other.val % MOD;
    }
};

Mint pows[MAXN];

struct Hasher {
    Mint f[MAXN];
    
    void update(int i, Mint val) {
        i++;
        for (; i < MAXN; i += i & -i) {
            f[i] = f[i] + val;
        }
    }
    
    Mint seg(int l, int r) {
        return pref(r) - pref(l - 1);
    }
    
    Mint pref(int i) {
        i++;
        Mint res = 0;
        for (; i > 0; i -= i & -i) {
            res = res + f[i];
        }
        return res;
    }
};

struct Palindromer {
    Hasher hasher;
    Hasher rev_hasher;
    string s;
    int n;
    
    Palindromer(string _s): s(_s), n(s.size()) {
        int n = (int) s.size();
        for (int i = 0; i < n; i++) {
            if (s[i] == 'b') {
                hasher.update(i, pows[i]);
            }
        }
        for (int i = n - 1; i >= 0; i--) {
            if (s[i] == 'b') {
                rev_hasher.update(n - i - 1, pows[n - i - 1]);
            }
        }
    }
    
    void update(int i) {
        if (s[i] == 'a') {
            hasher.update(i, pows[i]);
            rev_hasher.update(n - i - 1, pows[n - i - 1]);
            s[i] = 'b';
        } else {
            hasher.update(i, Mint(MOD) - pows[i]);
            rev_hasher.update(n - i - 1, Mint(MOD) - pows[n - i - 1]);
            s[i] = 'a';
        }
    }
    
    bool is_palindrome(int l, int r) {
        Mint x = hasher.seg(l, r) * pows[n - r - 1];
        Mint y = rev_hasher.seg(n - 1 - r, n - 1 - l) * pows[l];
        return x.val == y.val;
    }
};
 
pair<int, int> get_seg(Palindromer& palindromer, int l, int r, int k, char sym) {
    string& s = palindromer.s;
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
 
bool check(Palindromer& palindromer, int l, int r, int k) {
    auto [x1, y1] = get_seg(palindromer, l, r, k + 1, 'a');
    if (y1 < x1) {
        return true;
    }
    auto [x2, y2] = get_seg(palindromer, l, r, k + 1, 'b');
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
    return palindromer.is_palindrome(x2, y2);
}
 
int ans(Palindromer& palindromer, int l, int r) {
    int low = -1;
    int high = r - l + 1;
    while (high - low > 1) {
        int m = (low + high) / 2;
        if (check(palindromer, l, r, m)) {
            high = m;
        } else {
            low = m;
        }
    }
    return high;
}
 
void solve() {
    pows[0] = 1;
    for (int i = 1; i < MAXN; i++) {
        pows[i] = pows[i - 1] * H;
    }
    string s;
    cin >> s;
    Palindromer palindromer(s);
    
    int q;
    cin >> q;
    while (q--) {
        char type;
        cin >> type;
        if (type == '!') {
            int i;
            cin >> i;
            i--;
            palindromer.update(i);
        } else {
            int l, r;
            cin >> l >> r;
            l--;
            r--;
            //cout << s.substr(l, r - l + 1) << endl;
            cout << ans(palindromer, l, r) << '\n';
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
