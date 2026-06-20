#pragma GCC optimize("O3,unroll-loops")

#include <bits/stdc++.h>

using namespace std;

#define BORD 1'000'000'000'000'000

int solve(long long n) {
    int ans = 0;
    for (long long k = 1; k * k <= n; ++k) {
        if (n % k == 0 && n / k >= BORD) ++ans;
    }
    return ans;
}

int main() {
    ios_base::sync_with_stdio(false), cin.tie(NULL);
    int t; cin >> t;
    while (t--) {
        long long n; cin >> n;
        cout << solve(n) << '\n';
    }
}