#include <stdio.h>
#include <stdbool.h>

#define MAXN 6
#define MAXM 10100

int n, m;
char A[MAXN][MAXM];
int dp[MAXM][1 << MAXN];

bool gorilla(int mask, int cmask) {
    int cp = 0, cb = 0;
    for (int i = 0; i < n; ++i) {
        int bp = cmask >> i & 1;
        int bb = mask >> i & 1;
        if (bp && bb) {
            if (cp ^ cb) return false;
        } else if (bp && !bb) {
            if (cp && cb) return false;
        } else if (!bp && bb) {
            if (cp && cb) return false;
        }
        cp = bp;
        cb = bb;
    }
    return true;
}

inline int max(int a, int b) {
    return (a > b ? a : b);
}

bool check(int j, int mask) {
    for (int i = 0; i < n; ++i) {
        if ((mask >> i & 1) && A[i][j] == '1')
            return false;
    }
    for (int cmask = 0; cmask < (1 << n); ++cmask) {
        if (gorilla(mask, cmask))
            dp[j + 1][mask] = max(dp[j + 1][mask], dp[j][cmask] + __builtin_popcount(mask));
    }
    return true;
}

void build() {
    for (int j = 0; j <= m; ++j)
        memset(dp[j], 0, sizeof(dp[j]));
    for (int j = 0; j < m; ++j) {
        for (int mask = 0; mask < (1 << n); ++mask) {
            if (!check(j, mask)) continue;
            
        }
    }
}

int solve() {
    int ans = 0;
    for (int mask = 0; mask < (1 << n); ++mask)
        ans = max(ans, dp[m][mask]);
    return ans;
}

int main() {
    int t; scanf("%d", &t);
    while (t--) {
        scanf("%d%d", &n, &m);
        for (int i = 0; i < n; ++i)
            scanf("%s", A + i);
        build();
        printf("%d\n", solve());
    }
}