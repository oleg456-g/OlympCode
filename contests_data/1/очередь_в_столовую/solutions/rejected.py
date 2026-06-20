import sys

def solve():
    data = sys.stdin.readline().split()
    n = int(data[0])
    d = int(data[1])

    t = [0] * (n + 1)
    k = [0] * (n + 1)
    pref = [0] * (n + 1)

    for i in range(1, n + 1):
        data_line = sys.stdin.readline().split()
        t[i] = int(data_line[0])
        k[i] = int(data_line[1])
        pref[i] = pref[i - 1] + k[i - 1]


    ans = n + 1
    for i in range(n, 0, -1):
        if pref[i] + d < t[i]:
            ans = i
        else:
            break
    print(ans)


test = int(input())
while test:
    solve()
    test -= 1