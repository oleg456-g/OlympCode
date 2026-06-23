n, q = map(int, input().split())
a = list(map(int, input().split()))
b = list(map(int, input().split()))
x = list(map(int, input().split()))
max_b = 0
id = -1
for i in range(n):
    if max_b < b[i]:
        max_b = b[i]
        id = i
for i in range(q):
    l = 0
    r = (x[i] + a[id]) // max_b + 1
    while l + 1 < r:
        mid = (l + r) // 2
        left = x[i]
        ok = True
        for j in range(n):
            if b[j] * mid > a[j]:
                need = b[j] * mid - a[j]
                if need > left:
                    ok = False
                    break
                left -= need
        if ok:
            l = mid
        else:
            r = mid
    print(l, end=' ')
