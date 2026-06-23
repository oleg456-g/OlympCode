import sys
input = sys.stdin.readline

MOD = 10 ** 9 + 7

n, L = map(int, input().split())
words = [input().strip() for _ in range(n)]

if n < 8:
    print(0)
    raise SystemExit

# Ребра в порядке с хорошим ранним отсечением:
# AB, AT, BT, BC, CT, CD, DT, DA
order = [(0, 1), (0, 4), (1, 4), (1, 2), (2, 4), (2, 3), (3, 4), (3, 0)]

opts = []
for w in words:
    r = w[::-1]
    cur = [w]
    if r != w:
        cur.append(r)
    opts.append(cur)

used = [False] * n
vertex = [None] * 5
ans = [0]

def dfs(pos):
    if pos == 8:
        ans[0] += 1
        if ans[0] >= MOD:
            ans[0] -= MOD
        return

    u, v = order[pos]
    for i in range(n):
        if used[i]:
            continue
        for s in opts[i]:
            a, b = s[0], s[-1]

            if vertex[u] is not None and vertex[u] != a:
                continue
            if vertex[v] is not None and vertex[v] != b:
                continue

            old_u, old_v = vertex[u], vertex[v]
            vertex[u], vertex[v] = a, b
            used[i] = True

            dfs(pos + 1)

            used[i] = False
            vertex[u], vertex[v] = old_u, old_v

dfs(0)
print(ans[0] % MOD)