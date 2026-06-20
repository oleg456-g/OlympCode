import math

n = int(input())
a = list(map(int, input().split()))

ans, pnt, pnt_L = 1, 0, 0
d = 1
while pnt < n:
    cur = 1
    for i in range(pnt, max(pnt_L, pnt - 10) - 1, -1):
        cur *= a[i]
        d = math.isqrt(cur)
        if d * d == cur:
            ans += 1
            pnt_L = pnt
            break
    pnt += 1

print(ans)
