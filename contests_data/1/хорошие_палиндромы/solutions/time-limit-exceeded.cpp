#include <bits/stdc++.h>
using namespace std;

bool isPalindrome(const string& s) {
    int n = (int)s.size();
    for (int i = 0; i < n / 2; ++i)
        if (s[i] != s[n - 1 - i]) return false;
    return true;
}

int beauty(const string& orig) {
    if (isPalindrome(orig)) return 0;
    string cur = orig;
    int steps = 0;
    while (true) {
        int n = (int)cur.size();
        vector<bool> del(n, false);
        for (char c : string("ab")) {
            int first = -1, last = -1;
            for (int i = 0; i < n; ++i)
                if (cur[i] == c) { first = i; break; }
            for (int i = n - 1; i >= 0; --i)
                if (cur[i] == c) { last = i; break; }
            if (first != -1) {
                del[first] = true;
                if (last != first) del[last] = true;
            }
        }
        string nxt;
        nxt.reserve(n);
        for (int i = 0; i < n; ++i)
            if (!del[i]) nxt.push_back(cur[i]);
        ++steps;
        if (isPalindrome(nxt)) return steps;
        cur.swap(nxt);
    }
    return steps;               // never reached
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string t;
    if (!(cin >> t)) return 0;
    int q;
    cin >> q;
    while (q--) {
        char type;
        cin >> type;
        if (type == '!') {
            int i; cin >> i; --i;
            t[i] = (t[i] == 'a' ? 'b' : 'a');
        } else {                // '?'
            int l, r; cin >> l >> r; --l; --r;
            string sub = t.substr(l, r - l + 1);
            cout << beauty(sub) << '\n';
        }
    }
    return 0;
}
