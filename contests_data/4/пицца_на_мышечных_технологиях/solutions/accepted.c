#include <stdio.h>
#include <stdbool.h>

#define MAXN 200200

int n, k;
int arr[MAXN];

bool check(long long tm) {
    int i = 0, j = n - 1;
    {
        long long tt = tm;
        for (; i < n && arr[i] < tt; ++i)
            tt -= arr[i];    
    }
    {
        long long tt = tm;
        for (; j >= 0 && arr[j] < tt; --j)
            tt -= arr[j];
    }
    return j < i || j - i <= k;
}

long long solve() {
    long long l = 0, r = 1e17;
    while (r - l > 1) {
        long long mid = (l + r) >> 1LL;
        if (check(mid)) r = mid;
        else l = mid;
    }
    return r;
}

int main() {
    int t; scanf("%d", &t);
    while (t--) {
        scanf("%d%d", &n, &k);
        for (int i = 0; i < n; ++i)
            scanf("%d", &arr[i]);
        printf("%lld\n", solve());
    }
}