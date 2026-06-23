from math import log, exp

t = 1

for _ in range(t):
    W, X, A, B = map(float, input().split())

    if X == W:
        print(1)
        continue

    q = (W - X) / W
    log_q = log(q)
    target = log(A) - log(B)

    left, right = 0, 1
    while right * log_q >= target:
        right *= 2

    while left + 1 < right:
        mid = (left + right) // 2
        if mid * log_q < target:
            right = mid
        else:
            left = mid

    k = right

    log_y = log(W) + log(B) - log(A) + k * log_q
    log_x = log(X)

    m = int(exp(log_y - log_x))
    EPS = 1e-15

    while log(m + 1) + log_x <= log_y + EPS:
        m += 1
    while m > 0 and log(m) + log_x > log_y + EPS:
        m -= 1

    print(k + m)