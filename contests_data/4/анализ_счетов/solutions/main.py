t = int(input())
while t:
    t -= 1
    n = int(input())
    ans = 0
    for j in range(1, 1001):
        if n % j == 0:
            if n // j >= 10 ** 15:
                ans += 1
    print(ans)
