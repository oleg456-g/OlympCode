#include <bits/stdc++.h>
using namespace std;

// count of leap years in [0, N] (N may be -1)
long long leap(long long N) {
    if (N < 0) return 0;
    return N / 4 - N / 100 + N / 400;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    long long L, R;
    if (!(cin >> L >> R)) return 0;
    
    long long ans = leap(R) - leap(L - 1);
    cout << ans << '\n';
    return 0;
}
