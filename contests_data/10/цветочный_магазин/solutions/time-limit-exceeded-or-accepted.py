n, q = map(int, input().split())
a = list(map(int, input().split()))
b = list(map(int, input().split()))
x = list(map(int, input().split()))
for i in range(q):
    c = a.copy()
    left = x[i]
    cnt = 0
    while True:
        ok = True
        for j in range(n):
            if c[j] >= b[j]:
                c[j] -= b[j]
            else:
                need = b[j] - c[j]
                if left >= need:
                    left -= need
                    c[j] = 0
                else:
                    ok = False
                    break
        if not ok:
            break
        cnt += 1
    print(cnt, end=" ")
