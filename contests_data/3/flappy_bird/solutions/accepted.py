
t = int(input())
results = []

for _ in range(t):
    n, h, k, s = map(int, input().split())
    segs = [tuple(map(int, input().split())) for i in range(n)]
    
    l = s
    r = s
    assert segs[0][0] < l and r < segs[0][1]
    ok = True
    
    for i in range(1, n):
        l -= 1
        r += k
        tl = -1
        tr = 10**9
        
        while tl + 1 < tr:
            tm = (tl + tr) // 2
            if l + (k + 1) * tm < segs[i][1]:
                tl = tm
            else:
                tr = tm
        
        if tl == -1:
            ok = False
            break
        
        r = min(r, l + (k + 1) * tl)
        tl = -1
        tr = 10**9
        
        while tl + 1 < tr:
            tm = (tl + tr) // 2
            if r - (k + 1) * tm > segs[i][0]:
                tl = tm
            else:
                tr = tm
        
        if tl == -1:
            ok = False
            break
        
        l = max(l, r - (k + 1) * tl)
        assert l <= r
    
    if ok:
        results.append("Yes")
    else:
        results.append("No")

print("\n".join(results))
