import sys
input = sys.stdin.readline

MOD = 10 ** 9 + 7

def fall(x, k):
    if k > x:
        return 0
    res = 1
    for i in range(k):
        res = res * (x - i) % MOD
    return res

n, L = map(int, input().split())

# Эта программа предназначена только для подзадачи:
# у каждого слова первая и последняя буква различны.
cnt = [[0] * 26 for _ in range(26)]
for _ in range(n):
    s = input().strip()
    a = ord(s[0]) - 97
    b = ord(s[-1]) - 97
    if a > b:
        a, b = b, a
    cnt[a][b] += 1

ways = [[[0] * 5 for _ in range(26)] for _ in range(26)]
for i in range(26):
    for j in range(26):
        ways[i][j][0] = 1
for i in range(26):
    for j in range(i + 1, 26):
        for k in range(1, 5):
            ways[i][j][k] = ways[j][i][k] = fall(cnt[i][j], k)

ans = 0

# 1) Все 5 букв различны.
for a in range(26):
    for b in range(26):
        if b == a:
            continue
        ab = ways[a][b][1]
        if ab == 0:
            continue
        for c in range(26):
            if c == a or c == b:
                continue
            bc = ways[b][c][1]
            if bc == 0:
                continue
            for d in range(26):
                if d == a or d == b or d == c:
                    continue
                cd = ways[c][d][1]
                da = ways[d][a][1]
                if cd == 0 or da == 0:
                    continue
                base = ab * bc % MOD * cd % MOD * da % MOD
                for t in range(26):
                    if t == a or t == b or t == c or t == d:
                        continue
                    cur = base
                    cur = cur * ways[a][t][1] % MOD
                    cur = cur * ways[b][t][1] % MOD
                    cur = cur * ways[c][t][1] % MOD
                    cur = cur * ways[d][t][1] % MOD
                    ans += cur
                    if ans >= MOD:
                        ans -= MOD

# 2) A = C = x.
for x in range(26):
    for b in range(26):
        if b == x:
            continue
        xb2 = ways[x][b][2]
        if xb2 == 0:
            continue
        for d in range(26):
            if d == x or d == b:
                continue
            xd2 = ways[x][d][2]
            if xd2 == 0:
                continue
            base = xb2 * xd2 % MOD
            for t in range(26):
                if t == x or t == b or t == d:
                    continue
                cur = base
                cur = cur * ways[x][t][2] % MOD
                cur = cur * ways[b][t][1] % MOD
                cur = cur * ways[d][t][1] % MOD
                ans += cur
                if ans >= MOD:
                    ans -= MOD

# 3) B = D = y.
for y in range(26):
    for a in range(26):
        if a == y:
            continue
        ay2 = ways[a][y][2]
        if ay2 == 0:
            continue
        for c in range(26):
            if c == y or c == a:
                continue
            cy2 = ways[c][y][2]
            if cy2 == 0:
                continue
            base = ay2 * cy2 % MOD
            for t in range(26):
                if t == y or t == a or t == c:
                    continue
                cur = base
                cur = cur * ways[y][t][2] % MOD
                cur = cur * ways[a][t][1] % MOD
                cur = cur * ways[c][t][1] % MOD
                ans += cur
                if ans >= MOD:
                    ans -= MOD

# 4) A = C = x и B = D = y.
for x in range(26):
    for y in range(26):
        if y == x:
            continue
        xy4 = ways[x][y][4]
        if xy4 == 0:
            continue
        for t in range(26):
            if t == x or t == y:
                continue
            cur = xy4
            cur = cur * ways[x][t][2] % MOD
            cur = cur * ways[y][t][2] % MOD
            ans += cur
            if ans >= MOD:
                ans -= MOD

print(ans % MOD)