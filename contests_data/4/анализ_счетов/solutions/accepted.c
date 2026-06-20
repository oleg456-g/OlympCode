#include <stdio.h>

#define BORD 1000000000000000

int solve(long long n) {
    int ans = 0;
    for (long long k = 1; k < 2000; ++k) {
        if (n % k == 0 && n / k >= BORD) ++ans;
    }
    return ans;
}

int main() {
    int t; scanf("%d", &t);
    while (t--) {
        long long n; scanf("%lld", &n);
        printf("%d\n", solve(n));
    }
}