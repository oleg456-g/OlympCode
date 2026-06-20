#include <bits/stdc++.h>

using namespace std;

#define MAXN 200200

int n;
int arr[MAXN], brr[MAXN << 1];

void solve() {
    sort(brr, brr + (n << 1));
    multiset<int> M(brr, brr + (n << 1));
    int ps = 0;
    for (int i = 0; i < n; ++i) {
        arr[i] = *M.begin();
        M.erase(M.begin());
        ps += arr[i];
        auto it = M.find(ps);
        assert(it != M.end());
        M.erase(it);
    }
}

int main() {
    int t; cin >> t;
    while (t--) {
        cin >> n;
        for (int i = 0; i < (n << 1); ++i)
            cin >> brr[i];
        solve();
        for (int i = 0; i < n; ++i)
            cout << arr[i] << ' ';
        cout << '\n';
    }
}