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
 
struct Segtree {
    vector<int> tree;
    int size = 1;
    
    void init(string s) {
        int n = (int) s.size();
        while (size < n) {
            size <<= 1;
        }
        tree.resize(2 * size);
        for (int i = 0; i < n; i++) {
            if (s[i] == 'b') {
                update(i, 1);
            }
        }
    }
    
    void update(int i, int val) {
        i += size - 1;
        tree[i] = val;
        while (i > 0) {
            i = (i - 1) / 2;
            tree[i] = tree[2 * i + 1] + tree[2 * i + 2];
        }
    }
    
    int sum(int l, int r, int x, int lx, int rx) {
        if (l <= lx && rx <= r) {
            return tree[x];
        }
        if (r <= lx || rx <= l) {
            return 0;
        }
        int m = (lx + rx) / 2;
        return sum(l, r, 2 * x + 1, lx, m) + sum(l, r, 2 * x + 2, m, rx);
    }
    
    int sum(int l, int r) {
        return sum(l, r + 1, 0, 0, size);
    }
    
    int get1(int k, int x, int lx, int rx) {
        if (rx - lx == 1) {
            return lx;
        }
        int m = (lx + rx) / 2;
        if (k <= tree[2 * x + 1]) {
            return get1(k, 2 * x + 1, lx, m);
        } else {
            return get1(k - tree[2 * x + 1], 2 * x + 2, m, rx);
        }
    }
    
    int get1(int l, int k) {
        k += sum(0, l - 1);
        return get1(k, 0, 0, size);
    }
    
    int get0(int k, int x, int lx, int rx) {
        if (rx - lx == 1) {
            return lx;
        }
        int m = (lx + rx) / 2;
        int lsum = m - lx - tree[2 * x + 1];
        if (k <= lsum) {
            return get0(k, 2 * x + 1, lx, m);
        } else {
            return get0(k - lsum, 2 * x + 2, m, rx);
        }
    }
    
    int get0(int l, int k) {
        int lsum = l - sum(0, l - 1);
        k += lsum;
        return get0(k, 0, 0, size);
    }
};
 
pair<int, int> get_seg(Segtree& tree, int l, int r, int k, char sym) {
    int x = r;
    int y = l;
    if (sym == 'b') {
        x = tree.get1(l, k);
        int sm = tree.sum(l, r);
        y = tree.get1(l, sm - k + 1);
    } else {
        x = tree.get0(l, k);
        int sm = r - l + 1 - tree.sum(l, r);
        y = tree.get0(l, sm - k + 1);
    }
    return {x, y};
}
 
bool check(Palindromer& palindromer, Segtree& tree, int l, int r, int k) {
    auto [x1, y1] = get_seg(tree, l, r, k + 1, 'a');
    if (y1 < x1) {
        return true;
    }
    auto [x2, y2] = get_seg(tree, l, r, k + 1, 'b');
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
 
int ans(Palindromer& palindromer, Segtree& tree, int l, int r) {
    int low = -1;
    int high = r - l + 1;
    while (high - low > 1) {
        int m = (low + high) / 2;
        if (check(palindromer, tree, l, r, m)) {
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
    Segtree tree;
    tree.init(s);
    int q;
    cin >> q;
    if (s.size() > 100 || q > 100) {
        cout << "fjkdsja";
        return;
    }
    while (q--) {
        char type;
        cin >> type;
        if (type == '!') {
            int i;
            cin >> i;
            i--;
            palindromer.update(i);
            if (s[i] == 'a') {
                tree.update(i, 1);
                s[i] = 'b';
            } else {
                tree.update(i, 0);
                s[i] = 'a';
            }
        } else {
            int l, r;
            cin >> l >> r;
            l--;
            r--;
            //cout << s.substr(l, r - l + 1) << endl;
            cout << ans(palindromer, tree, l, r) << '\n';
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
