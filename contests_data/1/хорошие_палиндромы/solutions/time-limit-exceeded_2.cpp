#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string t;
    if (!(cin >> t)) return 0;
    int n = (int)t.size();
    int q;
    cin >> q;
    for (int qq = 0; qq < q; ++qq) {
        char type;
        cin >> type;
        if (type == '!') {
            int i;
            cin >> i;
            --i;
            t[i] = (t[i] == 'a' ? 'b' : 'a');
        } else { // '?'
            int l, r;
            cin >> l >> r;
            --l; --r;
            int m = r - l + 1;
            // count total a and b
            int totalA = 0, totalB = 0;
            for (int i = l; i <= r; ++i) {
                if (t[i] == 'a') ++totalA;
                else ++totalB;
            }
            // compute layer numbers L[i] and copy characters
            vector<int> L(m);
            vector<char> s(m);
            int cntA = 0, cntB = 0;
            int maxL = 0;
            for (int idx = 0; idx < m; ++idx) {
                char c = t[l + idx];
                s[idx] = c;
                if (c == 'a') {
                    ++cntA;
                    int rank = cntA;
                    int val = min(rank, totalA - rank + 1);
                    L[idx] = val;
                } else {
                    ++cntB;
                    int rank = cntB;
                    int val = min(rank, totalB - rank + 1);
                    L[idx] = val;
                }
                if (L[idx] > maxL) maxL = L[idx];
            }
            // binary search minimal k
            int lo = 0, hi = maxL;
            while (lo < hi) {
                int mid = (lo + hi) >> 1;
                // check palindrome after removing all L <= mid
                int i = 0, j = m - 1;
                bool ok = true;
                while (i < j) {
                    while (i < j && L[i] <= mid) ++i;
                    while (i < j && L[j] <= mid) --j;
                    if (i >= j) break;
                    if (s[i] != s[j]) {
                        ok = false;
                        break;
                    }
                    ++i;
                    --j;
                }
                if (ok) hi = mid;
                else lo = mid + 1;
            }
            cout << lo << '\n';
        }
    }
    return 0;
}
