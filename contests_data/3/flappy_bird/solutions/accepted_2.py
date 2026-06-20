t = int(input())

for test_id in range(t):
    n, h, k, s = map(int, input().split())
    a = []
    for i in range(n):
        l, r = map(int, input().split())
        a.append((l, r))
    curl = curr = s
    for i in range(1, n):
        curl -= 1
        curr += k
        while curl <= a[i][0]:
            curl += k + 1
        while curr >= a[i][1]:
            curr -= k + 1
        if curl > curr:
            print("No")
            break
    else:
        print("Yes")
