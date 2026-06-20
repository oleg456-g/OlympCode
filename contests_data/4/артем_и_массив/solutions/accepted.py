K = 30
for _ in range(int(input())):
    n = int(input())
    a = list(map(int,input().split()))
    if n == 1:
        print(0)
        continue
    acts =[]
    for i in range((n-1)//2, n-1):
        x = a[i] ^ a[i+1]
        acts.append((1, i+1, x))
    for i in range(1, (n-1)//2 + 1):
        x = a[i-1] ^ a[i]
        acts.append((i+1, n, x))
    print(len(acts))
    for x in acts:
        print(' '.join(map(str, x)))