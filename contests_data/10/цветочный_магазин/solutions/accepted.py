n, q = map(int, input().split())
a = list(map(int, input().split()))
b = [1] * n
x = list(map(int, input().split()))
div = [0] * n
need = [0] * n
pref_b = [0] * (n + 1)
answers = [0] * q
id = []
for i in range(n):
    if b[i] == 0:
        continue
    id.append(i)
    div[i] = a[i] // b[i]
    need[i] = (div[i] + 1) * b[i] - a[i]
id.sort(key=lambda i: div[i])
ans = 0
now = 0
mp = []
pref_b[0] = 0
for i in range(len(id)):
    ind = id[i]
    cnt = div[ind] + 1 - now
    ans += pref_b[i] * cnt + need[ind]
    now = div[ind] + 1
    pref_b[i + 1] = pref_b[i] + b[ind]
    if i == len(id) - 1 or div[id[i]] != div[id[i + 1]]:
        mp.append((ans, (i, div[ind] + 1)))
for i in range(q):
    l = -1
    r = len(mp)
    while l + 1 < r:
        mid = (l + r) // 2
        if mp[mid][0] > x[i]:
            r = mid
        else:
            l = mid
    it = r
    limit = 2 * 10**18
    if it != len(mp):
        limit = mp[it][1][1]
    if it == 0:
        answers[i] = limit - 1
        continue
    it -= 1
    o = mp[it][1][1]
    ind = mp[it][1][0]
    left = x[i] - mp[it][0]
    o += left // pref_b[ind + 1]
    answers[i] = min(o, limit - 1)
for i in range(q):
    print(answers[i], end=" ")
