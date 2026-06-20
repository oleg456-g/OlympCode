t = int(input())
for test in range(t):
    n = int(input())
    s = list(map(int, input().split()))
    m = int(input())
    b = list(map(int, input().split()))
    s.sort(reverse=True)
    b.sort(reverse=True)
    now_b = 0
    ans = 0
    for sell in s:
        if now_b == m:
            break
        if sell <= b[now_b]:
            ans += b[now_b]
            now_b += 1
    print(ans)