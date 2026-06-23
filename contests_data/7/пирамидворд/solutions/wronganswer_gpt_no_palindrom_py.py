import sys
input = sys.stdin.readline

MOD = 10 ** 9 + 7
EDGES = [(0, 1), (1, 2), (2, 3), (3, 0), (0, 4), (1, 4), (2, 4), (3, 4)]

def generate_partitions():
    parts = []

    def rec(v, blocks):
        if v == 5:
            cls = [0] * 5
            for i, block in enumerate(blocks):
                for x in block:
                    cls[x] = i
            parts.append(tuple(cls))
            return
        for block in blocks:
            block.append(v)
            rec(v + 1, blocks)
            block.pop()
        blocks.append([v])
        rec(v + 1, blocks)
        blocks.pop()

    rec(0, [])
    return parts

def fall(x, k):
    if k > x:
        return 0
    res = 1
    for i in range(k):
        res = res * (x - i) % MOD
    return res

PARTS = []
for cls in generate_partitions():
    b = max(cls) + 1
    loops = [0] * b
    mult = [[0] * b for _ in range(b)]
    deg = [0] * b

    for u, v in EDGES:
        a, c = cls[u], cls[v]
        if a == c:
            loops[a] += 1
            deg[a] += 2
        else:
            mult[a][c] += 1
            mult[c][a] += 1
            deg[a] += 1
            deg[c] += 1

    order = sorted(range(b), key=lambda i: (-deg[i], -loops[i]))
    loops2 = [loops[i] for i in order]
    prev = []
    for i in range(b):
        cur = []
        for j in range(i):
            m = mult[order[j]][order[i]]
            if m:
                cur.append((j, m))
        prev.append(cur)
    PARTS.append((b, loops2, prev))

n, L = map(int, input().split())

# Предполагается, что палиндромов нет.
cnt_diff = [[0] * 26 for _ in range(26)]   # для x < y
cnt_same = [0] * 26                        # слова вида x ... x

for _ in range(n):
    s = input().strip()
    a = ord(s[0]) - 97
    b = ord(s[-1]) - 97
    if a == b:
        cnt_same[a] += 1
    else:
        if a > b:
            a, b = b, a
        cnt_diff[a][b] += 1

diff = [[[0] * 9 for _ in range(26)] for _ in range(26)]
for i in range(26):
    for j in range(26):
        diff[i][j][0] = 1
for i in range(26):
    for j in range(i + 1, 26):
        for k in range(1, 9):
            diff[i][j][k] = diff[j][i][k] = fall(cnt_diff[i][j], k)

pow2 = [1] * 9
for i in range(1, 9):
    pow2[i] = pow2[i - 1] * 2 % MOD

same = [[0] * 9 for _ in range(26)]
for x in range(26):
    same[x][0] = 1
    for k in range(1, 9):
        same[x][k] = fall(cnt_same[x], k) * pow2[k] % MOD

used = [False] * 26
color = [0] * 5
ans = [0]

def dfs(pos, b, loops, prev, cur):
    if pos == b:
        ans[0] += cur
        if ans[0] >= MOD:
            ans[0] -= MOD
        return

    need_loop = loops[pos]
    links = prev[pos]
    for x in range(26):
        if used[x]:
            continue
        ways = same[x][need_loop]
        if ways == 0:
            continue

        ok = True
        for j, m in links:
            add = diff[x][color[j]][m]
            if add == 0:
                ok = False
                break
            ways = ways * add % MOD

        if not ok:
            continue

        used[x] = True
        color[pos] = x
        dfs(pos + 1, b, loops, prev, cur * ways % MOD)
        used[x] = False

for b, loops, prev in PARTS:
    dfs(0, b, loops, prev, 1)

print(ans[0] % MOD)