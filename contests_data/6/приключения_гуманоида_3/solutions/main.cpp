#include <bits/stdc++.h>

using namespace std;

int n, m, k;
string s;

inline void adv(char c, int &x, int &y) {
    switch (c) {
        case '2': y++; break;
        case '4': x--; break;
        case '6': x++; break;
        case '8': y--; break;
    }
}

inline void dev(char c, int &x, int &y) {
    switch (c) {
        case '2': y--; break;
        case '4': x++; break;
        case '6': x--; break;
        case '8': y++; break;
    }
}

bool solve() {
    int ss = s.size();
    int x = 0, y = 0;
    for (int i = 0; i < k; ++i)
        adv(s[i], x, y);
    if (x == n && y == m) return true;
    for (int i = k; i < ss; ++i) {
        dev(s[i - k], x, y);
        adv(s[i], x, y);
        if (x == n && y == m) return true;
    }
    return false;
}

int main() {
    int t; cin >> t;
    while (t--) {
        cin >> s >> n >> m >> k;
        cout << (solve() ? "YES" : "NO") << endl;
    }
}