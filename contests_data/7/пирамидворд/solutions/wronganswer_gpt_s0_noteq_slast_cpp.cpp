#include <bits/stdc++.h>
using namespace std;

static const long long MOD = 1000000007LL;

long long falling(long long x, int k) {
    if (k > x) return 0;
    long long res = 1;
    for (int i = 0; i < k; ++i) {
        res = res * ((x - i) % MOD) % MOD;
    }
    return res;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, L;
    cin >> n >> L;

    // Эта программа предназначена только для подзадачи:
    // у каждого слова первая и последняя буква различны.
    vector<vector<long long>> cnt(26, vector<long long>(26, 0));

    for (int i = 0; i < n; ++i) {
        string s;
        cin >> s;
        int a = s.front() - 'a';
        int b = s.back() - 'a';
        if (a > b) swap(a, b);
        cnt[a][b]++;
    }

    long long ways[26][26][5]{};
    for (int i = 0; i < 26; ++i)
        for (int j = 0; j < 26; ++j)
            ways[i][j][0] = 1;

    for (int i = 0; i < 26; ++i) {
        for (int j = i + 1; j < 26; ++j) {
            for (int k = 1; k <= 4; ++k) {
                ways[i][j][k] = ways[j][i][k] = falling(cnt[i][j], k);
            }
        }
    }

    long long ans = 0;

    // 1) Все пять букв различны.
    for (int a = 0; a < 26; ++a)
    for (int b = 0; b < 26; ++b) if (b != a) {
        long long ab = ways[a][b][1];
        if (ab == 0) continue;

        for (int c = 0; c < 26; ++c) if (c != a && c != b) {
            long long bc = ways[b][c][1];
            if (bc == 0) continue;

            for (int d = 0; d < 26; ++d) if (d != a && d != b && d != c) {
                long long cd = ways[c][d][1];
                long long da = ways[d][a][1];
                if (cd == 0 || da == 0) continue;

                long long base = ab * bc % MOD * cd % MOD * da % MOD;

                for (int t = 0; t < 26; ++t) if (t != a && t != b && t != c && t != d) {
                    long long cur = base;
                    cur = cur * ways[a][t][1] % MOD;
                    cur = cur * ways[b][t][1] % MOD;
                    cur = cur * ways[c][t][1] % MOD;
                    cur = cur * ways[d][t][1] % MOD;
                    ans += cur;
                    if (ans >= MOD) ans -= MOD;
                }
            }
        }
    }

    // 2) A = C = x.
    for (int x = 0; x < 26; ++x)
    for (int b = 0; b < 26; ++b) if (b != x) {
        long long xb2 = ways[x][b][2];
        if (xb2 == 0) continue;

        for (int d = 0; d < 26; ++d) if (d != x && d != b) {
            long long xd2 = ways[x][d][2];
            if (xd2 == 0) continue;

            long long base = xb2 * xd2 % MOD;

            for (int t = 0; t < 26; ++t) if (t != x && t != b && t != d) {
                long long cur = base;
                cur = cur * ways[x][t][2] % MOD;
                cur = cur * ways[b][t][1] % MOD;
                cur = cur * ways[d][t][1] % MOD;
                ans += cur;
                if (ans >= MOD) ans -= MOD;
            }
        }
    }

    // 3) B = D = y.
    for (int y = 0; y < 26; ++y)
    for (int a = 0; a < 26; ++a) if (a != y) {
        long long ay2 = ways[a][y][2];
        if (ay2 == 0) continue;

        for (int c = 0; c < 26; ++c) if (c != y && c != a) {
            long long cy2 = ways[c][y][2];
            if (cy2 == 0) continue;

            long long base = ay2 * cy2 % MOD;

            for (int t = 0; t < 26; ++t) if (t != y && t != a && t != c) {
                long long cur = base;
                cur = cur * ways[y][t][2] % MOD;
                cur = cur * ways[a][t][1] % MOD;
                cur = cur * ways[c][t][1] % MOD;
                ans += cur;
                if (ans >= MOD) ans -= MOD;
            }
        }
    }

    // 4) A = C = x и B = D = y.
    for (int x = 0; x < 26; ++x)
    for (int y = 0; y < 26; ++y) if (y != x) {
        long long xy4 = ways[x][y][4];
        if (xy4 == 0) continue;

        for (int t = 0; t < 26; ++t) if (t != x && t != y) {
            long long cur = xy4;
            cur = cur * ways[x][t][2] % MOD;
            cur = cur * ways[y][t][2] % MOD;
            ans += cur;
            if (ans >= MOD) ans -= MOD;
        }
    }

    cout << ans % MOD << '\n';
    return 0;
}