def solve():
    n, k = map(int, input().split())
    a = list(map(int, input().split()))
    INF = 10**18 + 228
    l, r = 0, INF
    
    while r - l > 1:
        mid = (l + r) // 2
        ind1, ind2 = n, -1
        sum1, sum2 = mid, mid
        
        for i in range(n):
            if a[i] >= sum1:
                ind1 = i
                break
            else:
                sum1 -= a[i]
        
        for i in range(n - 1, -1, -1):
            if a[i] >= sum2:
                ind2 = i
                break
            else:
                sum2 -= a[i]
        
        if ind2 - ind1 <= k:
            r = mid
        else:
            l = mid
            
    print(r)

t = int(input())
for _ in range(t):
    solve()

